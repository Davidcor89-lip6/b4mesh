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
	const char *arg_autogen = NULL;

	std::istringstream iss_name(argv[0]);
    std::vector<std::string> split_line_name(std::istream_iterator<std::string>{iss_name}, std::istream_iterator<std::string>());

    std::string usage = "./b4mesh -i <my IP> -p <port> -r true";
	std::string usage2 = "-i allows to force the IP \n-p allows to change the used port (5000) \n-r desactived the Auto generation of the transaction";

	// options parsing
    while ((opt = getopt(argc, argv, "i:p:r:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            arg_mIP = optarg;
            break;
        case 'p':
            arg_port = optarg;
            break;
		case 'r':
            arg_autogen = optarg;
            break;
        case '?':
        case 'h':
            std::cout << usage << std::endl;
			std::cout << usage2 << std::endl;
            return -1;
            break;
        }
    }

	
	std::string myIP;
	short port;
	bool geneTrans = true;

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
	if (arg_autogen != NULL)
	{
		geneTrans = false;
	}

	std::cout << " -> " << myIP << ":" << port << ", autoGeneTrans : " << geneTrans << std::endl;
	
	//create network service
    boost::asio::io_context io_context;
	
	DBus::Asio::Dispatcher dispatcher{io_context};
  	DBus::default_dispatcher = &dispatcher;
  	boost::asio::signal_set sighandler{io_context, SIGINT, SIGTERM};

	DBus::Connection conn = DBus::Connection::SystemBus();
	std::cout << "Connection Dbus ok" << std::endl;

	//create b4mesh node
    node s(io_context, conn, port, myIP, geneTrans);

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

