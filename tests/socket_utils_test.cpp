#include"socket_utils.hpp"

#include<iostream>
using namespace std;

#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    int fds[2];

    socketpair(AF_UNIX,SOCK_STREAM,0,fds);

    if(set_nonblocking(fds[0]))
    {
        cout<<"YES"<<endl;
    }
    else
    {
        cout<<"NO"<<endl;
    }

    int flag=fcntl(fds[0],F_GETFL,0);
    if(fcntl(fds[0],F_SETFL,flag | O_NONBLOCK)!=-1)
    {
        cout<<"YES"<<endl;
    }
    else
    {
        cout<<"NO"<<endl;
    }

    if(!set_nonblocking(-1))
    {
        cout<<"YES"<<endl;
    }
    else
    {
        cout<<"NO"<<endl;
    }

    close(fds[0]);

    close(fds[1]);
    return 0;
}