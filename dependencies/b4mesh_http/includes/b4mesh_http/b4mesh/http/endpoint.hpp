#pragma once

#include <algorithm>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <functional>
#include <map>
#include <tuple>
#include <string_view>
#include <limits>

#include <b4mesh/net/uri.hpp>

namespace b4mesh
{
    namespace beast = boost::beast;   // from <boost/beast.hpp>
    using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

    struct http /*struct-as-namespace*/ {

        enum class message_type : bool
        {
            request,
            response
        };
        template <message_type message_type_arg>
        struct message_data_type {

            constexpr static auto message_type_identifier_v = message_type_arg;
            constexpr static auto is_request_v =
                (message_type_identifier_v == decltype(message_type_identifier_v)::request);
            constexpr static auto is_response_v = not is_request_v;

            using mime_type = std::conditional_t<is_request_v, boost::string_view, std::string>;
            mime_type mime_information;
            using body_type = std::string;
            body_type body;
        };
        using request_data_type = message_data_type<message_type::request>;
        using response_data_type = message_data_type<message_type::response>;

      private:
        using endpoint_argument = struct {
            using target_type = std::string;
            target_type target;
            using method_type = boost::beast::http::verb;
            std::initializer_list<method_type> methods;
            using callback_type = std::function<response_data_type(request_data_type &&)>;
            callback_type callback;
        };

