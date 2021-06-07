
#include "session.hpp"
#include "b4mesh_p.hpp"

void session::start()
{
    start_reading();
}

void session::start_reading()
{
    destIP_ = socket_.remote_endpoint().address().to_string();
    boost::asio::async_read_until(socket_,
        response_,
        "end",
        boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
    std::size_t bytes_transferred)
{
    if (!error)
    {    
        std::string s((std::istreambuf_iterator<char>(&response_)), std::istreambuf_iterator<char>());
        
        std::string sEnd ("end");
        std::size_t found = s.find(sEnd);
        while (found!=std::string::npos)
        {
            // extract message and work on it
            std::string sStr = saveStr + s.substr(0,found);
            b4mesh_->ReceivePacket(sStr, destIP_);

            // erase the message and continue
            s.erase(0, found+3);
            found = s.find(sEnd);
            saveStr = "";
        }
        if ( s.size() != 0)
        {
            saveStr = s;
        }

        //restart reading
        start_reading();
    }
    else
    {
        std::cout << "Error with the session " << error << std::endl;
        socket_.close();
        //removeSessionFromList(destIP_, this);
        //delete this;
    }
}


/*        boost::asio::async_write(socket_,
            boost::asio::buffer(data_, bytes_transferred),
            boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));*/

void session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        //TODO?
    }
    else
    {
        std::cout << "Error with the session " << error << std::endl;
        socket_.close();
        //delete this;
    }
}

void session::closeSocket(void){
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