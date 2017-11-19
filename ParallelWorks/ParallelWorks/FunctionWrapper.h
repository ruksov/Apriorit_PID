#pragma once
#include <memory>
#include "ImplStructures.h"

class FunctionWrapper
{
public:
    template<typename F>
    FunctionWrapper(F&& f)
        : m_impl(new ImplType<F>(std::move(f)))
    {
    }

    FunctionWrapper() = default;

    FunctionWrapper(FunctionWrapper&& other)
        : m_impl(std::move(other.m_impl))
    {
    }

    FunctionWrapper& operator=(FunctionWrapper&& other)
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper& operator=(FunctionWrapper&) = delete;

    void operator() ()
    {
        m_impl->Call();
    }

private:
    std::unique_ptr<ImplBase> m_impl;
};