#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <vector>
#include <map>


class RedisOp
{
	RedisOp();
public:
	~RedisOp();

	static RedisOp& getInstance();

	bool isConnected();
	bool execCmd();

	void setFileList(std::string filePath);
	void getFileList(std::vector<std::string>& vecFileNames);
	void getFileList(std::map<int, std::string>& mapFileId2Name);

	void getFiles(const std::string& rootPath, std::vector<std::string> &ret, std::vector<std::string> &name);

private:
	redisContext* m_redCxt;
	bool m_isConnected;
};