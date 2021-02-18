
#include "node.hpp"
#include "client.hpp"
#include "b4mesh_p.hpp"
#include "configs.hpp"

node::node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP, bool geneTrans)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      acceptorB_(io_context, tcp::endpoint(tcp::v4(), port+1)),
      consensus_(myIP, conn),
      my_IP(myIP),
      port_(port),
      timer_pollDbus(io_context,std::chrono::steady_clock::now()),
      merge(0), 
      timer_merging(io_context,std::chrono::steady_clock::now())
{
    // blockgraph pointeur
    b4mesh_ = new B4Mesh (this, io_context, port_, myIP, geneTrans);

    //Starting Server part
    std::cout << " starting connection acceptor " << std::endl;
    session* new_session = new session(b4mesh_, io_context_);
    acceptor_.async_accept(new_session->socket(),
    boost::bind(&node::handle_accept, this, new_session, false,
    boost::asio::placeholders::error));

    session* new_sessionB = new session(b4mesh_, io_context_);
    acceptorB_.async_accept(new_sessionB->socket(),
    boost::bind(&node::handle_accept, this, new_sessionB, true,
    boost::asio::placeholders::error));

    //First Intialisation with the client list 
    startListAddr = consensus_.getNodeList();
    std::cout << " try to connect the list " << std::endl;
    for (auto addr = startListAddr.begin(); addr != startListAddr.end(); ++addr)
    {
        create_client(*addr);
    }

    //recurrent task
    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));    

}

// ************** Recurrent Task **************************************
void node::timer_pollDbus_fct (const boost::system::error_code& /*e*/) 
{
	std::cout << "Status Dbus" << std::endl; 
    std::vector<std::string> listAddr = consensus_.getNodeList();
    /*std::cout << "new list " << std::endl;
    for (auto it : listAddr)
    {
        std::cout << it << std::endl;
    }
    std::cout << "start list " << std::endl;
    for (auto it : startListAddr)
    {
        std::cout << it << std::endl;
    }*/

    std::cout << "leader " << consensus_.getLeader() << std::endl;

    for (auto it : listAddr)
    {
        auto Addr = std::find (startListAddr.begin(), startListAddr.end(), it);
        if (Addr == startListAddr.end())
        {
            std::cout << "timer_pollDbus_fct: adding " << it << std::endl;
            create_client(it);
            merge +=2;
        }
    }

    for (auto it : startListAddr)
    {
        auto Addr = std::find (listAddr.begin(), listAddr.end(), it);
        if (Addr == listAddr.end())
        {
            std::cout << "timer_pollDbus_fct: removing " << it << std::endl;
            removeClientFromList(it);
        }
    }

    if (merge > 0)
    {
        std::cout << RED << "Start merge waiting (" << merge << ")" << RESET << std::endl;
        timer_merging.expires_from_now(std::chrono::seconds(MERGE_TIMING));
        timer_merging.async_wait(boost::bind(&node::merge_launcher_fct, this, boost::asio::placeholders::error));
    }

    startListAddr = listAddr;

    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));

}

void node::merge_launcher_fct (const boost::system::error_code& /*e*/) 
{
    std::string ldr = consensus_.getLeader();  
    if ( ldr != "" && merge == 0){
        std::cout << RED << "Node: " << consensus_.GetId() << " Current leader is: " << ldr << RESET << std::endl;
        b4mesh_->StartMerge();
    } else {
        std::cout << RED << "Wait for new leader or merge connexions (" << ldr << " , " << merge << ")" << RESET << std::endl;
        timer_merging.expires_from_now(std::chrono::seconds(MERGE_TIMING));
        timer_merging.async_wait(boost::bind(&node::merge_launcher_fct, this, boost::asio::placeholders::error));
    }
}

void node::addClientToList( std::string IP, client * c, bool block)
{
    // TODO adding a mutex
    std::cout << "addClientToList " << IP << ", block :" << block << std::endl;
    if ( block)
    {
        listClientB.insert(std::pair<std::string, client*>(IP,c));
    } else {
        listClient.insert(std::pair<std::string, client*>(IP,c));
    }
    if ( merge > 0 )
        merge --;
}

