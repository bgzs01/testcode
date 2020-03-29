#pragma once

#include <map>

class DownloadFileInfo
{
public:
	static DownloadFileInfo& getInstance()
	{
		static DownloadFileInfo dfi;
		return dfi;
	}

	void setFileInfo(std::map<int, std::string>& mapFileId2Name)
	{
		m_mapFileId2Name = mapFileId2Name;
	}

	int getFileIdByName(std::string fileName)
	{
		for (auto iter : m_mapFileId2Name)
		{
			if (iter.second == fileName)
				return iter.first;
		}

		return 0;
	}

	~DownloadFileInfo()
	{}
private:
	DownloadFileInfo()
	{}

	std::map<int, std::string> m_mapFileId2Name;
};