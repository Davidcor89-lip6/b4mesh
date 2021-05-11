#ifndef B4MESH_SESSION
#define B4MESH_SESSION

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

using boost::asio::ip::tcp;

class B4Mesh;

class session
{
public:
    session(B4Mesh* b4mesh, boost::asio::io_context& io_context)
    : b4mesh_(b4mesh),
      io_context_(io_context),
      socket_(io_context){}

    tcp::socket& socket(){ return socket_;}

    void closeSocket(void);

    void start();
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

private:
    void start_reading();

private:
    B4Mesh * b4mesh_;
    std::string destIP_;
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    boost::asio::streambuf response_;
    std::string saveStr;
};

#endif //B4MESH_SESSION
