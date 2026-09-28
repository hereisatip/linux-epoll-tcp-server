#ifndef SERVER_HPP
#define SERVER_HPP

#include"connection_manager.hpp"
#include"protocol.hpp"
#include"thread_pool.hpp"
#include<stdint.h>
#include<iostream>
#include<mutex>

using namespace std;

class BusinessResult
{
    public:
    int client_fd=-1;

    int64 connection_id=0;

    string response;
};

class Server
{
    public:
    Server(short _port,size_t worker_count=4);
    ~Server();

    bool initialize();
    void run(size_t timeout_ms);

    private:
    void accept_clients();

    void close_client(int client_fd);

    void handle_client_event(int client_fd,size_t event);

    void handle_read(int client_fd);

    void handle_write(int client_fd);

    void submit_calc_result(int client_fd,int64 connection_id,int64 number,size_t delay_ms);

    void notify_result_ready();

    void handle_result_event();

    void process_complete_results();

    void cleanup();

    void handle_timeout();

    int listen_fd;
    int epoll_fd;
    short port;

    int result_event_fd = -1;

    int running=true;

    mutex result_mutex;

    ThreadPool thread_pool;

    connection_manager connections;

    queue<BusinessResult> complete_results;
};

#endif