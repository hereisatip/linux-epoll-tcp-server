#include"server.hpp"
#include"listen_socket.hpp"
#include"epoll_utils.hpp"
#include"socket_utils.hpp"
#include"protocol.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include<chrono>
#include<vector>

#include<csignal>

#include<termios.h>

#include<sys/eventfd.h>

#include<thread>

using namespace std;

volatile sig_atomic_t stop_request=0;

void handle_signal(int signal_number)
{

    if(signal_number==SIGINT||signal_number==SIGTERM)
    {
        stop_request=1;
    }
}


Server::Server(short _port,size_t worker_count):port(_port),thread_pool(worker_count,64)
{

}

Server::~Server()
{
    cleanup();
}

bool Server::initialize()
{
    listen_fd=create_listen_socket(port);

    if(listen_fd==-1)
    {
        return false;
    }

    epoll_fd=create_epoll();

    if (epoll_fd == -1)
    {
        close(listen_fd);
        listen_fd = -1;
        return false;
    }

    result_event_fd=eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);

    if(result_event_fd==-1)
    {
        close(epoll_fd);
        close(listen_fd);
        epoll_fd = -1;
        listen_fd = -1;
        cerr << "eventfd failed: " << strerror(errno) << '\n';
        return false;
    }

    epoll_event result_event{};

    result_event.events = EPOLLIN | EPOLLET;

    result_event.data.fd = result_event_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, result_event_fd, &result_event) == -1)
    {
        cerr << "epoll_ctl add result event failed: " <<strerror(errno) << '\n';
        return false;
    }


    if(!add_epoll_fd(epoll_fd,listen_fd,EPOLLIN | EPOLLET))
    {
        perror("epoll_ctl_add");
        close_client(epoll_fd);
        close_client(listen_fd);
        epoll_fd=-1;
        listen_fd=-1;
        return false;
    }
    signal(SIGINT,handle_signal);
    signal(SIGTERM,handle_signal);

    return true;
}

void Server::accept_clients()
{
    while(true)
    {
        sockaddr_in sa{};
        socklen_t client_address_length=sizeof(sa);

        //非阻塞监听,ET模式直到EAGAIN才能退出
        int client_fd=accept(listen_fd,(sockaddr*)&sa,&client_address_length);

        if(client_fd==-1)
        {
            if(errno==EAGAIN || errno==EWOULDBLOCK)
            {
                break;
            }
            if(errno==EINTR)
            {
                continue;
            }
            cerr<<"accept fail: "<<strerror(errno)<<endl;
            break;
        }

        if(!set_nonblocking(client_fd))
        {
            cerr<<"nonblock fail"<<strerror(errno)<<endl;
            close_client(client_fd);
            continue;
        }

        if(!connections.add(client_fd))
        {
            cerr<<"connections.add fail"<<strerror(errno)<<endl;
            close_client(client_fd);
            continue;
        }

        
        if(!add_epoll_fd(epoll_fd,client_fd,EPOLLIN | EPOLLET | EPOLLRDHUP))
        {
            cerr<<"add_epoll_event fail"<<strerror(errno)<<endl;
            close_client(client_fd);
            continue;
        }

        cout<<"client accept,fd= "<<client_fd<<endl;
    }

}

void Server::run(size_t timeout_ms)
{
    constexpr int max_event=16;
    epoll_event events[max_event];

    while(running&&!stop_request)
    {
        //有几个fd就绪
    int count=epoll_wait(epoll_fd,events,max_event,timeout_ms);

    if (count == -1)
    {
        if (errno == EINTR)
        {
            continue;
        }
        std::cerr<< "epoll_wait failed: "<< std::strerror(errno)<< '\n';
        break;
    }

    for(int i=0;i<count;i++)
        {
            int ready_fd=events[i].data.fd;

            if(ready_fd==listen_fd)
            {
                accept_clients();
            }
            else if(ready_fd==result_event_fd)
            {
                handle_result_event();
            }
            else
            {
                handle_client_event(ready_fd,events[i].events);
            }
        }
        process_complete_results();
        handle_timeout();
    }
    cout<<"run is over,cleaning up"<<endl;
}

void Server::cleanup()
{
    if(listen_fd==-1&&epoll_fd==-1&&connections.size()==0)
    {
        return;
    }

    running=false;

    vector<int> remain_clients=connections.file_descriptors();
    for(int fd:remain_clients)
    {
        close_client(fd);
    }

    connections.clear();

    if(result_event_fd!=-1)
    {
        close(result_event_fd);
        result_event_fd=-1;
    }

    if(epoll_fd!=-1)
    {
        close(epoll_fd);
        epoll_fd=-1;
    }

    if(listen_fd!=-1)
    {
        close(listen_fd);
        listen_fd=-1;
    }

    cout<<"server clean is over"<<endl;
}

