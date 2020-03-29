#include "redisdb.h"
#include <string>
#include <string.h>
#include <iostream>
 
#include <boost/filesystem.hpp>

RedisOp::RedisOp()
{
	//redis默认监听端口为6387 可以再配置文件中修改 
	m_redCxt = redisConnect("192.168.1.180", 6379);
	if (m_redCxt->err)
	{
		m_isConnected = false;
		redisFree(m_redCxt);
		printf("Connect to redisServer faile\n");
		return;
	}
	m_isConnected = true;
	printf("Connect to redisServer Success\n");
}

RedisOp::~RedisOp()
{
	if(m_isConnected)
		redisFree(m_redCxt);
}

RedisOp & RedisOp::getInstance()
{
	static RedisOp ro;
	return ro;
	// TODO: 在此处插入 return 语句
}

bool RedisOp::isConnected()
{
	return m_isConnected;
}

bool RedisOp::execCmd()
{
	return false;
}

void RedisOp::setFileList(std::string filePath)
{
	if (!m_isConnected)
		return;
	//清空存储列表
	redisReply* r = (redisReply*)redisCommand(m_redCxt, "del fileList");
	if (NULL == r)
	{
		printf("Execute command failure\n");
		//redisFree(m_redCxt);
		return;
	}
	freeReplyObject(r);

	r = (redisReply*)redisCommand(m_redCxt, "del fileIdList");
	if (NULL == r)
	{
		printf("Execute command failure\n");
		//redisFree(m_redCxt);
		return;
	}
	freeReplyObject(r);


	//写入文件名称列表
	std::string command = "rpush fileList";
	std::string commandid = "rpush fileIdList";

	std::vector<std::string> vecFilePaths;
	std::vector<std::string> vecFileNames;
	getFiles(filePath, vecFilePaths, vecFileNames);

	for (int i=0;i<vecFileNames.size(); ++i)
	{
		command.append(" ");
		command.append(vecFileNames[i]);
		commandid.append(" ");
		commandid.append(std::to_string(i + 1));
	}

	r = (redisReply*)redisCommand(m_redCxt, command.c_str());

	if (NULL == r)
	{
		printf("Execute command failure\n");
		//redisFree(m_redCxt);
		return;
	}

	freeReplyObject(r);
	
	r = (redisReply*)redisCommand(m_redCxt, commandid.c_str());

	if (NULL == r)
	{
		printf("Execute command failure\n");
		//redisFree(m_redCxt);
		return;
	}
	freeReplyObject(r);

	printf("Succeed to execute command[%s ; %s]\n", command.c_str(), commandid.c_str());
}

void RedisOp::getFileList(std::vector<std::string>& vecFileNames)
{
	if (!m_isConnected)
		return;

	const char* command = "lrange fileList 0 -1";
	redisReply* r = (redisReply*)redisCommand(m_redCxt, command);

	if (NULL == r)
	{
		printf("Execute command1 failure\n");
		//redisFree(m_redCxt);
		return;
	}

	if (r->type != REDIS_REPLY_ARRAY)
	{
		printf("Failed to execute command[%s]\n", command);
		freeReplyObject(r);
		//redisFree(m_redCxt);
		return;
	}

	for (size_t i = 0; i < r->elements; i++)
	{
		std::string strFileName(r->element[i]->str);
		vecFileNames.push_back(strFileName);
	}

	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command);
}

void RedisOp::getFileList(std::map<int, std::string>& mapFileId2Name)
{
	if (!m_isConnected)
		return;

	std::vector<std::string> vecFileNames;
	std::vector<int> vecFileIds;

	const char* command = "lrange fileList 0 -1";
	redisReply* r = (redisReply*)redisCommand(m_redCxt, command);

	if (NULL == r)
	{
		printf("Execute command1 failure\n");
		//redisFree(m_redCxt);
		return;
	}

	if (r->type != REDIS_REPLY_ARRAY)
	{
		printf("Failed to execute command[%s]\n", command);
		freeReplyObject(r);
		//redisFree(m_redCxt);
		return;
	}

	for (size_t i = 0; i < r->elements; i++)
	{
		std::string strFileName(r->element[i]->str);
		vecFileNames.push_back(strFileName);
	}

	freeReplyObject(r);


	const char* commandid = "lrange fileIdList 0 -1";
	r = (redisReply*)redisCommand(m_redCxt, commandid);

	if (NULL == r)
	{
		printf("Execute command1 failure\n");
		//redisFree(m_redCxt);
		return;
	}

	if (r->type != REDIS_REPLY_ARRAY)
	{
		printf("Failed to execute command[%s]\n", commandid);
		freeReplyObject(r);
		//redisFree(m_redCxt);
		return;
	}

	for (size_t i = 0; i < r->elements; i++)
	{
		std::string strFileId(r->element[i]->str);
		vecFileIds.push_back(std::stoi(strFileId));
	}

	freeReplyObject(r);

	if (vecFileNames.size() == vecFileIds.size())
	{
		for (size_t i = 0; i < vecFileNames.size(); i++)
		{
			mapFileId2Name[vecFileIds[i]] = vecFileNames[i];
		}
	}

	printf("Succeed to execute command[%s ; %s]\n", command, commandid);
}

void RedisOp::getFiles(const std::string& rootPath, std::vector<std::string> &ret, std::vector<std::string> &name)
{
	namespace fs = boost::filesystem;
	fs::path fullpath(rootPath);
	fs::recursive_directory_iterator end_iter;
	for (fs::recursive_directory_iterator iter(fullpath); iter != end_iter; iter++)
	{
		try
		{
			if (fs::is_directory(*iter))
			{
				std::cout << *iter << "is dir" << std::endl;
				ret.push_back(iter->path().string());
			}
			else
			{
				std::string file = iter->path().string();
				ret.push_back(iter->path().string());
				fs::path filePath(file);
				name.push_back(filePath.filename().string());
			}
		}
		catch (const std::exception & ex)
		{
			std::cerr << ex.what() << std::endl;
			continue;
		}
	}
}