        // This function produces an HTTP response for the given
        // request. The type of the response object depends on the
        // contents of the request, so the interface requires the
        // caller to pass a generic lambda for receiving the response.
        template <class Body, class Allocator, class Send, class RequestHandlerRangeType>
        static void handle_request(
            beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req,
            Send&&                                                             send,
            const RequestHandlerRangeType&                                     request_handler_callback)
        {
            // Returns a server error response
            const auto server_error = [&req](beast::string_view what) {
                beast::http::response<beast::http::string_body> res{
                    beast::http::status::internal_server_error, req.version()};
                res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(beast::http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + std::string(what) + "'";
                res.prepare_payload();
                return res;
            };

            response_data_type request_return;
            try
            {
                using message_header_type = typename std::remove_reference_t<decltype(req)>::header_type;
                auto request_mime_information =
                    static_cast<const message_header_type&>(req)[beast::http::field::content_type]; // sw

                request_return = request_handler_callback(
                    {std::move(request_mime_information), std::forward<std::string&&>(req.body())});
            }
            catch (const std::exception& error)
            {
                send(server_error(error.what()));
                return;
            }
            catch (...)
            {
                send(server_error("unknown error"));
                return;
            }

            const auto request_return_size =
                request_return.body.size(); // Cache the size since we need it after the move

            beast::http::response<beast::http::string_body> res{
                beast::http::status::ok, req.version(), std::move(request_return.body)};
            res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(beast::http::field::content_type, request_return.mime_information);
            res.content_length(request_return_size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        //------------------------------------------------------------------------------
        // Report a failure
        static void fail(beast::error_code ec, char const* what) { std::cerr << what << ": " << ec.message() << "\n"; }
        // Handles an HTTP server connection
        struct session : public std::enable_shared_from_this<session> {

            using method_to_callback_map_type = std::multimap<
                decltype(std::declval<endpoint_argument>().methods)::value_type,
                decltype(std::declval<endpoint_argument>().callback)>;
            using target_type =
                std::decay_t<std::remove_reference_t<decltype(std::declval<b4mesh::net::uri>().path())>>;
            using endpoints_collection_type = std::map<target_type, method_to_callback_map_type>;

          private:
            // This is the C++11 equivalent of a generic lambda.
            // The function object is used to send an HTTP message.
            struct send_lambda_type {
                session& self_;

                explicit send_lambda_type(session& self)
                    : self_(self)
                {}

                template <bool isRequest, class Body, class Fields>
                void operator()(beast::http::message<isRequest, Body, Fields>&& msg) const
                {
                    // The lifetime of the message has to extend
                    // for the duration of the async operation so
                    // we use a shared_ptr to manage it.
                    auto sp = std::make_shared<beast::http::message<isRequest, Body, Fields>>(std::move(msg));

                    // Store a type-erased version of the shared
                    // pointer in the class to keep it alive.
                    self_.res_ = sp;

                    beast::http::async_write(
                        self_.stream_,
                        *sp,
                        beast::bind_front_handler(&session::on_write, self_.shared_from_this(), sp->need_eof()));
                }
            };

            beast::tcp_stream                              stream_;
            beast::flat_buffer                             buffer_;
            beast::http::request<beast::http::string_body> req_;
            std::shared_ptr<void>                          res_;
            send_lambda_type                               send_lambda;

            const endpoints_collection_type& endpoint_methods_router;

          public:
            // Take ownership of the stream
            session(tcp::socket&& socket, const endpoints_collection_type& endpoint_methods_router_argument)
                : stream_(std::move(socket))
                , send_lambda(*this)
                , endpoint_methods_router{endpoint_methods_router_argument}
            {}

            // Start the asynchronous operation
            void run() { do_read(); }

            void do_read()
            {
                // Make the request empty before reading,
                // otherwise the operation behavior is undefined.
                req_ = {};

                // Set the timeout.
                stream_.expires_after(std::chrono::seconds(30));

                // Read a request
                beast::http::async_read(
                    stream_, buffer_, req_, beast::bind_front_handler(&session::on_read, shared_from_this()));
            }

            void on_read(beast::error_code ec, std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);

                // This means they closed the connection
                if (ec == beast::http::error::end_of_stream)
                    return do_close();
                if (ec)
                    return fail(ec, "read");

                // std::cout << "Receive request : " << req_.method() << " on " << req_.target() << '\n';

                // Returns a bad request response
                auto const bad_request = [req = req_](beast::string_view why) {
                    beast::http::response<beast::http::string_body> res{
                        beast::http::status::bad_request, req.version()};
                    res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = std::string(why);
                    res.prepare_payload();
                    return res;
                };
                // Returns a not found response
                auto const not_found = [req = req_](beast::string_view target) {
                    beast::http::response<beast::http::string_body> res{beast::http::status::not_found, req.version()};
                    res.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(beast::http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + std::string(target) + "' was not found.";
                    res.prepare_payload();
                    return res;
                };

                const auto requet_handlers_it = endpoint_methods_router.find(
                    std::string{req_.target()}); // boost (previous releases) issue : no known conversion from
                                                 // boost::string_view to std::string
                if (requet_handlers_it == std::cend(endpoint_methods_router))
                    return send_lambda(not_found(req_.target()));

                const auto& [target, request_handlers_map] = *requet_handlers_it;
                const auto request_handlers_range = request_handlers_map.equal_range(req_.method());
                const auto [request_handlers_range_begin, request_handlers_range_end] = request_handlers_range;

                if (request_handlers_range_begin == request_handlers_range_end)
                    return send_lambda(bad_request("Unknown HTTP-method"));

                for (auto range_it = request_handlers_range_begin; range_it != request_handlers_range_end; ++range_it)
                {
                    const auto& [request_method, request_handler_callback] = *range_it;
                    handle_request(std::move(req_), send_lambda, request_handler_callback);
                }
            }

            void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);

                if (ec)
                    return fail(ec, "write");

                if (close)
                {
                    // This means we should close the connection, usually because
                    // the response indicated the "Connection: close" semantic.
                    return do_close();
                }

                // We're done with the response so delete it
                res_ = nullptr;

                // Read another request
                do_read();
            }

            void do_close()
            {
                // Send a TCP shutdown
                beast::error_code ec;
                stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

                // At this point the connection is closed gracefully
            }
        };
        //------------------------------------------------------------------------------
        // Accepts incoming connections and launches the sessions
        class listener : public std::enable_shared_from_this<listener> {
            boost::asio::io_context&           ioc_;
            tcp::acceptor                      acceptor_;
            session::endpoints_collection_type endpoints_collection;

          public:
            using endpoint_descriptor_type_element = std::tuple<
                decltype(std::declval<endpoint_argument>().target),
                decltype(std::declval<endpoint_argument>().methods),
                decltype(std::declval<endpoint_argument>().callback)>;
            using endpoint_descriptor_type = std::vector<endpoint_descriptor_type_element>;
            // todo : replace endpoint_descriptor_type by range of { method -> callback }

            listener(
                boost::asio::io_context& ioc, tcp::endpoint endpoint, endpoint_descriptor_type&& endpoint_descriptor)
                : ioc_(ioc)
                , acceptor_(boost::asio::make_strand(ioc))
                , endpoints_collection([&endpoint_descriptor] {
                    session::endpoints_collection_type value;

                    for (auto&& element : endpoint_descriptor)
                    {
                        const auto& [target, methods, callback] = element; // todo : move

                        const auto is_invalid_valid_target = [](const auto& target) {
                            return target.empty() or target[0] != '/' or target.find("..") != std::string::npos;
                        }(target);
                        if (is_invalid_valid_target)
                            throw std::runtime_error{std::string{"invalid target : ["} + target + "]"};

                        for (auto&& method : methods)
                        {
                            value[target].insert({std::forward<decltype(method)>(method), callback});
                        }
                    }
                    return value;
                }())
            {
                beast::error_code ec;

                // Open the acceptor
                acceptor_.open(endpoint.protocol(), ec);
                if (ec)
                {
                    fail(ec, "open");
                    return;
                }

                // Allow address reuse
                acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
                if (ec)
                {
                    fail(ec, "set_option");
                    return;
                }

                // Bind to the server address
                acceptor_.bind(endpoint, ec);
                if (ec)
                {
                    fail(ec, "bind");
                    return;
                }

                // Start listening for connections
                acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
                if (ec)
                {
                    fail(ec, "listen");
                    return;
                }
            }

            // Start accepting incoming connections
            void run() { do_accept(); }

          private:
            void do_accept()
            {
                // The new connection gets its own strand
                acceptor_.async_accept(
                    boost::asio::make_strand(ioc_),
                    beast::bind_front_handler(&listener::on_accept, shared_from_this()));
            }
            void on_accept(beast::error_code ec, tcp::socket socket)
            {
                if (ec)
                {
                    fail(ec, "accept");
                }
                else
                {
                    std::make_shared<session>(std::move(socket), endpoints_collection)->run();
                }

                // Accept another connection
                do_accept();
            }
        };
        //------------------------------------------------------------------------------

      public:
        static auto add_enpoints(boost::asio::io_context& ioc, std::initializer_list<endpoint_argument> arguments)
        {
            // remove [ip,port] collision -> sockets are on [ip,port]
            using listener_arguments_deduplicator_type = std::map<std::string, listener::endpoint_descriptor_type>;
            listener_arguments_deduplicator_type listener_arguments_deduplicator;
            for (auto&& [uri, methods, callback] : arguments)
            {
                auto [protocol, host, ip, port, path, query] = b4mesh::net::uri{std::string{uri}}.unwrap();

                if (ip.empty())
                    throw std::invalid_argument{"missing argument : ip"};
                if (port.empty())
                    throw std::invalid_argument{"missing argument : port"};
                if (not methods.size())
                    throw std::invalid_argument{"missing argument : method(s)"};

                listener_arguments_deduplicator[std::move(host)].push_back(
                    {std::move(path), std::move(methods), std::move(callback)}); // todo : forward
            };
            // create endpoints
            using endpoint_type = std::shared_ptr<listener>;
            std::vector<std::shared_ptr<listener>> listeners;
            for (auto&& [listener_ip, listener_argument] : listener_arguments_deduplicator)
            {
                auto [protocol, host, ip, port, path, query] = b4mesh::net::uri{std::string{listener_ip}}.unwrap();

                std::cout << "[+] Creating endpoint on adress=[" << ip << ':' << port << "]\n";
                for (const auto& [path, methods, callback] : listener_argument)
                {
                    std::cout << "\t`- [" << path << "] => [";
                    std::copy(
                        std::cbegin(methods),
                        std::cend(methods),
                        std::ostream_iterator<decltype(methods)::value_type>(std::cout, ","));
                    std::cout << "\b\b]\n";
                }

                const auto listener_port = [&port = port] {
                    const auto value = std::stoi(port);
                    using boost_port_type = decltype(std::declval<tcp::endpoint>().port());
                    if (value > std::numeric_limits<boost_port_type>::max())
                        throw std::invalid_argument{"invalid port"};
                    return static_cast<boost_port_type>(value);
                }();

                auto listener_value = std::make_shared<listener>(
                    ioc,
                    tcp::endpoint{boost::asio::ip::make_address(ip), listener_port},
                    std::move(listener_arguments_deduplicator[std::move(host)]));
                listeners.push_back(std::move(listener_value));
            }
            for (auto& listener : listeners)
                listener->run();
            return listeners;
        }
    };
}