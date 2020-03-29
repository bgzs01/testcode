#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/thread.hpp>

#include <fstream>

#include "../protocol/protocol.h"
//#include "DownloadFileInfo.h"
#include <math.h>
#include <sys/time.h>


using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

class tcp_client
	: public boost::enable_shared_from_this<tcp_client>
{
public:
	tcp_client(boost::asio::io_context& io_context, 
	const tcp::resolver::results_type& endpoints, 
	std::string downloadfilename)
		: io_context_(io_context), socket_(io_context), m_downloadfilename(downloadfilename)
	{
		m_package_count = 0;

		boost::asio::async_connect(socket_, endpoints,
				boost::bind(&tcp_client::handle_connect, this,
					boost::asio::placeholders::error
					));

		//boost::asio::connect(socket_, endpoints);
		//		do_write();

		//do_connect(endpoints);
	}

	//typedef boost::shared_ptr<tcp_client> pointer;

	//static pointer create(boost::asio::io_context& io_context)
	//{
	//	return pointer(new tcp_connection(io_context));
	//}

	tcp::socket& socket()
	{
		return socket_;
	}

private:

	void do_connect(const tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(socket_, endpoints,
				boost::bind(&tcp_client::handle_connect, shared_from_this(),_1
					//boost::asio::placeholders::error
					)
				);
/*
		boost::asio::async_connect(socket_, endpoints,
			[this](boost::system::error_code ec, tcp::endpoint)
		{
			if (!ec)
			{
				do_write();
			}
		});
*/
	}


	void handle_connect(const boost::system::error_code& error)
	{
			if (!error)
			{
				std::cout<<"connect ok"<<std::endl;
				do_write();
			}
	}

	void do_write()
	{
		//发送请求下载文件包
		m_package.header.msg_id = EEVENTID_DOWNLOAD_FILE_REQ;
		m_package.header.tag = protocol_tag;
		
		memcpy(m_package.body, m_downloadfilename.c_str(), m_downloadfilename.size());
		
		boost::asio::async_write(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
				boost::bind(&tcp_client::handle_write, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}


	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (error)
		{
			show_network_error(error);
			socket_.close();
			return;
		}

		if (!check_package_valid())
		{
			socket_.close();
			return;
		}

		if (m_package.header.msg_id == EEVENTID_FILE_DATA_RSP)
		{
			m_ofs.write(reinterpret_cast<char*>(m_package.body), m_package.header.msg_len);

			boost::asio::async_read(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
				boost::bind(&tcp_client::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else if (m_package.header.msg_id == EEVENTID_FILE_END_RSP)
		{
			m_ofs.close();

			int fileSize = m_package.header.file_len;

			gettimeofday(&tv_end, NULL);

			int secs = tv_end.tv_sec - tv_begin.tv_sec;
			int usec = tv_end.tv_usec - tv_begin.tv_usec;
			double tu = secs * 1000.0 + usec / 1000.0;

			double fileSizeKB = fileSize / 1024.0;
			double speedKB = fileSizeKB / (tu / 1000);

			printf("int unixtime, download file time all used : %d ms, average speed: %f KB/s\n\n\n", (int)tu, speedKB);

			std::cout << "receive file ok at: " << make_daytime_string() <<"   file_path:" << m_file_path << std::endl;
		}
	}

	void handle_write(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			std::string curPath = boost::filesystem::initial_path<boost::filesystem::path>().string();
			std::cout << "curPath:  " << curPath << std::endl;

			//std::string file_path = "/home/zhao/code/remote/Linux-Debug/build/FileTransferClient/" + file_name;
			m_file_path = curPath + "/" + m_downloadfilename;

			m_ofs.open(m_file_path, std::ios::binary);
			if ( !m_ofs )
			{
				std::cerr << "Create file failed\n"; 
				return;
			}

			std::cout << "wait, begin to download file... " << std::endl;

			gettimeofday(&tv_begin, NULL);


			boost::asio::async_read(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
				boost::bind(&tcp_client::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			
			

		}

	}


	bool check_package_valid()
	{
		return (protocol_tag == m_package.header.tag);
	}

	inline void show_network_error(const boost::system::error_code &error)
	{
		std::cout << error.message() << std::endl;
	}

	boost::asio::io_context& io_context_;
	tcp::socket socket_;
	protocol_package_t m_package;

	std::string m_downloadfilename;
	std::string m_file_path;
	std::ofstream m_ofs;
	size_t m_file_size;
	int m_package_count;
	int m_all_pack;

		struct timeval tv_begin;
		struct timeval tv_end;
};

//class BoostAsioClient
//{
//public:
//	BoostAsioClient(boost::asio::io_context& io_context)
//		: io_context_(io_context)
//	{
//	}
//
//	void downloadFile(std::string filename) 
//	{
//	}
//
//private:
//	boost::asio::io_context& io_context_;
//};
