#include "stdafx.h"
#include "SearchFile.h"

SearchFile::SearchFile(const std::wstring & mask)
    : m_mask(mask)
{
}

SearchFile::~SearchFile()
{
}

FileList SearchFile::DoSearch(const std::wstring& dirStr)
{
	if (fs::is_empty(dirStr))
	{
		return FileList();
	}

	FutureList pendingFolders;
	FileList result;

	for (auto& el : fs::directory_iterator(dirStr))
	{
		auto elPath = el.path();

		if (fs::is_directory(elPath))
		{
			pendingFolders.push_back(
				std::move(m_pool.Submit(std::bind(&SearchFile::DoSearch, this, elPath))));
		}

		if (PathMatchSpec(elPath.filename().c_str(), m_mask.c_str()))
		{
			result.push_back(elPath.wstring());
		}
	}

	for (auto& folder : pendingFolders)
	{
		while (folder.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		{
			m_pool.RunPendingTask();
		}

		result.splice(result.begin(), folder.get());
	}

	return result;
}

std::list<std::wstring> parallelSearch(const std::wstring & directory, const std::wstring & mask)
{
	std::cout << "Main thread " << std::this_thread::get_id() << "\n\n";

    if (!fs::exists(directory)
        || !fs::is_directory(directory)
        || fs::is_empty(directory))
    {
        return FileList();
    }

    SearchFile search(mask);

    return search.DoSearch(directory);
}
