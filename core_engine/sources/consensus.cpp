#include <b4mesh/core_engine/consensus.hpp>

/**  green state method **/

green_state::green_state(DBus::Connection &connection, const char *path, const char *name)
    : DBus::ObjectProxy(connection, path, name)
{
}

void green_state::Added(const std::string& address)
{
    std::cout << "Added " << address << std::endl;
}

void green_state::Removed(const std::string& address)
{
    std::cout << "Removed " << address << std::endl;
}

/** consensus methods **/

consensus::consensus(std::string mIP)
    : mIP_(mIP) 
{

    /* compute ID */
    ID_ = GetIdFromIP(mIP_);
    std::cout << "my ID : " << ID_ << std::endl;

    /* init leader */
    leader_ = "10.181.178.217";

    /* connexion to DBUS green state */ 
    initialisation_dbus();

}

static const char *GSTATE_SERVER_NAME = "net.qolsr.Qolyester";
static const char *GSTATE_SERVER_PATH = "/net/qolsr/Qolyester/State";

void consensus::initialisation_dbus(void)
{

    DBus::default_dispatcher = &dispatcher;

    DBus::Connection conn = DBus::Connection::SystemBus();

    std::cout << "Connection Dbus ok" << std::endl;

    green_state client(conn, GSTATE_SERVER_PATH, GSTATE_SERVER_NAME);

    std::cout << "client Dbus ok" << std::endl;

    listAddr = client.GetAll(); 

    for (auto	it = listAddr.begin(); it != listAddr.end(); ++it)
    {
        std::cout << *it << std::endl;
    }


}

std::vector<std::string>& consensus::getNodeList(void)
{
    return listAddr;
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

