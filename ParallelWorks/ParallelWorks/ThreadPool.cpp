#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool()
    : m_done(false)
{
    unsigned const threadCount = std::thread::hardware_concurrency();

    try
    {
        for (unsigned i = 0; i < threadCount; ++i)
        {
            m_threads.push_back(std::async(&ThreadPool::WorkerThread, this));
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
    m_cond.notify_all();

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

    if (m_mainQueue.try_pop(task))
    {
        task();
    }
}

void ThreadPool::WorkerThread()
{
    while (!m_done)
    {
        TaskType task;
        std::unique_lock<std::mutex> lock(m_lock);

        m_cond.wait(lock, 
            [this, &task]() -> bool
        {
            return m_mainQueue.try_pop(task) || m_done;
        });

        if (!m_done)
        {
            task();
        }
    }
}