void node::removeClientFromList( std::string IP)
{
    // TODO adding a mutex
    if (listClient.find(IP) != listClient.end())
    {
        std::cout << "removeClientFromList " << IP << std::endl;
        listClient[IP]->closeSocket();
        listClient.erase(IP);
    }
    if (listClientB.find(IP) != listClientB.end())
    {
        std::cout << "removeClientBFromList " << IP << std::endl;
        listClientB[IP]->closeSocket();
        listClientB.erase(IP);
    }

    removeSessionFromList(IP);
}

void node::addSessionToList( std::string IP, session * s, bool block)
{
    // TODO adding a mutex
    if (block)
    {
        listSessionB.insert(std::pair<std::string, session*>(IP,s));			
    } else {
        listSession.insert(std::pair<std::string, session*>(IP,s));			
    }
    
}

void node::removeSessionFromList( std::string IP)
{
    // TODO adding a mutex
    if (listSession.find(IP) != listSession.end())
    {
        std::cout << "removeSessionFromList " << IP << std::endl;
        listSession[IP]->closeSocket();
        listSession.erase(IP);
    }
    if (listSessionB.find(IP) != listSessionB.end())
    {
        std::cout << "removeSessionBFromList " << IP << std::endl;
        listSessionB[IP]->closeSocket();
        listSessionB.erase(IP);
    }		
}

void node::create_client(std::string addr)
{
    if (addr != my_IP)
    {
        std::cout << " creation client with : " << addr << std::endl;
        /*client* new_client =*/ new client(this, io_context_, addr, std::to_string(port_), false);
        /*client* new_clientB =*/ new client(this, io_context_, addr, std::to_string(port_+1), true);
    }
}

void node::handle_accept(session* new_session, bool block, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        std::string IP = new_session->socket().remote_endpoint().address().to_string();
        addSessionToList(IP, std::move(new_session), block);
        std::cout << " new connection with : " << IP << std::endl; 
        new_session = new session(b4mesh_, io_context_);
        if (block)
        {
            acceptorB_.async_accept(new_session->socket(),
            boost::bind(&node::handle_accept, this, new_session, true,
            boost::asio::placeholders::error));
        } else {
            acceptor_.async_accept(new_session->socket(),
            boost::bind(&node::handle_accept, this, new_session, false,
            boost::asio::placeholders::error));
        }

    }
    else
    {
        delete new_session;
    }
}


void node::BroadcastPacket( ApplicationPacket& packet, bool block)
{
    std::cout << "BroadcastPacket to : " << listClient.size() << std::endl;
    if (block)
    {
        for (auto const& it : listClientB)
        {
            std::string IP = it.first;
            client* c = it.second;
            std::cout << "(block) message to " << IP << std::endl;
            c->write_message(packet.Serialize());
        }
    } else {
        for (auto const& it : listClient)
        {
            std::string IP = it.first;
            client* c = it.second;
            std::cout << "message to " << IP << std::endl;
            c->write_message(packet.Serialize());
        }
    }
    
}

void node::SendPacket(std::string& IP, ApplicationPacket& packet, bool block)
{
    client* c = NULL;
    if (block)
    {
        auto it = listClientB.find(IP);
        if ( it != listClientB.end()){
            c = it->second;
        }
    } else {
        auto it = listClient.find(IP);
        if ( it != listClient.end()){
            c = it->second;
        }
        
    }

    if (c != NULL)
    {
        //std::cout << "message to " << IP <<  " " << packet << std::endl;
        c->write_message(packet.Serialize());
    } else {
        std::cout << RED << "Client missing : can 't send message to " << IP <<  RESET << " " << packet << std::endl;
        std::cout << "listClientB : " ;
        for (auto client : listClientB)
        {
            std::cout << client.first << "; ";
        }
        std::cout << std::endl << "listClient : ";
        for (auto client : listClient)
        {
            std::cout << client.first << "; ";
        }
        std::cout << std::endl;
    }
    
}

std::string node::GetIp(void)
{
    return my_IP;
}

void node::GenerateResults (void)
{
    b4mesh_->GenerateResults();
}