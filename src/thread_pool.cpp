#include"thread_pool.hpp"

#include<iostream>
#include<utility>

ThreadPool::ThreadPool(size_t worker_count,size_t max_Queue_size)
{
    max_queue_size=max_Queue_size;
    stopping=false;

    if(worker_count==0)
    {
        worker_count=1;
    }

    for(size_t i=0;i<worker_count;i++)
    {
        workers.emplace_back(&ThreadPool::worker_loop,this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        lock_guard<mutex> lock(Mutex);
        stopping=true;
    }
    condition.notify_all();
    for(auto& worker:workers)
    {
        if(worker.joinable())
        {
            worker.join();
        }
    }
}

bool ThreadPool::enqueue(function<void()> task)
{
    if(!task)
    {
        return false;
    }

    {
        lock_guard<mutex> lock(Mutex);
        if(stopping)
        {
            return false;
        }
        if(tasks.size()>=max_queue_size)
        {
            return false;
        }

        tasks.push(move(task));
    }
    condition.notify_one();
    return true;
}

void ThreadPool::worker_loop()
{
    while(true)
    {
        function<void()> task;
        {
            unique_lock<mutex> lock(Mutex);
            //wait必须要是unique_lock,这里需要手动加锁和解锁的类
            condition.wait(lock,[this]()
        {
            return stopping||!tasks.empty();
        }
        );
            if(stopping&&tasks.empty())
            {
                return;
            }
            task=move(tasks.front());
            tasks.pop();
        }
        //不能持有锁进行task(),有锁的话只能执行完task才能继续下一个任务,会限制其他线程
        try
        {
            task();
        }
        catch(const std::exception& error)
        {
            std::cerr <<
            "thread pool task failed: "
            <<error.what()
             << '\n';
        }
        catch (...)
        {
        // 捕获未知异常，保护工作线程。
        std::cerr
            << "thread pool task failed: "
            "unknown exception\n";
        }   
    }
}


