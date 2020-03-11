﻿// FileTransferClient.cpp: 定义应用程序的入口点。
//

#include "FileTransferClient.h"

#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#include <memory>

#include "TransferFile.h"

#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>

#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000.0 + (tv1.tv_usec - tv2.tv_usec) / 1000.0)

using namespace std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

bool write_to_file(const char* file_name)
{
	std::string file_path = "/home/zhao/code/remote/Linux-Debug/build/FileTransferClient/" + std::string(file_name);

	struct stat statbuf;
	lstat(file_path.c_str(), &statbuf);
	if (S_ISREG(statbuf.st_mode))
	{
		remove(file_path.c_str());
	}

	int fd = open(file_path.c_str(), O_RDWR);
	if (-1 == fd)
	{
		printf("the open the file is failure");
		//goto error;
		return false;
	}

	return true;
}

int main()
{
//	void * pCtx = NULL;
//	void * pSock = NULL;
//	//使用tcp协议进行通信，需要连接的目标机器IP地址为192.168.1.2
//	//通信使用的网络端口 为7766 
//	const char * pAddr = "tcp://192.168.1.180:7766";
//
//	//创建context 
//	if ((pCtx = zmq_ctx_new()) == NULL)
//	{
//		return 0;
//	}
//	//创建socket 
//	if ((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
//	{
//		zmq_ctx_destroy(pCtx);
//		return 0;
//	}
//	int iSndTimeout = 50000;// millsecond
//	//设置接收超时 
//	if (zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iSndTimeout, sizeof(iSndTimeout)) < 0)
//	{
//		zmq_close(pSock);
//		zmq_ctx_destroy(pCtx);
//		return 0;
//	}
//	//连接目标IP192.168.1.2，端口7766 
//	if (zmq_connect(pSock, pAddr) < 0)
//	{
//		zmq_close(pSock);
//		zmq_ctx_destroy(pCtx);
//		return 0;
//	}
//
//	struct timeval tv_begin;
//	struct timeval tv_end;
//
//	gettimeofday(&tv_begin, NULL);
//
//	//循环发送消息 
//	//while (1)
//	//{
//		//static int i = 0;
//	char szMsg[1024] = { 0 };
//	snprintf(szMsg, sizeof(szMsg), "1");
//	printf("Enter to send...\n");
//	if (zmq_send(pSock, szMsg, sizeof(szMsg), 0) < 0)
//	{
//		fprintf(stderr, "send message faild\n");
//		return -1;
//	}
//	printf("send message : [%s] succeed\n", szMsg);
//
//	size_t fileSize;
//
//	if (zmq_recv(pSock, &fileSize, sizeof(fileSize), 0) < 0)
//	{
//		printf("error = %s\n", zmq_strerror(errno));
//		return -1;
//	}
//
//	int fd = open("/home/zhao/code/remote/Linux-Debug/build/FileTransferClient/mysql.rar", O_RDWR);
//	if (-1 == fd)
//	{
//		printf("the open the file is failure");
//		//goto error;
//		return -1;
//	}
//
//
//	char* buf = new char[fileSize];
//	if (zmq_recv(pSock, buf, fileSize, 0) < 0)
//	{
//		printf("error = %s\n", zmq_strerror(errno));
//		return -1;
//	}
//
//	int ret = write(fd, buf, fileSize);
//
//	close(fd);
//
//	delete[] buf;
//	//getchar();
////}
//
//
//	gettimeofday(&tv_end, NULL);
//
//	int time_used = TIME_SUB_MS(tv_end, tv_begin);
//
//	int secs = tv_end.tv_sec - tv_begin.tv_sec;
//	int usec = tv_end.tv_usec - tv_begin.tv_usec;
//	double tu = secs * 1000.0 + usec / 1000.0;
//
//	fprintf(stdout, "send file time used : %d ms\n", tu);

	auto mySock = std::make_shared<TSocket>("192.168.1.180", 9090);
	std::shared_ptr<TTransport> myTransport(new TBufferedTransport(mySock));
	std::shared_ptr<TProtocol> myProtoc(new TBinaryProtocol(myTransport));
	
	TransferFileClient client(myProtoc);

	try {
		myTransport->open();
		struct timeval tv_begin;
		struct timeval tv_end;

		gettimeofday(&tv_begin, NULL);

		std::string file_data;
		std::string file_name = "mysql.tar";
		client.DownloadFile(file_data, file_name);


		gettimeofday(&tv_end, NULL);

		int secs = tv_end.tv_sec - tv_begin.tv_sec;
		int usec = tv_end.tv_usec - tv_begin.tv_usec;
		double tu = secs * 1000.0 + usec / 1000.0;
		//int itu = tu;

		fprintf(stdout, "download file time used : %d ms\n", (int)tu);

		myTransport->close();
	}
	catch (TException& tx) {
		cout << "ERROR: " << tx.what() << endl;
	}


	return 0;
}
