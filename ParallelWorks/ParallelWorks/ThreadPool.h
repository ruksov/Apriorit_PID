#pragma once
#include <atomic>
#include <future>

#include "ThreadSafeStack.h"
#include "ThreadsJoin.h"
#include "FunctionWrapper.h"
#include "WorkStealQueue.h"

class ThreadPool
{
    typedef FunctionWrapper TaskType;
    typedef WorkStealQueue<TaskType> ThreadQueue;

public:
    ThreadPool();
    ~ThreadPool();

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType func)
    {
        typedef typename std::result_of<FunctionType()>::type ResultType;

        std::packaged_task<ResultType(void)> task(std::move(func));
        std::future<ResultType> res(task.get_future());

		if (m_localThreadQueue)
		{
			m_localThreadQueue->push(TaskType(std::move(task)));
		}
		else
		{
			m_mainQueue.push(std::move(task));
		}

        return res;
    }

    void RunPendingTask();

private:
    void WorkerThread(unsigned threadIndex);

    bool GetTaskFromLocal(TaskType& task);
    bool GetTaskFromMain(TaskType& task);
    bool GetTaskFromOther(TaskType& task);

private:
    std::atomic_bool m_done;

    ThreadSafeStack<TaskType> m_mainQueue;
    std::vector<std::unique_ptr<ThreadQueue>> m_threadQueues;

    std::vector<std::future<void>> m_threads;

    static thread_local ThreadQueue* m_localThreadQueue;
    static thread_local unsigned m_threadIndex;
};

