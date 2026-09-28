#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

//在程序启动前先创建几个线程
class ThreadPool
{
    public:

    //explicit 禁止隐式转换
    //创建工作线程
    explicit ThreadPool(size_t worker_count,size_t max_Queue_size);

    //停止并等待工作线程
    ~ThreadPool();

    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool& )=delete;

    //提交任务
    bool enqueue(function<void()> task);
    

    private:
    //每个工作线程反复执行的函数
    void worker_loop();

    //保存所有工作线程
    vector<thread> workers;

    //等待执行的任务
    queue<function<void()>> tasks;
    //保护tasks和stopping
    mutex Mutex;
    //没任务时让工作线程睡眠
    condition_variable condition;
    
    //控制线程池是否停止接受任务
    bool stopping;

    //限制等待队列的最大容量
    size_t max_queue_size;


};

#endif