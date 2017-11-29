#include "stdafx.h"
#include "ThreadPool.h"

TEST(TestThreadPool, ParallelAccumulate)
{
	std::vector<int> vec;

	for (auto i = 0; i < 1000; ++i)
	{
		vec.push_back(1);
	}

	auto res = std::accumulate(vec.begin(), vec.end(), 0);

	ThreadPool pool;

	auto itStart = vec.begin();
	auto itEnd = itStart;
    auto step = 50;
	std::advance(itEnd, step);

	std::vector<std::future<int>> futures;

	while (itEnd != vec.end())
	{
		futures.push_back(pool.Submit(
			[&]()
		{
			return std::accumulate(itStart, itEnd, 0);
		}));

		itStart = itEnd;
		std::advance(itEnd, step);
	}

	futures.push_back(pool.Submit(
		[&]()
	{
		return std::accumulate(itStart, vec.end(), 0);
	}));

	int parallelRes = 0;

	for (auto& f : futures)
	{
		auto buf = f.get();

		ASSERT_EQ(step, buf);
		parallelRes += buf;
	}

	ASSERT_EQ(res, parallelRes);
}