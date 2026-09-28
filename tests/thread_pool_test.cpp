#include "thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

atomic<int> cmp=0;

void task_of_add()
{
    //模拟任务执行
    this_thread::sleep_for(chrono::milliseconds(10));
    cmp++;
}

int main()
{
    ThreadPool tp(2,8);

    constexpr int task_number=20;
    for(int i=0;i<task_number;i++)
    {
        bool accepted=tp.enqueue(task_of_add);
        if(!accepted)
        {
            cmp++;
        }
    }

    while(cmp.load()<task_number)
    {
        this_thread::sleep_for(chrono::milliseconds(10));
        //不指定时间的睡眠
        //this_thread::yield();
    }

    if(cmp.load()!=task_number)
    {
        cout<<"work is failture"<<endl;
        return 1;
    }
    
    cout<<"all work is pass"<<endl;

    return 0;
}