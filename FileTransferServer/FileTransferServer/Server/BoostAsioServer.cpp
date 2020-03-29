#include "BoostAsioServer.h"

using namespace boost::asio;
using namespace boost::asio::ip;




//BoostAsioServer::BoostAsioServer(boost::asio::io_context& io_context, int port)
//	:io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
//{
//	start_accept();
//}
//
//BoostAsioServer::~BoostAsioServer()
//{
//}
//
//void BoostAsioServer::start_accept()
//{
//	 tcp_connection::pointer new_connection =
//		tcp_connection::create(io_context_);
//
//	acceptor_.async_accept(new_connection->socket(),
//		boost::bind(&BoostAsioServer::handle_accept, this, new_connection,
//			boost::asio::placeholders::error));
//}


