#include <b4mesh/core_engine/node.hpp>
#include <b4mesh/core_engine/client.hpp>
#include <b4mesh/core_engine/b4mesh_p.hpp>
#include <b4mesh/core_engine/configs.hpp>

<<<<<<< Updated upstream
node::node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP)
=======


node::node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP, bool geneTrans)
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
    b4mesh_ = new B4Mesh (this, io_context, port_, myIP);
=======
    b4mesh_ = new B4Mesh (this, io_context, port_, myIP, geneTrans);
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
    startListAddr = consensus_.getNodeList();
    std::cout << " try to connect the list " << std::endl;
    for (auto addr = startListAddr.begin(); addr != startListAddr.end(); ++addr)
=======
    currentListAddr = consensus_.getNodeList();
    std::cout << " try to connect the list " << std::endl;
    for (auto addr = currentListAddr.begin(); addr != currentListAddr.end(); ++addr)
>>>>>>> Stashed changes
    {
        create_client(*addr);
    }

    //recurrent task
    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));    

}

<<<<<<< Updated upstream
=======
std::string node::GetGroupId()
{
    return groupId;
}

void node::setGroupId (std::vector<std::string> new_listnodes){

    std::string groupstring;
    std::vector<std::string> tmplist = std::vector<std::string> ();
   
    // Add myself to the list of nodes
    tmplist.push_back(GetIp());
    for (auto &ip : new_listnodes)
    {
        tmplist.push_back(ip);
    }
    // sort the list so every node have the same order 
    std::sort (tmplist.begin(), tmplist.end());
 
    std::cout << " List of nodes: " << std::endl;
    for (auto &ip :tmplist){
        std::cout << ip << std::endl;
        // Creating the string from which the groupId will be calculate
        groupstring += ip;
    }
    groupId = hashing(groupstring); 
    std::cout << " Group Id is: " << groupId << std::endl;
}


>>>>>>> Stashed changes
// ************** Recurrent Task **************************************
void node::timer_pollDbus_fct (const boost::system::error_code& /*e*/) 
{
	std::cout << "Status Dbus" << std::endl; 
<<<<<<< Updated upstream
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
=======

    previousListAddr = currentListAddr;
    currentListAddr = consensus_.getNodeList();
    bool findNode = false;

    std::cout << "leader " << consensus_.getLeader() << std::endl;

    for (auto it : currentListAddr)
    {
        auto Addr = std::find (previousListAddr.begin(), previousListAddr.end(), it);
        if (Addr == previousListAddr.end())
>>>>>>> Stashed changes
        {
            std::cout << "timer_pollDbus_fct: adding " << it << std::endl;
            create_client(it);
            merge +=2;
<<<<<<< Updated upstream
        }
    }

    for (auto it : startListAddr)
    {
        auto Addr = std::find (listAddr.begin(), listAddr.end(), it);
        if (Addr == listAddr.end())
        {
=======
            int ID = consensus_.GetIdFromIP(it);
            new_nodes.push_back(pair<int, std::string>(ID, it));
        }
    }

    for (auto it : previousListAddr)
    {
        auto Addr = std::find (currentListAddr.begin(), currentListAddr.end(), it);
        if (Addr == currentListAddr.end())
        {
            // This code avoids the case when a node was added as client but sockets weren't stablished
            // This cause that the program was waiting to connect to a node that is not longer in the group.
            if (merge > 0){
                findNode = false;
                for (auto &ip : listClientB){
                    if (it == ip.first){
                        findNode = true;
                    }
                }
                if (!findNode){
                    std::cout << "Decrementing merge before removing a node that could't connect " << endl;
                    merge-=2;
                }
            }
>>>>>>> Stashed changes
            std::cout << "timer_pollDbus_fct: removing " << it << std::endl;
            removeClientFromList(it);
        }
    }
<<<<<<< Updated upstream

    if (merge > 0)
    {
=======
    
    if (merge > 0)
    {
        b4mesh_->setCreateBlock(false);
>>>>>>> Stashed changes
        std::cout << RED << "Start merge waiting (" << merge << ")" << RESET << std::endl;
        timer_merging.expires_from_now(std::chrono::seconds(MERGE_TIMING));
        timer_merging.async_wait(boost::bind(&node::merge_launcher_fct, this, boost::asio::placeholders::error));
    }

<<<<<<< Updated upstream
    startListAddr = listAddr;

    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));

=======
    setGroupId(currentListAddr);

    timer_pollDbus.expires_from_now(std::chrono::seconds(POLLING_DBUS));
	timer_pollDbus.async_wait(boost::bind(&node::timer_pollDbus_fct, this, boost::asio::placeholders::error));
    
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
    }
    else
    {
        delete new_session;
    }
}

<<<<<<< Updated upstream
void node::SendOnNetwork(tcp::socket& socket, std::string string)
{
    //adding symbole to the end 
    std::string s = string + std::string("\r\n");
    try
    {
        //std::cout << "message: sending size " << string.size() << " size+ender : " << s.size() << std::endl;
        boost::asio::write(socket, boost::asio::buffer(s, s.size()));
    }
    catch(const std::exception& e)
    {
        std::cout << "Error: SendOnNetwork :" << e.what() << std::endl;
    }
    
    
}
=======
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
            SendOnNetwork(c->socket(), packet.Serialize());
=======
            c->write_message(packet.Serialize());
>>>>>>> Stashed changes
        }
    } else {
        for (auto const& it : listClient)
        {
            std::string IP = it.first;
            client* c = it.second;
            std::cout << "message to " << IP << std::endl;
<<<<<<< Updated upstream
            SendOnNetwork(c->socket(), packet.Serialize());
=======
            c->write_message(packet.Serialize());
>>>>>>> Stashed changes
        }
    }
    
}

void node::SendPacket(std::string& IP, ApplicationPacket& packet, bool block)
{
    client* c = NULL;
    if (block)
    {
<<<<<<< Updated upstream
        c = listClientB.find(IP)->second;
    } else {
        c = listClient.find(IP)->second;
=======
        auto it = listClientB.find(IP);
        if ( it != listClientB.end()){
            c = it->second;
        }
    } else {
        auto it = listClient.find(IP);
        if ( it != listClient.end()){
            c = it->second;
        }
        
>>>>>>> Stashed changes
    }

    if (c != NULL)
    {
        //std::cout << "message to " << IP <<  " " << packet << std::endl;
<<<<<<< Updated upstream
        SendOnNetwork(c->socket(), packet.Serialize());
=======
        c->write_message(packet.Serialize());
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
}
=======
}

void node::RegisterTransaction (std::string payload)
{
    b4mesh_->RegisterTransaction(payload);
}
>>>>>>> Stashed changes
