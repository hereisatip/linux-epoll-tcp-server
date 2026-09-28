#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include<string>
#include<cstring>
#include<chrono>
#include<cstdint>

using namespace std;

using Clock = std::chrono::steady_clock;

class ClientSession
{
    public:
    //客户端SOCKET文件描述符
    size_t fd;
    //当前连接的唯一身份
    size_t connection_id;
    //保存尚未解析完整的输入数据
    string input_buffer;
    //保存尚未发送完的响应数据
    string output_buffer;

    //最近一次收到客户端数据的时间
    Clock::time_point last_activity=Clock::now();
};




#endif