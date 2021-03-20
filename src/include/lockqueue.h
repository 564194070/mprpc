#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>


//异步写的日志队列
template<typename T>
class LockQueue
{
public:
    //多个日志写日志queue
    void Push(const T& data)
    {
        //获取了互斥锁
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvarable.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        //防止虚假唤醒
        while(m_queue.empty())
        {
            //日志队列为空 线程进入wait状态
            m_condvarable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvarable;
};