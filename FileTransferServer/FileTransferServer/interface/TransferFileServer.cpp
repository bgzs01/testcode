#include "TransferFileServer.h"


#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include "redisdb.h"

TransferFileServer::TransferFileServer()
{
	// Your initialization goes here
}

bool TransferFileServer::UploadFile(const std::string& file_name, const std::string& file_data, const int64_t file_length)
{
	// Your implementation goes here
	printf("in UploadFile\n");
	return true;
}

bool TransferFileServer::isExistFile(const std::string& file_name) {
	// Your implementation goes here
	printf("in isExistFile\n");

	std::vector<std::string> fileNames;
	RedisOp::getInstance().getFileList(fileNames);

	auto it = find(fileNames.begin(), fileNames.end(), file_name);

	if (it != fileNames.end())
	{
		return true;
	} 
	else{
		return false;
	}
}

void TransferFileServer::DownloadFile(std::string& _return, const std::string& file_name) {
	// Your implementation goes here
	printf("in DownloadFile\n");

	if (!isExistFile(file_name))
	{
		printf("Download Not Exist File\n");
		return;
	}

	std::string file_path = "/home/zhao/download/" + file_name;
	
	size_t fileSize = 0;
	struct stat statbuf;
	if(stat(file_path.c_str(), &statbuf) == 0)
		fileSize = statbuf.st_size;


	int fd = open(file_path.c_str(), O_RDWR);
	if (-1 == fd)
	{
		printf("the open the file is failure");
		//goto error;
		return;
	}

	char* buf = new char[fileSize];
	int ret = read(fd, buf, fileSize);
	if (ret < 0)
	{
		fprintf(stderr, "read file failed\n");
		return;
	}

	_return = std::move(std::string(buf, fileSize));
	
}

void TransferFileServer::getFileList(std::vector<std::string>& _return)
{
	RedisOp::getInstance().getFileList(_return);
	printf("getFileList\n");
}