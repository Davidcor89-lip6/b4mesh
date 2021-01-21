#include "utils.hpp"

//----------------------------
// find own IP

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) {
    boost::asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return boost::asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

std::vector<boost::asio::ip::address> get_local_interfaces(void) {
    std::vector<boost::asio::ip::address> res;
    ifaddrs *ifs;
    if (getifaddrs(&ifs)) {
        return res;
    }
    for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) {
        // No address? Skip.
        if (addr->ifa_addr == nullptr) continue;

        // Interface isn't active? Skip.
        if (!(addr->ifa_flags & IFF_UP)) continue;

        if(addr->ifa_addr->sa_family == AF_INET) {
            res.push_back(boost::asio::ip::make_address_v4(ntohl(
                reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
        } else if(addr->ifa_addr->sa_family == AF_INET6) {
            res.push_back(sinaddr_to_asio(reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
        } else continue;
    }
    freeifaddrs(ifs);
    return res;
}

std::string findOwnIP(void)
{
	std::vector<boost::asio::ip::address> vectAddr = get_local_interfaces();
    for (auto i = vectAddr.begin(); i != vectAddr.end(); ++i)
    {
        //std::cout << (*i).to_string()<< ' ';
        if ((*i).to_string().compare(0, 3, "10.") == 0) // All IP in 10.X.X.X
        {
            return  (*i).to_string();
        }
    }
    //std::cout << std::endl;

	return std::string("Error");
}