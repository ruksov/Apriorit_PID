#pragma once
#include "ThreadPool.h"
#include <experimental/filesystem>
#include <list>

namespace fs = std::experimental::filesystem;

using FileList = std::list<std::wstring>;
using FutureList = std::vector<std::future<FileList>>;

class SearchFile
{
public:
    explicit SearchFile(const std::wstring& mask);
    ~SearchFile();

    FileList DoSearch(const std::wstring& dirStr);

private:
    std::wstring m_mask;

    ThreadPool m_pool;
};

FileList parallelSearch(const std::wstring& directory, const std::wstring& mask);
