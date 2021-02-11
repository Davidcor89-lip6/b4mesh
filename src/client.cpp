#include "client.hpp"

client::client(node* parent, boost::asio::io_context& io_context, std::string destIP, std::string port, bool block)
    : parent_(parent),
      io_context_(io_context),
      socket_(io_context),
      resolver_(io_context),
      strand_(io_context),
      outbox_(),
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
            std::cout << "read from client :" << data_ << std::endl;
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

void client::write_message(const std::string& message)
{
    strand_.post(
        boost::bind(
            &client::writeImpl,
            this,
            message
            )
        );
}


void client::writeImpl(const std::string& message)
{
    outbox_.push_back( message );
    if ( outbox_.size() > 1 ) {
        // outstanding async_write
        return;
    }

    this->write();
}

void client::write()
{
    const std::string& message = outbox_[0] + std::string("end");
    boost::asio::async_write(
            socket_,
            boost::asio::buffer( message.c_str(), message.size() ),
            strand_.wrap(
                boost::bind(
                    &client::writeHandler,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void client::writeHandler(const boost::system::error_code& error, const size_t bytesTransferred)
{
    outbox_.pop_front();

    if ( error ) {
        std::cerr << "could not write: " << boost::system::system_error(error).what() << std::endl;
        return;
    }

    if ( !outbox_.empty() ) {
        // more messages to send
        this->write();
    }
}