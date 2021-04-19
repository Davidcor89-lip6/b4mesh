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

#include <b4mesh/core_engine/node.hpp>
#include <b4mesh/core_engine/configs.hpp>
#include <b4mesh/utils/utils.hpp>

<<<<<<< Updated upstream
=======

#include <b4mesh/http/endpoint.hpp>



>>>>>>> Stashed changes
int main(int argc, char* argv[])
{
  try
  {

	int opt = 0;
	const char *arg_mIP = NULL;
	const char *arg_port = NULL;
<<<<<<< Updated upstream
=======
	const char *arg_autogen = NULL;
>>>>>>> Stashed changes

	std::istringstream iss_name(argv[0]);
    std::vector<std::string> split_line_name(std::istream_iterator<std::string>{iss_name}, std::istream_iterator<std::string>());

<<<<<<< Updated upstream
    std::string usage = "./b4mesh -i <my IP> -p <port>";

	// options parsing
    while ((opt = getopt(argc, argv, "i:p:h")) != -1)
=======
    std::string usage = "./b4mesh -i <my IP> -p <port> -r true";
	std::string usage2 = "-i allows to force the IP \n-p allows to change the used port (5000) \n-r desactived the Auto generation of the transaction";

	// options parsing
    while ((opt = getopt(argc, argv, "i:p:r:h")) != -1)
>>>>>>> Stashed changes
    {
        switch (opt)
        {
        case 'i':
            arg_mIP = optarg;
<<<<<<< Updated upstream
            //std::cout << "my IP : " << arg_mIP << std::endl;
            break;
        case 'p':
            arg_port = optarg;
            //std::cout << "used port : " << arg_port << std::endl;
=======
            break;
        case 'p':
            arg_port = optarg;
            break;
		case 'r':
            arg_autogen = optarg;
>>>>>>> Stashed changes
            break;
        case '?':
        case 'h':
            std::cout << usage << std::endl;
<<<<<<< Updated upstream
=======
			std::cout << usage2 << std::endl;
>>>>>>> Stashed changes
            return -1;
            break;
        }
    }

	
	std::string myIP;
	short port;
<<<<<<< Updated upstream
=======
	bool geneTrans = true;
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream

	std::cout << " -> " << myIP << ":" << port << std::endl;
=======
	if (arg_autogen != NULL)
	{
		geneTrans = false;
	}

	std::cout << " -> " << myIP << ":" << port << ", autoGeneTrans : " << geneTrans << std::endl;
>>>>>>> Stashed changes
	
	//create network service
    boost::asio::io_context io_context;
	
	DBus::Asio::Dispatcher dispatcher{io_context};
  	DBus::default_dispatcher = &dispatcher;
  	boost::asio::signal_set sighandler{io_context, SIGINT, SIGTERM};

	DBus::Connection conn = DBus::Connection::SystemBus();
	std::cout << "Connection Dbus ok" << std::endl;

	//create b4mesh node
<<<<<<< Updated upstream
    node s(io_context, conn, port, myIP);
=======
    node s(io_context, conn, port, myIP, geneTrans);
>>>>>>> Stashed changes

	sighandler.async_wait([&io_context, &s](const boost::system::error_code&, const int&) {
		std::cout << " Signal that end the program" << std::endl;
    	io_context.stop();
		s.GenerateResults();
  	});

<<<<<<< Updated upstream
=======
	using listeners_type = decltype(b4mesh::http::add_enpoints(io_context, {}));
	listeners_type listeners;

	if (!geneTrans){
		using method = boost::beast::http::verb;
		b4mesh::http::add_enpoints(
			io_context,
			{
				{
					"0.0.0.0:4242/add_transaction",
					{ method::put, method::post},
					[&s](b4mesh::http::request_data_type && request_datas)
						-> b4mesh::http::response_data_type
					{
						std::cout << "add_transaction : [PUT, POST] received : [" << request_datas.body << "]\n";
						s.RegisterTransaction(std::string(request_datas.body));
						return {
							"application/json",
							R"({
								"operation":"add_transaction",
								"return_value": "OK"
							})"
						};
					}
				}
			}
		);
	}

>>>>>>> Stashed changes
	//launch network service
    io_context.run();

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

