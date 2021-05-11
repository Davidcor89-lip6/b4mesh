#ifndef B4MESH_CONSENSUS
#define B4MESH_CONSENSUS

#include <iostream>
#include <dbus-c++/dbus.h>
#include <dbus-c++/asio-integration.h>
#include <b4mesh/core_engine/generated/state.hpp>
#include <b4mesh/core_engine/generated/consensus.hpp>

class green_state :
	public net::qolsr::Qolyester::ReachableNodes_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
public:

	green_state(DBus::Connection &connection, const char *path, const char *name);


	void Added(const std::string& address);
	void Removed(const std::string& address);

};

class green_cons :
	public net::qolsr::Consensus_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
public:

	green_cons(DBus::Connection &connection, const char *path, const char *name);


	void ValueChanged(const std::string& name, const std::string& new_version, const std::vector< uint8_t >& new_value, const double& expiration);
    void TransactionResult(const uint32_t& id, const bool& success, const std::map< std::string, std::string >& changed_keys);
    void WatchResult(const std::vector< ::DBus::Struct< std::string, std::string > >& changed_keys_versions, const std::map< std::string, ::DBus::Struct< std::string, std::vector< uint8_t >, double > >& new_keys);

};

class consensus
{
public:
	consensus(std::string mIP, DBus::Connection& conn);
	
	std::vector<std::string>& getNodeList(void);	
	std::string getLeader(void);
	bool AmILeader(void);
	int GetId();
	std::string GetIPFromId(int id);
	int GetIdFromIP(std::string s);
	std::string GetGroupId();

	std::vector<std::string>& getAddNodeList(void);
	std::vector<std::string>& getRemoveNodeList(void);
	void removeFromAddAddr(std::string addr);
	void removeFromRemoveAddr(std::string addr);

	void addToListAddr(std::string addr);
	void removeFromListAddr(std::string addr);
	
private:
	std::string mIP_;
	int ID_;
    std::string leader_;
    std::vector<std::string> listAddr;

	/*dbus*/
	DBus::Connection conn_;
	green_state state_client;
	green_cons cons_client;

};


#endif //B4MESH_CLIENT
