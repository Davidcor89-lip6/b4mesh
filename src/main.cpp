#include <cstdlib>
#include <iostream>
#include <string>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include <dbus-c++/dbus.h>
#include <dbus-c++/asio-integration.h>

using boost::asio::ip::tcp;

#include "node.hpp"
#include "utils.hpp"
#include "configs.hpp"


int main(int argc, char* argv[])
{
  try
  {

	int opt = 0;
	const char *arg_mIP = NULL;
	const char *arg_port = NULL;

	std::istringstream iss_name(argv[0]);
    std::vector<std::string> split_line_name(std::istream_iterator<std::string>{iss_name}, std::istream_iterator<std::string>());

    std::string usage = "./b4mesh -i <my IP> -p <port>";

	// options parsing
    while ((opt = getopt(argc, argv, "i:p:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            arg_mIP = optarg;
            //std::cout << "my IP : " << arg_mIP << std::endl;
            break;
        case 'p':
            arg_port = optarg;
            //std::cout << "used port : " << arg_port << std::endl;
            break;
        case '?':
        case 'h':
            std::cout << usage << std::endl;
            return -1;
            break;
        }
    }

	
	std::string myIP;
	short port;

	if (arg_mIP == NULL)
	{
		myIP = findOwnIP();
	} else {
		myIP = arg_mIP;
	}
	if (arg_port != NULL)
	{
		port = std::atoi(arg_port);
	} else {
		port = DEFAULT_PORT;
	}

	std::cout << " -> " << myIP << ":" << port << std::endl;
	
	//create network service
    boost::asio::io_context io_context;
	
	DBus::Asio::Dispatcher dispatcher{io_context};
  	DBus::default_dispatcher = &dispatcher;
  	boost::asio::signal_set sighandler{io_context, SIGINT, SIGTERM};

	DBus::Connection conn = DBus::Connection::SystemBus();
	std::cout << "Connection Dbus ok" << std::endl;

	//create b4mesh node
    node s(io_context, conn, port, myIP);

	sighandler.async_wait([&io_context, &s](const boost::system::error_code&, const int&) {
		std::cout << " Signal that end the program" << std::endl;
    	io_context.stop();
		s.GenerateResults();
  	});

	//launch network service
    io_context.run();

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

