#ifndef B4MESH_NODE
#define B4MESH_NODE

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/asio/steady_timer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

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
    node(boost::asio::io_service& io_service, short port, std::string myIP);
    
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

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;

public: 
    consensus consensus_;
    B4Mesh* b4mesh_;

private:
    std::string my_IP;
    std::vector<std::string> startListAddr; 
    std::map<std::string, session*> listSession;
    std::map<std::string, client*> listClient;

};



#endif //B4MESH_NODE

