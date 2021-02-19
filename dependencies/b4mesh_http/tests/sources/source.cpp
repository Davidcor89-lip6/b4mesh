#include <boost/asio/bind_executor.hpp>
#include <boost/asio/io_service.hpp>

#include <b4mesh/http/endpoint.hpp>

#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

void test(std::chrono::seconds run_duration, int threads_count)
{
    boost::asio::io_context io_service{threads_count};

    // b4mesh payload
    using method = boost::beast::http::verb;
    auto listeners = b4mesh::http::add_enpoints(
        // clang-format off
        io_service,
        {
            {
                "0.0.0.0:4242/benchmark",
                { method::put, method::post},
                [](b4mesh::http::request_data_type && request_datas)
                    -> b4mesh::http::response_data_type
                {
                    return {};
                }
            },
            {
                "0.0.0.0:4242/error",
                { method::get },
                [](b4mesh::http::request_data_type && request_datas)
                    -> b4mesh::http::response_data_type
                {
                    std::cout << "error : [GET] received : [" << request_datas.body << "]\n";
                    throw std::runtime_error{"test error\n"};
                    return{};
                }
            },
            {
                "0.0.0.0:4242/add_transaction",
                { method::put, method::post},
                [](b4mesh::http::request_data_type && request_datas)
                    -> b4mesh::http::response_data_type
                {
                    std::cout << "add_transaction : [PUT, POST] received : [" << request_datas.body << "]\n";
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
        // clang-format on
    );
    // b4mesh payload

    auto running_threads = [&io_service, &threads_count] {
        auto value = std::vector<std::thread>(threads_count);
        std::generate(std::begin(value), std::end(value), [&io_service]() {
            return std::thread{[&io_service] { io_service.run(); }};
        });
        return value;
    }();

    std::cout << "Running ...\n";
    std::this_thread::sleep_for(run_duration);
    io_service.stop();
    for (auto& thread : running_threads)
    {
        thread.join();
    }
}

auto main(int ac, char *av[]) -> int
{
    try
    {
        std::cout << "b4mesh::http::binary :\n"
                  << "usage : <bin> [run_duration=120s] [threads_qty=1]";
        test(
            std::chrono::seconds(ac >= 2 ? std::atoi(av[1]) : 120),
            ac == 3 ? std::atoi(av[2]) : 1
        );
    }
	catch (const std::exception& error)
	{
		std::cerr << "error : " << error.what() << '\n';
        throw; // to invalidate test
	}
	catch (...)
	{
		std::cerr << "unhandled error catched\n";
        // to invalidate test
	}
}