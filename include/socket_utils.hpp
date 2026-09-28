#ifndef SOCKET_UTILS_HPP
#define SOCKET_UTILS_HPP

// 把文件描述符设置为非阻塞模式。
// 成功返回 true，失败返回 false。
bool set_nonblocking(int fd);

#endif