void Server::process_complete_results()
{
    queue<BusinessResult> results;
    {
        lock_guard<mutex> lock(result_mutex);

        results.swap(complete_results);
    }

    while(!results.empty())
    {
        BusinessResult result=move(results.front());
        results.pop();

        ClientSession* client=connections.find(result.client_fd);
        if(client==nullptr)
        {
            continue;
        }

        if(client->connection_id!=result.connection_id)
        {
            continue;
        }

        client->output_buffer+=result.response;

        epoll_event client_event{};

        client_event.events =EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;

        client_event.data.fd = result.client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, result.client_fd, &client_event) == -1)
        {
             std::cerr << "epoll_ctl enable EPOLLOUT ""for result failed: "<< std::strerror(errno)<< '\n';

            close_client(result.client_fd);
        }
    }
}

void Server::handle_result_event()
{
    while(true)
    {
        int64 notify=0;

        int count=read(result_event_fd,&notify,sizeof(notify));
        if(count==sizeof(notify))
        {
            continue;
        }

        if(count==-1&&errno==EINTR)
        {
            continue;
        }

        if(count==-1&&(errno==EAGAIN||errno==EWOULDBLOCK))
        {
            break;
        }
        if(count==0)
        {
            break;
        }

        cerr<<"read result event fail: "<<strerror(errno)<<endl;
        break;
    }
    process_complete_results();
}

void Server::close_client(int fd)
{
    if(!connections.find(fd))
    {
        return;
    }

    if(epoll_fd!=-1)
    {
        if(!remove_epoll_fd(epoll_fd,fd))
        {
            cerr<<"remove epoll fail"<<strerror(errno)<<endl;
        }
    }
    

    close(fd);

    if(!connections.remove(fd))
    {
        cerr<<"remove "<<fd<<" fail"<<strerror(errno)<<endl;
    }

    cout<<"client closed,fd= "<<fd<<endl;
}

void Server::handle_client_event(int client_fd,size_t event)
{
    if(event& EPOLLERR)
    {
        close_client(client_fd);
        return;
    }

    if(event&EPOLLIN)
    {
        //读事件
        handle_read(client_fd);

        if(connections.find((size_t)client_fd)==nullptr)
        {
            close(client_fd);
            return;
        }
    }

    if(event&EPOLLHUP||event&EPOLLRDHUP)
    {
        close_client(client_fd);
        return;
    }

    if((event&EPOLLOUT)&&connections.find((size_t)client_fd)!=nullptr)
    {
        //写事件
        handle_write(client_fd);
    }

}

void Server::handle_read(int client_fd)
{
    ClientSession* client=connections.find(client_fd);

    if(client==nullptr)
    {
        return;
    }

    constexpr size_t max_message_length=1024;

    while(true)
    {
        char buffer[128];
        int count=read(client_fd,buffer,sizeof(buffer));


        if(count>0)
        {
            client->last_activity=Clock::now();

            client->input_buffer.append(buffer,static_cast<size_t>(count));

            if(client->input_buffer.size()>max_message_length)
            {
                cerr<<"message too long,fd= "<<client_fd<<endl;
                close_client(client_fd);
                return;
            }

            while(true)
            {
                size_t message_end=client->input_buffer.find('\n');

                if(message_end==string::npos)
                {
                    break;
                }

                string message=client->input_buffer.substr(0,message_end);

                client->input_buffer.erase(0,message_end+1);

                if(message.empty())
                {
                    continue;
                }

                cout<<"receive from fd= "<<client_fd<<": "<<message<<endl;

                if(is_calc_command(message))
                {
                    int64 number=0;
                    int delay_ms=0;

                    if(parse_calc_request(message,number,delay_ms))
                    {
                        const int64 connection_id=client->connection_id;

                        //交给线程池
                        submit_calc_result(client_fd,connection_id,number,delay_ms);
                    }
                    else
                    {
                        client->output_buffer+=make_response(message);

                        epoll_event client_event{};

                        client_event.data.fd=client_fd;

                        client_event.events=EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLET;

                        if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,client_fd,&client_event)==-1)
                        {
                            cerr<<"epoll_ctl_mod invalid "<<"CALC response fail "<<strerror(errno)<<endl;
                            close_client(client_fd);
                        }
                    }
                }
                else
                {
                        client->output_buffer+=make_response(message);

                        epoll_event client_event{};

                        client_event.data.fd=client_fd;

                        client_event.events=EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLET;

                        if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,client_fd,&client_event)==-1)
                        {
                            cerr<<"epoll_ctl_mod invalid "<<"response fail "<<strerror(errno)<<endl;
                            close_client(client_fd);
                        }
                }
            }
            continue;
        }

        if(count==0)
        {
            close_client(client_fd);
            return;
        }

        if(errno==EINTR)
        {
            continue;
        }

        if(errno==EAGAIN||errno==EWOULDBLOCK)
        {
            break;
        }

        cerr<<"read faii "<<"fd: "<<client_fd<<strerror(errno)<<endl;

        close_client(client_fd);
        return;
    }

    if(client->output_buffer.empty())
    {
        return;
    }

    epoll_event client_event{};

    client_event.data.fd=client_fd;

    client_event.events=EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLET;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,client_fd,&client_event)==-1)
    {
        cerr<<"epoll_ctl_mod invalid "<<"response fail "<<strerror(errno)<<endl;
        close_client(client_fd);
    }

}

