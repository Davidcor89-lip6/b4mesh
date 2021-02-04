#include "client.hpp"

client::client(node* parent, boost::asio::io_context& io_context, std::string destIP, std::string port, bool block)
    : parent_(parent),
      io_context_(io_context),
      socket_(io_context),
      resolver_(io_context),
      destIP_(destIP),
      m_timer(io_context, std::chrono::steady_clock::now() + std::chrono::seconds(2)),
      forBlock(block)
{

    endpoints = resolver_.resolve(destIP_, port);
    do_connect();

}

void client::do_connect()
{
    boost::asio::async_connect(socket_, endpoints,
    [this](boost::system::error_code ec, tcp::endpoint)
    {
        if (!ec)
        {
            std::cout << " creation with " << socket_.remote_endpoint() << " ok" << std::endl;
            parent_->addClientToList( destIP_, this, forBlock);
            do_read();
        } else {
            std::cout << "no connection : " << ec << std::endl; 
            socket_.close();
            m_timer.expires_from_now(std::chrono::seconds(2));
            m_timer.async_wait(boost::bind(&client::on_ready_to_reconnect, this, boost::asio::placeholders::error));
            //m_timer.async_wait(std::bind(&client::on_ready_to_reconnect, this, std::placeholders::_1));
            /* pt->expires_at(pt->expires_at() + boost::posix_time::seconds(1)); 
            pt->async_wait(boost::bind(callback_func, boost::asio::placeholders::error, pt, pcont)); */
        }
            
    });
}


void client::do_read()
{
boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length),
    [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            std::cout << "read from client " << data_ << std::endl;
            do_read();
        }
        else
        {
            std::cout << " closing connection :" << ec << std::endl;
            parent_->removeClientFromList(destIP_);
        }
    });
}

void client::closeSocket(void){
    io_context_.post([this]() {
        try
        {
            socket_.close();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    });
}
