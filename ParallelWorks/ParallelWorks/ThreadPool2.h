#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stack>

#include "FunctionWrapper.h"

class ThreadPool
{
    typedef FunctionWrapper TaskType;

public:
    ThreadPool()
        : m_done(false)
    {
        unsigned const threadCount = std::thread::hardware_concurrency();

        try
        {
            for (unsigned i = 0; i < threadCount; ++i)
            {
                m_threads.push_back(std::async(std::launch::async, &ThreadPool::WorkerThread, this));
            }
        }
        catch (std::system_error& ex)
        {
            m_done = true;
            std::cout << "Thread pool ctor exception:\n"
                << ex.what();
        }
    }
    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_done = true;
            m_cond.notify_all();
        }
       
        try
        {
            for (auto& th : m_threads)
            {
                if (th.valid())
                {
                    th.get();
                }
            }
        }
        catch (std::system_error& ex)
        {
            std::cout << "Thread pool dtor exception:\n"
                << ex.what();
        }
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType func)
    {
        typedef typename std::result_of<FunctionType()>::type ResultType;

        std::packaged_task<ResultType(void)> task(std::move(func));
        std::future<ResultType> res(task.get_future());

        std::unique_lock<std::mutex> lock(m_lock);
        m_workStack.push(std::make_shared<TaskType>(std::move(task));
        m_cond.notify_one();

        return res;
    }

    void RunPendingTask()
    {
        std::unique_lock<std::mutex> lock(m_lock);
        if (!m_workStack.empty())
        {
            TaskType task = std::move(*m_workStack.top());
            m_workStack.pop();
            lock.unlock();

            task();
        }
    }

private:
    void WorkerThread()
    {
        TaskType task;

        std::unique_lock<std::mutex> lock(m_lock);
        while (!m_done)
        {
            m_cond.wait(lock, [this]() { return !m_workStack.empty() || m_done; });

            if(!m_done)
            {
                task = std::move(*m_workStack.top());
                m_workStack.pop();
                lock.unlock();

                task();
                lock.lock();
            }          
        }
    }

private:
    bool m_done;
    std::mutex m_lock;
    std::condition_variable m_cond;

    std::stack<std::shared_ptr<TaskType>> m_workStack;
    std::vector<std::future<void>> m_threads;
};