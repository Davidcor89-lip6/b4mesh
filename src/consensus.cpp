#include "consensus.hpp"
#include <thread>
#include <chrono>
#include <algorithm>

/**  green state method **/

green_state::green_state(DBus::Connection &connection, const char *path, const char *name)
    : DBus::ObjectProxy(connection, path, name)
{
}

void green_state::Added(const std::string& address)
{
    std::cout << "green_state: Added " << address << std::endl;
    addListAddr.push_back(address);
}

void green_state::Removed(const std::string& address)
{
    std::cout << "green_state: Removed " << address << std::endl;
    removeListAddr.push_back(address);
}

/** consensus methods **/
static const char *GSTATE_SERVER_NAME = "net.qolsr.Qolyester";
static const char *GSTATE_SERVER_PATH = "/net/qolsr/Qolyester/State";

consensus::consensus(std::string mIP)
    : mIP_(mIP) 
{

    /* compute ID */
    ID_ = GetIdFromIP(mIP_);
    std::cout << "my ID : " << ID_ << std::endl;

    /* init leader */
    leader_ = "10.181.178.217";

    /* connexion to DBUS green state */ 
    //initialisation_dbus();
    std::thread t([this]() {
        try
        {
            std::cout << "Start DBUS Connection" << std::endl;

            DBus::default_dispatcher = &dispatcher;

            DBus::Connection conn = DBus::Connection::SystemBus();

            std::cout << "Connection Dbus ok" << std::endl;

            green_state client(conn, GSTATE_SERVER_PATH, GSTATE_SERVER_NAME);
            p_clientState = &client;

            std::cout << "client Dbus ok" << std::endl;

            listAddr = client.GetAll(); 

            for (auto	it = listAddr.begin(); it != listAddr.end(); ++it)
            {
                std::cout << *it << std::endl;
            }

            dispatcher.enter();
            std::cout << "DBUS terminated" << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    });
    t.detach();

    // wait for the connexion DBUS
    std::this_thread::sleep_for (std::chrono::seconds(1));

}

void consensus::terminate_dbus(void){
    dispatcher.leave();
}

std::vector<std::string>& consensus::getNodeList(void)
{
    return listAddr;
}

std::vector<std::string>& consensus::getAddNodeList(void)
{
    return p_clientState->addListAddr;
}

std::vector<std::string>& consensus::getRemoveNodeList(void)
{
    return p_clientState->removeListAddr;
}

void consensus::removeFromAddAddr(std::string addr)
{
    std::vector<std::string>::iterator position = std::find(p_clientState->addListAddr.begin(), p_clientState->addListAddr.end(), addr);
    if (position != p_clientState->addListAddr.end()) // == p_clientState->addListAddr.end() means the element was not found
        p_clientState->addListAddr.erase(position);
}

void consensus::removeFromRemoveAddr(std::string addr)
{
    std::vector<std::string>::iterator position = std::find(p_clientState->removeListAddr.begin(), p_clientState->removeListAddr.end(), addr);
    if (position != p_clientState->removeListAddr.end()) //  not found
        p_clientState->removeListAddr.erase(position);
}

void consensus::addToListAddr(std::string addr)
{
    listAddr.push_back(addr);
}

void consensus::removeFromListAddr(std::string addr)
{
    std::vector<std::string>::iterator position = std::find(listAddr.begin(), listAddr.end(), addr);
    if (position != listAddr.end()) // not found
        listAddr.erase(position);
}

std::string consensus::getLeader(void)
{
    return leader_;
}

bool consensus::AmILeader(void)
{
    return (leader_ == mIP_)? 1: 0;
}

int consensus::GetId()
{
    return ID_;
}

int consensus::GetIdFromIP(std::string s)
{
    size_t pos = 0;
    std::string token;
    std::string delimiter = ".";
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        //std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    return std::stoi(s);
}

std::string consensus::GetIPFromId(int id)
{
    if ( id == GetIdFromIP(mIP_) )
    {
        return mIP_;
    }
    for (auto it = listAddr.begin(); it != listAddr.end(); ++it)
    {
        if ( id == GetIdFromIP(*it) )
        {
            return *it;
        }
    }

    std::string r("-1");
    return r;
}

std::string consensus::GetGroupId()
{
    std::string g = std::to_string(GetIdFromIP(leader_)); // group id is the id of the leader
    return g;
}

