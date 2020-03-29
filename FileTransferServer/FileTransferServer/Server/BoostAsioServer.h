#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/thread.hpp>

#include <fstream>

#include "../protocol/protocol.h"
#include "DownloadFileInfo.h"
#include <math.h>


using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context& io_context)
	{
		return pointer(new tcp_connection(io_context));
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		//message_ = make_daytime_string();

		boost::asio::async_read(socket_,
			boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
			boost::bind(&tcp_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
		);

		//boost::asio::async_write(socket_, boost::asio::buffer(message_),
		//	boost::bind(&tcp_connection::handle_write, shared_from_this(),
		//		boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_context& io_context)
		: socket_(io_context)
	{
		m_package_count = 0;
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

		if (m_package.header.msg_id == EEVENTID_DOWNLOAD_FILE_REQ)
		{
			std::string downloadFileName((char*)m_package.body);
			std::string filePath = "/home/zhao/download/" + downloadFileName;
			m_package_count = 0;

			m_ifs.open(filePath, std::ios::binary);
			if (!m_ifs)
			{
				std::cerr << "open file error" << std::endl;
				socket_.close();
				return;
			}
			m_ifs.seekg(0, std::ios::end);
			m_file_size = m_ifs.tellg();

			//清除end状态
			m_ifs.clear();
			//定位到起始位置
			m_ifs.seekg(0, std::ios::beg);

			double d_all_pack = (double)m_file_size / sizeof(m_package.body);
			m_all_pack = ceil(d_all_pack);

			m_package.header.tag = protocol_tag;
			m_package.header.file_id = DownloadFileInfo::getInstance().getFileIdByName(downloadFileName);
			m_package.header.file_len = m_file_size;
			m_package.header.msg_id = EEVENTID_FILE_DATA_RSP;
			m_package.header.all_pack = m_all_pack;
			//m_package.header.md5;
			m_package.header.seq = ++m_package_count;

			m_ifs.read(reinterpret_cast<char*>(m_package.body), sizeof(m_package.body));
			m_package.header.msg_len = m_ifs.gcount();

			boost::asio::async_write(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
				boost::bind(&tcp_connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_write(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			m_package.header.seq = ++m_package_count;
			if (!m_ifs.eof())
			{
				m_package.header.seq = ++m_package_count;

				m_ifs.read(reinterpret_cast<char*>(m_package.body), sizeof(m_package.body));
				m_package.header.msg_len = m_ifs.gcount();

				boost::asio::async_write(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
					boost::bind(&tcp_connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				if (m_package.header.msg_id != EEVENTID_FILE_END_RSP)
				{
					m_package.header.msg_id = EEVENTID_FILE_END_RSP;
					//m_package.header.seq = ++m_package_count;

					memset(m_package.body, 0, sizeof(m_package.body));

					m_package.header.msg_len = 0;

					boost::asio::async_write(socket_, boost::asio::buffer(static_cast<void*>(&m_package), sizeof(m_package)),
						boost::bind(&tcp_connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
				}
			}

		}

		//message_ = make_daytime_string();

		////boost::thread::sleep(boost)
		//boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(5));

		//boost::asio::async_write(socket_, boost::asio::buffer(message_),
		//	boost::bind(&tcp_connection::handle_write, shared_from_this(),
		//		boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred));
	}


	bool check_package_valid()
	{
		return (protocol_tag == m_package.header.tag);
	}

	inline void show_network_error(const boost::system::error_code &error)
	{
		std::cout << error.message() << std::endl;
	}

	tcp::socket socket_;
	std::string message_;
	protocol_package_t m_package;

	std::ifstream m_ifs;
	size_t m_file_size;
	int m_package_count;
	int m_all_pack;
};

class tcp_server
{
public:
	tcp_server(boost::asio::io_context& io_context)
		: io_context_(io_context),
		acceptor_(io_context, tcp::endpoint(tcp::v4(), 8080))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(io_context_);

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_connection->start();
			std::cout << "accept new connection" << std::endl;
		}

		start_accept();
	}

	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
};


//class BoostAsioServer
//{
//public:
//	BoostAsioServer(boost::asio::io_context& io_context, int port);
//	
//	~BoostAsioServer();
//
//	void start_accept();
//	
//	void handle_accept(tcp_connection::pointer new_connection,
//		const boost::system::error_code& error)
//	{
//		if (!error)
//		{
//			new_connection->start();
//		}
//
//		start_accept();
//	}
//
//	boost::asio::io_context& io_context_;
//	boost::asio::ip::tcp::acceptor acceptor_;
//};