#include"socket_utils.hpp"

#include<fcntl.h>

bool set_nonblocking(int fd)
{
    //FL file status flags
    //Fd file descriptor flags

    //读取当前的文件状态标志
    //可能有O_RDONLY
    //O_WRONLY
    //O_RDWR
    //O_APPEND
    //O_NONBLOCK
    //F_GETFL表示获取
    int flag=fcntl(fd,F_GETFL,0);

    if(flag==-1)
    {
        return false;
    }

    //保留原有标志,只增加O_NONBLOCK
    //F_SETFL表示设置,第三个变量是位标志集合,每一位表示一个状态
    if(fcntl(fd,F_SETFL,flag | O_NONBLOCK)==-1)
    {
        return false;
    }


    return true;
}