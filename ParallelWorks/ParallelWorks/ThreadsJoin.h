#pragma once
#include <thread>
#include <vector>

class ThreadsJoin
{
public:
    explicit ThreadsJoin(std::vector<std::thread>& threads)
        : m_threads(threads)
    {
    }

    ~ThreadsJoin()
    {
        for (auto& th : m_threads)
		{
            if (th.joinable())
            {
                th.join();
            }
        }
    }

private:
    std::vector<std::thread>& m_threads;
};