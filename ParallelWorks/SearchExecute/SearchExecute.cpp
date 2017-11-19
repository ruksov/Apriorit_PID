// SearchExecute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>

#include "SearchFile.h"

int main()
{
    std::wstring dir, mask;

    std::wcout << "directory: ";
    std::getline(std::wcin, dir);

    std::wcout << "mask: ";
    std::getline(std::wcin, mask);

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;

    auto t0 = Time::now();
    auto res = parallelSearch(dir, mask);
    auto t1 = Time::now();

    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::wcout << "\nDuration " << d.count() << "ms\n";
    std::wcout << "Found " << res.size() << " items\n";

    std::wofstream out("SearchResult.txt");

    if (out.is_open())
    {
        for (auto& el : res)
        {
            out << el << '\n';
        }
    }

    out.close();

    system("pause");
    return 0;
}