void Server::handle_write(int client_fd)
{

    ClientSession* client=connections.find(client_fd);

    if(client==nullptr)
    {
        return;
    }

    string& output=client->output_buffer;

    cout<<"handle write,fd= "<<client_fd<<": "<<output<<endl;


    while(!output.empty())
    {
        int count=write(client_fd,output.data(),output.size());

        if(count>0)
        {
            output.erase(0,static_cast<size_t>(count));
            continue;
        }

        if(count==-1&&errno==EINTR)
        {
            continue;
        }

        if(count==-1&&(errno==EAGAIN||errno==EWOULDBLOCK))
        {
            return;
        }

        cerr<<"write fail,fd: "<<client_fd<<" "<<strerror(errno)<<endl;

        close_client(client_fd);
        return;
    }

    epoll_event client_event{};

    client_event.data.fd=client_fd;
    client_event.events=EPOLLIN|EPOLLRDHUP|EPOLLET;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,client_fd,&client_event))
    {
        cerr<<"epoll_ctl_mod fail:"<<strerror(errno)<<endl;
        close_client(client_fd);
    }

}

void Server::notify_result_ready()
{
    const int64 notify=1;

    int count=write(result_event_fd,&notify,sizeof(notify));

    if (count == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        std::cerr << "write result event failed: " << std::strerror(errno) << '\n';
    }
}

void Server::submit_calc_result(int client_fd,int64 connection_id,int64 number,size_t delay_ms)
{
    bool accepted=thread_pool.enqueue([this,client_fd,connection_id,number,delay_ms](){
        if(delay_ms)
        {
            this_thread::sleep_for(chrono::milliseconds(delay_ms));
        }
        const int64 result=number*(number+1)/2;

        BusinessResult complete_result;

        complete_result.client_fd=client_fd;
        complete_result.connection_id=connection_id;
        complete_result.response="RESULT: "+to_string(result)+"\n";

        {
            lock_guard<mutex> lock(result_mutex);

            complete_results.push(complete_result);
        }
        //通知epoll线程
        notify_result_ready();
    });

    if(accepted)
    {
        return;
    }
    ClientSession* client=connections.find(client_fd);
    if(client==nullptr)
    {
        return;
    }

    client->output_buffer+="ERROR server busy\n";

    epoll_event client_event{};
    client_event.events =EPOLLIN |EPOLLOUT |EPOLLRDHUP |EPOLLET;

    client_event.data.fd =client_fd;

    if (epoll_ctl(epoll_fd,EPOLL_CTL_MOD,client_fd,&client_event) == -1)
    {
        std::cerr<< "epoll_ctl enable busy response ""failed: "<< std::strerror(errno)<< '\n';

        // 如果连错误响应都无法注册发送，
        // 只能关闭当前客户端。
        close_client(client_fd);
    }
}

void Server::handle_timeout()
{
    constexpr auto client_timeout=chrono::seconds(10);

    const auto now=Clock::now();

    vector<int> time_clients;

    for(int client_fd:connections.file_descriptors())
    {
        const ClientSession* client =
        connections.find(client_fd);

        if (client == nullptr)
        {
            continue;
        }

        if (now - client->last_activity >=client_timeout)
        {
            time_clients.push_back(client_fd);
        }
    }

    for(int client_fd:time_clients)
    {
        cerr<<"client timeout, fd= "<<client_fd<<endl;

        string res="连接超时,请重新连接\n";

        write(client_fd,res.data(),res.size());

        close_client(client_fd);
    }
}