
#include "node.hpp"
#include "client.hpp"
#include "b4mesh_p.hpp"


node::node(boost::asio::io_service& io_service, short port, std::string myIP)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      consensus_(myIP),
      my_IP(myIP)
{
    // blockgraph pointeur
    b4mesh_ = new B4Mesh (this, io_service, port, myIP);

    //Starting Server part
    std::cout << " starting connection acceptor " << std::endl;
    session* new_session = new session(b4mesh_, io_service_);
    acceptor_.async_accept(new_session->socket(),
    boost::bind(&node::handle_accept, this, new_session,
    boost::asio::placeholders::error));

    //First Intialisation with the client list 
    startListAddr = consensus_.getNodeList();
    std::cout << " try to connect the list " << std::endl;
    for (auto addr = startListAddr.begin(); addr != startListAddr.end(); ++addr)
    {
        if (*addr != my_IP)
        {
        std::cout << " creation client with : " << *addr << std::endl;
        /*client* new_client =*/ new client(this, io_service_, *addr, std::to_string(port));
        }
    }
    

}

void node::addClientToList( std::string IP, client * c)
{
    // TODO adding a mutex
    listClient.insert(std::pair<std::string, client*>(IP,c));			
}

void node::removeClientFromList( std::string IP)
{
    // TODO adding a mutex
    listClient.erase(IP);			
}

void node::addSessionToList( std::string IP, session * s)
{
    // TODO adding a mutex
    listSession.insert(std::pair<std::string, session*>(IP,s));			
}

void node::removeSessionFromList( std::string IP)
{
    // TODO adding a mutex
    listSession.erase(IP);			
}

void node::handle_accept(session* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        std::string IP = new_session->socket().remote_endpoint().address().to_string();
        addSessionToList(IP, std::move(new_session));
        std::cout << " new connection with : " << IP << std::endl; 
        new_session = new session(b4mesh_, io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&node::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }
    else
    {
        delete new_session;
    }
}

void node::SendOnNetwork(tcp::socket& socket, std::string string)
{
    //adding symbole to the end 
    std::string s = string + std::string("\r\n");
    //std::cout << "message: sending size " << string.size() << " size+ender : " << s.size() << std::endl;
    boost::asio::write(socket, boost::asio::buffer(s, s.size()));
}

void node::BroadcastPacket( ApplicationPacket& packet)
{

    for (auto const& it : listClient)
    {
        std::string IP = it.first;
        client* c = it.second;
        //std::cout << "message to " << IP <<  " " << packet << std::endl;
        SendOnNetwork(c->socket(), packet.Serialize());
    }
}

void node::SendPacket(std::string& IP, ApplicationPacket& packet)
{
    client* c = listClient.find(IP)->second;
    //std::cout << "message to " << IP <<  " " << packet << std::endl;
    SendOnNetwork(c->socket(), packet.Serialize());
}

std::string node::GetIp(void)
{
    return my_IP;
}

void node::GenerateResults (void)
{
    b4mesh_->GenerateResults();
}