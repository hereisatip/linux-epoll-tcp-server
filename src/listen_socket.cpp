#include"listen_socket.hpp"

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cerrno>
#include<cstring>
#include<iostream>
#include"socket_utils.hpp"

using namespace std;

int create_listen_socket(short port)
{
    int server_fd=socket(AF_INET,SOCK_STREAM,0);

    if(server_fd==-1)
    {
        cerr<<"端口创建失败"<<endl;
        return -1;
    }
    int reuse=1;
    
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))==-1)
    {
        cerr<<"端口设置失败"<<endl;
        close(server_fd);
        return -1;
    }

    sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    sin.sin_port=htons(port);

    if(bind(server_fd,(sockaddr*)&sin,sizeof(sin))==-1)
    {
        cerr<<"端口绑定失败"<<endl;
        close(server_fd);
        return -1;
    }

    if(listen(server_fd,SOMAXCONN)==-1)
    {
        cerr<<"端口监听失败"<<endl;
        close(server_fd);
        return -1;
    }

    if(!set_nonblocking(server_fd))
    {
        cerr<<"非阻塞设置失败"<<endl;
        close(server_fd);
        return -1;
    }

    return server_fd;
}