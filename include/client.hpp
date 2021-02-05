#ifndef B4MESH_CLIENT
#define B4MESH_CLIENT

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

using boost::asio::ip::tcp;

#include "node.hpp"

class client
{
public:
	client(node* parent, boost::asio::io_service& io_service, std::string destIP, std::string port, bool block);
	
	tcp::socket& socket(){return socket_;}

	void closeSocket(void);

private:

	void do_connect();
	void on_ready_to_reconnect(const boost::system::error_code &error) { do_connect();}
	void do_read();
	
	//parent
	node * parent_;

	boost::asio::io_context& io_context_;
    tcp::socket socket_;
	tcp::resolver resolver_;
	std::string destIP_;
    tcp::resolver::results_type endpoints;
	boost::asio::steady_timer m_timer;

	// connexion dedecated to block
	bool forBlock;
 
	enum { max_length = 4096 };
    char data_[max_length];
};



#endif //B4MESH_CLIENT