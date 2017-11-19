#include "stdafx.h"
#include "ThreadPool.h"

thread_local WorkStealQueue<FunctionWrapper>* ThreadPool::m_localThreadQueue;
thread_local unsigned ThreadPool::m_threadIndex;

ThreadPool::ThreadPool()
    : m_done(false)
{
    unsigned const threadCount = std::thread::hardware_concurrency();

    try
    {
        for (unsigned i = 0; i < threadCount; ++i)
        {
            m_threadQueues.push_back(std::unique_ptr<ThreadQueue>(new ThreadQueue));

            m_threads.push_back(std::async(&ThreadPool::WorkerThread, this, i));
        }
    }
    catch (std::system_error& ex)
    {
        m_done = true;
        std::cout << "Thread pool ctor exception:\n"
            << ex.what();
    }
}


ThreadPool::~ThreadPool()
{
    m_done = true;

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

void ThreadPool::RunPendingTask()
{
    TaskType task;
    if (GetTaskFromLocal(task)
        || GetTaskFromMain(task)
        || GetTaskFromOther(task))
    {
            task(); 
    }
    else
    {
        std::this_thread::yield();
    }
}

void ThreadPool::WorkerThread(unsigned threadIndex)
{
    m_threadIndex = threadIndex;
    m_localThreadQueue = m_threadQueues[threadIndex].get();

    while (!m_done)
    {
        RunPendingTask();
    }
}

bool ThreadPool::GetTaskFromLocal(TaskType & task)
{
    return m_localThreadQueue && m_localThreadQueue->try_pop(task);
}

bool ThreadPool::GetTaskFromMain(TaskType & task)
{
    return m_mainQueue.try_pop(task);
}

bool ThreadPool::GetTaskFromOther(TaskType & task)
{
    for (size_t i = 0, size = m_threadQueues.size(); i < size; ++i)
    {
        auto index = (m_threadIndex + i + 1) % size;
        if (m_threadQueues[index]->try_steal(task))
        {
            return true;
        }
    }

    return false;
}
