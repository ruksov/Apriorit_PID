#pragma once
#include <deque>
#include <mutex>

template <typename T>
class WorkStealQueue
{
public:
    WorkStealQueue() = default;

    void push(T& value)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_data.push_front(std::move(value));
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_lock);
        return m_data.empty();
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_data.empty())
        {
            return false;
        }

        value = std::move(m_data.front());
        m_data.pop_front();

        return true;
    }

    bool try_steal(T& value)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_data.empty())
        {
            return false;
        }

        value = std::move(m_data.back());
        m_data.pop_back();

        return true;
    }

private:
    mutable std::mutex m_lock;
    std::deque<T> m_data;
};