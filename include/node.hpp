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
    
    void addClientToList(std::string IP, client * c);
    void removeClientFromList(std::string IP);
    void addSessionToList(std::string IP, session * s);
    void removeSessionFromList(std::string IP);
    void handle_accept(session* new_session, const boost::system::error_code& error);

    void SendPacket(std::string& IP, ApplicationPacket& packet);
	void BroadcastPacket( ApplicationPacket& packet);

    std::string GetIp(void);
    void GenerateResults (void);

private:
    void SendOnNetwork(tcp::socket& socket, std::string string);
    void create_client(std::string addr);

private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;

public: 
    consensus consensus_;
    B4Mesh* b4mesh_;

private:
    std::string my_IP;
    short port_;
    std::vector<std::string> startListAddr; 
    std::map<std::string, session*> listSession;
    std::map<std::string, client*> listClient;

private:
    // polling Dbus
    void timer_pollDbus_fct (const boost::system::error_code& /*e*/);
    boost::asio::steady_timer timer_pollDbus;
    int merge;

};



#endif //B4MESH_NODE

