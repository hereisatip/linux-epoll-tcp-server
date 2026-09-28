#include"epoll_utils.hpp"
#include"listen_socket.hpp"

#include<sys/epoll.h>
#include<unistd.h>
#include<iostream>
using namespace std;

int main()
{
    

    int epoll_fd=create_epoll();
    if(epoll_fd!=-1)
    {
        cout<<"YES"<<endl;
    }
    int listen_fd=create_listen_socket(0);
    if(listen_fd!=-1)
    {
        cout<<"YES"<<endl;
    }


    if(add_epoll_fd(epoll_fd,listen_fd,EPOLLIN | EPOLLET))
    {
        cout<<"YES"<<endl;
    }

    epoll_event event{};
    event.events=EPOLLIN;
    event.data.fd=listen_fd;

    epoll_wait(epoll_fd,&event,1,0);

    if(modify_epoll_fd(epoll_fd,listen_fd,EPOLLIN))
    {
        cout<<"YES"<<endl;
    }

    if(remove_epoll_fd(epoll_fd,listen_fd))
    {
        cout<<"YES"<<endl;
    }

    close(epoll_fd);
    close(listen_fd);

    return 0;
}