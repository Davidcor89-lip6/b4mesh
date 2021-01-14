#ifndef B4MESH_CONSENSUS
#define B4MESH_CONSENSUS

#include <iostream>
#include <dbus-c++/dbus.h>
#include "state_glue.h"


class green_state :
	public net::qolsr::Qolyester::ReachableNodes_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
public:

	green_state(DBus::Connection &connection, const char *path, const char *name);


	void Added(const std::string& address);
	void Removed(const std::string& address);
	
	std::vector<std::string> addListAddr;
	std::vector<std::string> removeListAddr;

};

class consensus
{
public:
	consensus(std::string mIP);
	
	std::vector<std::string>& getNodeList(void);	
	std::string getLeader(void);
	bool AmILeader(void);
	int GetId();
	std::string GetIPFromId(int id);
	int GetIdFromIP(std::string s);
	std::string GetGroupId();

	void terminate_dbus(void);
	std::vector<std::string>& getAddNodeList(void);
	std::vector<std::string>& getRemoveNodeList(void);
	void removeFromAddAddr(std::string addr);
	void removeFromRemoveAddr(std::string addr);
	
private:
	std::string mIP_;
	int ID_;
    std::string leader_;
    std::vector<std::string> listAddr;

	/*dbus*/
	DBus::BusDispatcher dispatcher;
	green_state * p_clientState;

};


#endif //B4MESH_CLIENT