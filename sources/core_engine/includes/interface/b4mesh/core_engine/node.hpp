#ifndef B4MESH_NODE
#define B4MESH_NODE

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <dbus-c++/dbus.h>
#include <dbus-c++/asio-integration.h>

using boost::asio::ip::tcp;

#include <b4mesh/core_engine/consensus.hpp>
#include <b4mesh/core_engine/session.hpp>
#include <b4mesh/core_engine/application_packet.hpp>
#include <b4mesh/core_engine/block.hpp>

// Class declaration to avoid circular dependency
class client;
class B4Mesh;

class node
{
public:
<<<<<<< Updated upstream
    node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP);
=======
    node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP, bool geneTrans);
>>>>>>> Stashed changes
    
    void addClientToList(std::string IP, client * c, bool block);
    void removeClientFromList(std::string IP);
    void addSessionToList(std::string IP, session * s, bool block);
    void removeSessionFromList(std::string IP);
    void handle_accept(session* new_session, bool block, const boost::system::error_code& error);

    void SendPacket(std::string& IP, ApplicationPacket& packet, bool block);
	void BroadcastPacket( ApplicationPacket& packet, bool block);

    std::string GetIp(void);
    void GenerateResults (void);
<<<<<<< Updated upstream

private:
    void SendOnNetwork(tcp::socket& socket, std::string string);
=======
    void RegisterTransaction (std::string payload);

    std::vector<std::string> GetcurrentListAddr(void) { return currentListAddr; }
    std::vector<std::string> GetpreviousListAddr(void) { return previousListAddr; }
    std::vector<pair<int, std::string>> GetnewNodes(void) { return new_nodes; }
    void ClearnewNodes(void) { new_nodes.clear(); }
    void setGroupId (std::vector<std::string> new_listnodes);
    std::string GetGroupId(void);
    
private:
>>>>>>> Stashed changes
    void create_client(std::string addr);

private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    tcp::acceptor acceptorB_;

public: 
    consensus consensus_;
    B4Mesh* b4mesh_;

private:
    std::string my_IP;
<<<<<<< Updated upstream
    short port_;
    std::vector<std::string> startListAddr; 
=======
    std::string groupId;
    short port_;
    std::vector<std::string> currentListAddr; 
    std::vector<std::string> previousListAddr; 
    std::vector<pair<int, std::string>> new_nodes;
>>>>>>> Stashed changes
    std::map<std::string, session*> listSession;
    std::map<std::string, client*> listClient;
    std::map<std::string, session*> listSessionB;
    std::map<std::string, client*> listClientB;

private:
    // polling Dbus
    void timer_pollDbus_fct (const boost::system::error_code& /*e*/);
    boost::asio::steady_timer timer_pollDbus;
    // merge
    int merge;
    void merge_launcher_fct (const boost::system::error_code& /*e*/);
    boost::asio::steady_timer timer_merging;

};

<<<<<<< Updated upstream
=======


>>>>>>> Stashed changes
#endif //B4MESH_NODE

