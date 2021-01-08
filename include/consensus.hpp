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

	void initialisation_dbus(void);
	
private:
	std::string mIP_;
	int ID_;
    std::string leader_;
    std::vector<std::string> listAddr;

	/*dbus*/
	DBus::BusDispatcher dispatcher;

};


#endif //B4MESH_CLIENT