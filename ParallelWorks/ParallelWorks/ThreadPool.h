#pragma once
#include <atomic>
#include <future>

#include "ThreadSafeStack.h"
#include "FunctionWrapper.h"

class ThreadPool
{
    typedef FunctionWrapper TaskType;

public:
    ThreadPool();
    ~ThreadPool();

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType func)
    {
        typedef typename std::result_of<FunctionType()>::type ResultType;

        std::packaged_task<ResultType(void)> task(std::move(func));
        std::future<ResultType> res(task.get_future());

		m_mainQueue.push(std::move(task));

        m_cond.notify_one();

        return res;
    }

    void RunPendingTask();

private:
    void WorkerThread();

private:
    std::atomic_bool m_done;
    std::mutex m_lock;
    std::condition_variable m_cond;

    ThreadSafeStack<TaskType> m_mainQueue;
    std::vector<std::future<void>> m_threads;
};

