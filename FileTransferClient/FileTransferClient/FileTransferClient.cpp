// FileTransferClient.cpp: 定义应用程序的入口点。
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
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "TransferFile.h"

#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/timer.hpp>

#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000.0 + (tv1.tv_usec - tv2.tv_usec) / 1000.0)

using namespace std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

bool write_to_file(std::string file_name, std::string& file_data)
{
	std::string curPath = boost::filesystem::initial_path<boost::filesystem::path>().string();
	std::cout << "curPath:  " << curPath << std::endl;

	//std::string file_path = "/home/zhao/code/remote/Linux-Debug/build/FileTransferClient/" + file_name;
	std::string file_path = curPath + "/" + file_name;

	//struct stat statbuf;
	//lstat(file_path.c_str(), &statbuf);
	//if (S_ISREG(statbuf.st_mode))
	//{
	//	remove(file_path.c_str());
	//	printf("%s   file is exist, delete it\n", file_path.c_str());
	//}

	//mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = open(file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, mode);
	//int fd = open(file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC);
	if (-1 == fd)
	{
		printf("the open the file is failure  %s\n", strerror(errno));

		//goto error;
		return false;
	}
	else
	{
		printf("open file success\n");
	}

	int fileSize = file_data.length();

	int ret = write(fd, &file_data[0], fileSize);

	close(fd);
	return true;
}

void zeroMqClient()
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
}

void thriftClient()
{
	auto mySock = std::make_shared<TSocket>("192.168.1.180", 9090);
	std::shared_ptr<TTransport> myTransport(new TBufferedTransport(mySock));
	std::shared_ptr<TProtocol> myProtoc(new TBinaryProtocol(myTransport));

	TransferFileClient client(myProtoc);

	try {
		myTransport->open();
		struct timeval tv_begin;
		struct timeval tv_end;


		while (true)
		{
			std::vector<std::string> vecFileNames;
			client.getFileList(vecFileNames);

			for (size_t i = 0; i < vecFileNames.size(); i++)
			{
				std::cout << i + 1 << ": " << vecFileNames[i] << "    ";

			}
			std::cout << std::endl << "please input the number of file to download (press enter to startup ) or input quit to exit:" << std::endl;
			string input;
			getline(cin, input);

			if (input == "quit")
			{
				break;
			}

			int choice = atoi(input.c_str());
			if (choice < 1 || choice > vecFileNames.size())
			{
				std::cout << "no match file, please input agin" << std::endl;
				continue;
			}

			std::string file_data;
			std::string file_name = vecFileNames[choice - 1];


			cout << "wait, begin to download file... " << endl;

			gettimeofday(&tv_begin, NULL);
			boost::timer boostTimer;


			client.DownloadFile(file_data, file_name);
			size_t fileSize = file_data.length();
			cout << "download file size: " << file_data.length() << endl;
			double t1 = boostTimer.elapsed();
			std::cout << "Download use :" << t1 << " secs" << std::endl;

			bool isWrite = write_to_file(file_name, file_data);
			if (isWrite)
			{
				cout << "write success!" << endl;
			}
			else
			{
				cout << "write failure!" << endl;
			}

			gettimeofday(&tv_end, NULL);
			double tall = boostTimer.elapsed();
			double t2 = tall - t1;
			std::cout << "write to file use: " << t2 << " secs" << std::endl;

			std::cout << "int boosttime, download file time all used: " << tall << " secs" << std::endl;


			int secs = tv_end.tv_sec - tv_begin.tv_sec;
			int usec = tv_end.tv_usec - tv_begin.tv_usec;
			double tu = secs * 1000.0 + usec / 1000.0;
			//int itu = tu;

			double fileSizeKB = fileSize / 1024.0;
			double speedKB = fileSizeKB / (tu / 1000);

			fprintf(stdout, "int unixtime, download file time all used : %d ms, average speed: %f KB/s\n\n\n", (int)tu, speedKB);

		}
		myTransport->close();
	}
	catch (TException& tx) {
		cout << "ERROR: " << tx.what() << endl;
	}
}

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "BoostAsioClient.h"

using boost::asio::ip::tcp;

void handle_connect(const boost::system::error_code& error,
	size_t bytes_transferred)
{
	if (!error)
	{
		//do_write();
	}
}

void boostClient(int argc, char* argv[])
{
	try
	{
		//if (argc != 2)
		//{
		//	std::cerr << "Usage: client <host>" << std::endl;
		//	return 1;
		//}

		boost::asio::io_context io_context;

		tcp::resolver resolver(io_context);
		tcp::resolver::results_type endpoints =
			resolver.resolve("192.168.1.180", "8080");

		std::string downloadfilename = "cmake-3.17.0-rc1.tar.gz";
		//std::string downloadfilename = "boost_1_72_0.tar.gz";

		if (argc > 1)
		{
			int fileid = atoi(argv[1]);

			switch (fileid)
			{
			case 1:
				downloadfilename = "qt.exe";
				break;
			case 2:
				downloadfilename = "mysql.tar";
				break;
			case 3:
				downloadfilename = "cmake-3.17.0-rc1.tar.gz";
				break;
			case 4:
				downloadfilename = "boost_1_53_0.tar.gz";
				break;
			case 5:
				downloadfilename = "boost_1_72_0.tar.gz";
				break;
			}
		}
		
		boost::shared_ptr<tcp_client> client_ptr(new tcp_client(io_context, endpoints, downloadfilename));
		io_context.run();


		//tcp::socket socket_(io_context);
		//boost::asio::async_connect(socket_, endpoints,
		//	boost::bind( handle_connect,
		//		boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred));


		//tcp::socket socket(io_context);
		//boost::asio::connect(socket, endpoints);

		//for (;;)
		//{
		//	boost::array<char, 128> buf;
		//	boost::system::error_code error;

		//	size_t len = socket.read_some(boost::asio::buffer(buf), error);

		//	if (error == boost::asio::error::eof)
		//		break; // Connection closed cleanly by peer.
		//	else if (error)
		//		throw boost::system::system_error(error); // Some other error.

		//	std::cout.write(buf.data(), len);
		//}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

#include "ChartClient.h"

void ChartClient()
{
	try
	{

		boost::asio::io_context io_context;

		tcp::resolver resolver(io_context);
		tcp::resolver::results_type endpoints = resolver.resolve("192.168.1.180", "8081");

		chat_client c(io_context, endpoints);

		boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));

		char line[chat_message::max_body_length + 1];
		while (std::cin.getline(line, chat_message::max_body_length + 1))
		{
			using namespace std; // For strlen and memcpy.
			chat_message msg;
			msg.body_length(strlen(line));
			memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}

		c.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

int main(int argc, char* argv[])
{
	boostClient(argc, argv);
	//ChartClient();

	return 0;
}
