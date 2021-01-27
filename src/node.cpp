
#include "node.hpp"
#include "client.hpp"
#include "b4mesh_p.hpp"
#include "configs.hpp"

node::node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      consensus_(myIP, conn),
      my_IP(myIP),
      port_(port),
      timer_pollDbus(io_context,std::chrono::steady_clock::now())
{
    // blockgraph pointeur
    b4mesh_ = new B4Mesh (this, io_context, port_, myIP);

    //Starting Server part
    std::cout << " starting connection acceptor " << std::endl;
    session* new_session = new session(b4mesh_, io_context_);
    acceptor_.async_accept(new_session->socket(),
    boost::bind(&node::handle_accept, this, new_session,
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
    int merge = 0;
	std::cout << "Status Dbus" << std::endl; 
    std::vector<std::string> listAddr = consensus_.getNodeList();
    std::cout << "new list " << std::endl;
    for (auto it : listAddr)
    {
        std::cout << it << std::endl;
    }
    std::cout << "start list " << std::endl;
    for (auto it : startListAddr)
    {
        std::cout << it << std::endl;
    }

    std::cout << "leader " << std::endl;
    std::string leader = consensus_.getLeader();
    std::cout << leader << std::endl;

    for (auto it : listAddr)
    {
        auto Addr = std::find (startListAddr.begin(), startListAddr.end(), it);
        if (Addr == startListAddr.end())
        {
            std::cout << "timer_pollDbus_fct: adding " << it << std::endl;
            create_client(it);
            merge++;
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
        std::string ldr = "";
        while (ldr == "")
        {
            ldr = consensus_.getLeader();  
            if ( ldr != ""){
                cout << "Node: " << consensus_.GetId() << "Current leader is: " << ldr << endl;
                b4mesh_->StartMerge();
                return;
            } 
            usleep(500);
        }
        merge = 0;
    }

    startListAddr = listAddr;

    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));

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

void node::create_client(std::string addr)
{
    if (addr != my_IP)
    {
        std::cout << " creation client with : " << addr << std::endl;
        /*client* new_client =*/ new client(this, io_context_, addr, std::to_string(port_));
    }
}

void node::handle_accept(session* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        std::string IP = new_session->socket().remote_endpoint().address().to_string();
        addSessionToList(IP, std::move(new_session));
        std::cout << " new connection with : " << IP << std::endl; 
        new_session = new session(b4mesh_, io_context_);
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