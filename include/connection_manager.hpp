#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "client_session.hpp"

#include<string>
#include<unordered_map>
#include<vector>

using namespace std;

class connection_manager
{
    public:
    //添加一个客户端
    bool add(int fd);
    //删除一个客户端
    bool remove(size_t fd);
    //查找对应客户端
    ClientSession* find(size_t fd);
    //判断fd是否存在
    bool contains(size_t fd);
    //返回当前连接数量
    size_t size();

    vector<int> file_descriptors();

    void clear();
    //客户端哈希表
    unordered_map<int,ClientSession> client_session;

    private:
    //客户端数量计时器
    size_t connection_number=1;
    
};

#endif