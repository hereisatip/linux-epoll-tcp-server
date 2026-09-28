#include"listen_socket.hpp"
#include"socket_utils.hpp"

#include<iostream>
using namespace std;

#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

int main()
{
    int server_fd=create_listen_socket(0);
    if(server_fd>=0)
    {
        cout<<"创建端口成功"<<endl;
    }
    else
    {
        cout<<"创建端口失败"<<endl;
    }

    if(set_nonblocking(server_fd)&&(fcntl(server_fd,F_GETFL,0)&O_NONBLOCK))
    {
        cout<<"非阻塞设置成功"<<endl;
    }
    else
    {
        cout<<"非阻塞设置失败"<<endl;
    }
    
    sockaddr_in address{};
    socklen_t address_length=sizeof(address);

    int s_fd=getsockname(server_fd,reinterpret_cast<sockaddr*>(&address),&address_length);

    if(s_fd==0)
    {
        cout<<"获取fd成功"<<endl;
    }
    else
    {
        cout<<"获取fd失败"<<endl;
    }

    close(server_fd);

    return 0;
}