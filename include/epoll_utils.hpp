#ifndef EPOLL_UTILS_HPP
#define EPOLL_UTILS_HPP

#include<cstdint>

//创建epoll实例
//成功返回epoll fd,失败返回-1
int create_epoll();

//向epoll注册一个fd
//events时EPOLLIN、EPOLLOUT、EPOLLET等事件组合
bool add_epoll_fd(int epoll_fd,int monitored_fd,uint32_t events);

//修改epoll中已有fd的监听事件
bool modify_epoll_fd(int epoll_fd,int monitored_fd,uint32_t events);

//从epoll中删除fd
bool remove_epoll_fd(int epoll_fd,int monitored_fd);



#endif