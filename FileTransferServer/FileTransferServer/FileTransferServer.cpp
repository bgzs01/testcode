// FileTransferServer.cpp: 定义应用程序的入口点。
//

#include "FileTransferServer.h"
#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000.0 + (tv1.tv_usec - tv2.tv_usec) / 1000.0)

using namespace std;

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

#include "FileTransferServer.h"
#include "TransferFileServer.h"

//void readFile(int type, void* pSock)
//{
//	char fileName[128] = { 0 };
//
//	struct timeval tv_begin;
//	struct timeval tv_end;
//
//	gettimeofday(&tv_begin, NULL);
//
//	if (type == 1)
//	{
//		//mysql.rar
//		strcpy(fileName, "/home/zhao/download/mysql.tar");
//
//	}
//	else if (type == 2)
//	{
//		//qt.exe
//		strcpy(fileName, "/home/zhao/download/qt.tar");
//	}
//	else {
//		return;
//	}
//
//	size_t fileSize = 0;
//	int fd = open(fileName, O_RDWR);
//	if (-1 == fd)
//	{
//		printf("the open the file is failure");
//		//goto error;
//		return;
//	}
//
//	struct stat statbuf;
//	if(stat(fileName, &statbuf) == 0)
//		fileSize = statbuf.st_size;
//
//	if (zmq_send(pSock, &fileSize, sizeof(fileSize), 0) < 0)
//	{
//		fprintf(stderr, "send file size failed\n");
//		return;
//	}
//
//	char* buf = new char[fileSize];
//	int ret = read(fd, buf, fileSize);
//	if (ret < 0)
//	{
//		fprintf(stderr, "read file failed\n");
//		return;
//	}
//
//	if (zmq_send(pSock, buf, fileSize, 0) < 0)
//	{
//		fprintf(stderr, "send file failed\n");
//		return;
//	}
//
//	delete[] buf;
//
//	close(fd);
//
//	gettimeofday(&tv_end, NULL);
//
//	int time_used = TIME_SUB_MS(tv_end, tv_begin);
//
//
//	fprintf(stdout, "send file time used : %d ms\n", time_used);
//
////error:
////	fileSize = -1;
//}

int main()
{
	 //void * pCtx = NULL;
  //   void * pSock = NULL;
  //   const char * pAddr = "tcp://*:7766";
 
  //   //创建context，zmq的socket 需要在context上进行创建 
  //   if((pCtx = zmq_ctx_new()) == NULL)
  //   {
  //       return 0;
  //   }
  //   //创建zmq socket ，socket目前有6中属性 ，这里使用dealer方式
  //   //具体使用方式请参考zmq官方文档（zmq手册） 
  //   if((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
  //   {
  //       zmq_ctx_destroy(pCtx);
  //       return 0;
  //   }
  //   int iRcvTimeout = 5000;// millsecond
  //   //设置zmq的接收超时时间为5秒 
  //   if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout)) < 0)
  //   {
  //       zmq_close(pSock);
  //       zmq_ctx_destroy(pCtx);
  //       return 0;
  //   }
  //   //绑定地址 tcp://*:7766 
  //   //也就是使用tcp协议进行通信，使用网络端口 7766
  //   if(zmq_bind(pSock, pAddr) < 0)
  //   {
  //       zmq_close(pSock);
  //       zmq_ctx_destroy(pCtx);
  //       return 0;
  //   }
  //   printf("bind at : %s\n", pAddr);
  //   while(1)
  //   {
  //       char szMsg[1024] = {0};
  //       printf("waitting...\n");
  //       errno = 0;
  //       //循环等待接收到来的消息，当超过5秒没有接到消息时，
  //       //zmq_recv函数返回错误信息 ，并使用zmq_strerror函数进行错误定位 
  //       if(zmq_recv(pSock, szMsg, sizeof(szMsg), 0) < 0)
  //       {
  //           printf("error = %s\n", zmq_strerror(errno));
		//	 continue;
  //       }

  //       printf("received message : %s\n", szMsg);

		// int type = atoi(szMsg);
		// 

		// readFile(type, pSock);
  //   }

	int port = 9090;
	::std::shared_ptr<TransferFileServer> handler(new TransferFileServer());
	::std::shared_ptr<TProcessor> processor(new TransferFileProcessor(handler));
	::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();
	return 0;
}
