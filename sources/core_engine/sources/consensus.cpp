#include <b4mesh/core_engine/consensus.hpp>
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
}

void green_state::Removed(const std::string& address)
{
    std::cout << "green_state: Removed " << address << std::endl;
}

/** consensus method **/
green_cons::green_cons(DBus::Connection &connection, const char *path, const char *name)
    : DBus::ObjectProxy(connection, path, name)
{
}

void green_cons::ValueChanged(const std::string& name, const std::string& new_version, const std::vector< uint8_t >& new_value, const double& expiration)
{
    std::cout << "green_cons::ValueChanged " << std::endl;
}
void green_cons::TransactionResult(const uint32_t& id, const bool& success, const std::map< std::string, std::string >& changed_keys)
{
    std::cout << "green_cons::TransactionResult " << std::endl;
}
void green_cons::WatchResult(const std::vector< ::DBus::Struct< std::string, std::string > >& changed_keys_versions, const std::map< std::string, ::DBus::Struct< std::string, std::vector< uint8_t >, double > >& new_keys)
{
    std::cout << "green_cons::WatchResult " << std::endl;
}


//DBUS connection information
static const char *GSTATE_SERVER_NAME_STATE = "net.qolsr.Qolyester";
static const char *GSTATE_SERVER_PATH_STATE = "/net/qolsr/Qolyester/State";

static const char *GSTATE_SERVER_NAME_CONS = "net.qolsr.Consensus";
static const char *GSTATE_SERVER_PATH_CONS = "/net/qolsr/Consensus";

consensus::consensus(std::string mIP, DBus::Connection& conn)
    : mIP_(mIP),
      conn_(conn),
      state_client(conn, GSTATE_SERVER_PATH_STATE, GSTATE_SERVER_NAME_STATE),
      cons_client(conn, GSTATE_SERVER_PATH_CONS, GSTATE_SERVER_NAME_CONS)
{

    /* compute ID */
    ID_ = GetIdFromIP(mIP_);
    std::cout << "my ID : " << ID_ << std::endl;

    /* init green info */
    leader_ = cons_client.Leader();
    listAddr = state_client.GetAll(); 

}


std::vector<std::string>& consensus::getNodeList(void)
{
    listAddr = state_client.GetAll();
    return listAddr;
}


std::string consensus::getLeader(void)
{
    leader_ = cons_client.Leader();
    return leader_;
}

bool consensus::AmILeader(void)
{
    bool ret = false;
    leader_ = cons_client.Leader();
    if ( leader_ != "")
        if (leader_ == mIP_)
            ret = true;
    return ret;
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
    getNodeList();
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
/*
std::string consensus::GetGroupId()
{
    std::string g = std::to_string(GetIdFromIP(leader_)); // group id is the id of the leader
    return g;
}
*/

