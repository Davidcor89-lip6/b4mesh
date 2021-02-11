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

#include "consensus.hpp"
#include "session.hpp"

#include "application_packet.hpp"
#include "block.hpp"

// Class declaration to avoid circular dependancy
class client;
class B4Mesh;

class node
{
public:
    node(boost::asio::io_context& io_context, DBus::Connection& conn, short port, std::string myIP);
    
    void addClientToList(std::string IP, client * c, bool block);
    void removeClientFromList(std::string IP);
    void addSessionToList(std::string IP, session * s, bool block);
    void removeSessionFromList(std::string IP);
    void handle_accept(session* new_session, bool block, const boost::system::error_code& error);

    void SendPacket(std::string& IP, ApplicationPacket& packet, bool block);
	void BroadcastPacket( ApplicationPacket& packet, bool block);

    std::string GetIp(void);
    void GenerateResults (void);

private:
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
    short port_;
    std::vector<std::string> startListAddr; 
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



#endif //B4MESH_NODE

