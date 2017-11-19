#pragma once
#include <stack>
#include <mutex>

template<typename T>
class ThreadSafeStack
{
public:
    ThreadSafeStack()
    {
    }

    void push(T value)
    {
        auto data(std::make_shared<T>(std::move(value)));

        std::lock_guard<std::mutex> lock(m_lock);
        m_data.push(data);
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if (m_data.empty())
        {
            return false;
        }

        value = std::move(*m_data.top());
        m_data.pop();

        return true;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_lock);
        return m_data.empty();
    }

private:
    mutable std::mutex m_lock;
    std::stack<std::shared_ptr<T>> m_data;
};