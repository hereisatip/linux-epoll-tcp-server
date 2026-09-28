#include"epoll_utils.hpp"

#include<sys/epoll.h>

#include<error.h>

#include<cstdio>

int create_epoll()
{
    int epoll_server=epoll_create1(EPOLL_CLOEXEC);
    if(epoll_server==-1)
    {
        
        return -1;
    }
    return epoll_server;
}

bool add_epoll_fd(int epoll_fd,int monitored_fd,uint32_t events)
{
    epoll_event event{};
    event.events=events;
    event.data.fd=monitored_fd;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,monitored_fd,&event)==-1)
    {
        perror("epoll_ctl_add");
        return false;
    }

    return true;
}

bool modify_epoll_fd(int epoll_fd,int monitored_fd,uint32_t events)
{
    epoll_event event{};
    event.events=events;
    event.data.fd=monitored_fd;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,monitored_fd,&event)==-1)
    {
        perror("epoll_ctl_mod");
        return false;
    }

    return true;
}

bool remove_epoll_fd(int epoll_fd,int monitored_fd)
{
    if(epoll_ctl(epoll_fd,EPOLL_CTL_DEL,monitored_fd,nullptr)==-1)
    {
        perror("epoll_ctl_del");
        return false;
    }
    return true;
}

