#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <set>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <dirent.h>
#include <random>

//find own IP
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#define RANDOM_SEED 0

using namespace std;

inline string dump(const char* data, int size) {
	std::ostringstream buf;
	buf << "0x";
	for (int i = 0; i < size; ++i)
		buf << setfill('0') << setw(2) << hex << (unsigned int)((unsigned char)data[i]);
	return buf.str();
};

inline vector<string> tokenize(const string& s, const string& delimiters = " ") {
	vector<string> tokens;

	// Skip delimiters at beginning
	auto lastPos = s.find_first_not_of(delimiters, 0);

	// Find the first "non-delimiter"
	auto pos = s.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector
		tokens.push_back(s.substr(lastPos, pos - lastPos));

		// Find next word
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

inline vector<string> listDir(string dirname) {
	DIR *dir;
	struct dirent *ent;
	vector<string> ret;
	if ((dir = opendir(dirname.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG)
				ret.push_back(ent->d_name);
		}
		closedir(dir);
	}
	return ret;
}

inline string hashing(const string& data, int hash_size = 20){
  string ret = "";

  static int count = 0;
  count++;
  ret = to_string(count);
  ret = ret + string(hash_size - ret.size(), 0);

  return ret;
}

inline int poisson_rand(){
//  static random_device rd;
//  static mt19937 gen(rd());
  static mt19937 gen(RANDOM_SEED);
  static poisson_distribution<> poisson_gen(4);
  
  return poisson_gen(gen);
}

inline int uniform_rand(int min=0, int max=100000){
//  static random_device rd;
//  static mt19937 gen(rd());
  static mt19937 gen(RANDOM_SEED);
  static uniform_int_distribution<> uniform_gen(0, 100000);
  float draw = uniform_gen(gen); 
  float prop = draw / 100000;
  return (prop * (max-min)) + min;
//  return (uniform_gen(gen) % (max - min)) + min;
}

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr);
std::vector<boost::asio::ip::address> get_local_interfaces(void);
std::string findOwnIP(void);

#endif
