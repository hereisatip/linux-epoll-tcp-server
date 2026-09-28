#ifndef LISTEN_SOCKET_HPP
#define LISTEN_SOCKET_HPP

#include<cstdint>

//创建并初始化TCP监听socket
//port:监听端口
//返回值: >=0:成功,返回监听socket fd;-1:失败
int create_listen_socket(short port);

#endif