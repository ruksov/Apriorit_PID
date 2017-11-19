#pragma once

struct ImplBase
{
    virtual void Call() = 0;
    virtual ~ImplBase() = default;
};

template<typename F>
struct ImplType : ImplBase
{
    F m_f;
    
    ImplType(F&& f) : m_f(std::move(f))
    {
    }

    void Call() override
    {
        m_f();
    }
};