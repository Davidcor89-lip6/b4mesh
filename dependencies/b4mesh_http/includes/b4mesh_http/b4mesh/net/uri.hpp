#pragma once

#include <cctype>
#include <string>
#include <algorithm>
#include <tuple>

namespace b4mesh::net
{
    // alernative to boost::network::uri which still is a proposal
    // POC : https://godbolt.org/z/TWP5hP
    struct uri {
        uri(std::string&& argument) { parse(std::forward<decltype(argument)>(argument)); }
        uri(const std::string& argument) { parse(std::string{argument}); }

        const std::string& protocol() const { return protocol_; }
        const std::string& host() const { return host_; }
        const std::string& ip() const { return ip_; }
        const std::string& port() const { return port_; }
        const std::string& path() const { return path_; }
        const std::string& query() const { return query_; }

        using structure_binding_accessor_type = std::tuple<
        // provides accessors for unwrap on const-lvalue-reference
            const std::string&,
            const std::string&,
            const std::string&,
            const std::string&,
            const std::string&,
            const std::string&>;
        operator structure_binding_accessor_type() const { return {protocol_, host_, ip_, port_, path_, query_}; }
        const auto unwrap() const & { return static_cast<const structure_binding_accessor_type>(*this); }

        using structure_binding__rvalue_accessor_type = std::tuple<
        // provides storage for unwrap on rvalue
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string>;
        operator structure_binding__rvalue_accessor_type() && { return {
                std::forward<decltype(protocol_)>(protocol_),
                std::forward<decltype(host_)>(host_),
                std::forward<decltype(ip_)>(ip_),
                std::forward<decltype(port_)>(port_),
                std::forward<decltype(path_)>(path_),
                std::forward<decltype(query_)>(query_)
        };
        }
        auto unwrap() && { return static_cast<structure_binding__rvalue_accessor_type&&>(std::move(*this)); }

      private:
        std::string protocol_, host_, ip_, port_, path_, query_;
        void        parse(std::string&& argument)
        {   // todo : move_iterator-s, IIFE
            const std::string     prot_end("://");
            std::string::iterator prot_i =
                std::search(argument.begin(), argument.end(), prot_end.begin(), prot_end.end());

            if (prot_i != argument.end())
            {
                protocol_.reserve(distance(argument.begin(), prot_i));
                std::transform(argument.begin(), prot_i, back_inserter(protocol_), [](unsigned char c) {
                    return std::tolower(c);
                }); // protocol is icase
                std::advance(prot_i, prot_end.length());
            }
            else
                prot_i = std::begin(argument);

            std::string::iterator path_i = find(prot_i, argument.end(), '/');
            host_.reserve(distance(prot_i, path_i));
            std::transform(
                prot_i, path_i, back_inserter(host_), [](unsigned char c) { return std::tolower(c); }); // host is icase
            std::string::iterator query_i = find(path_i, argument.end(), '?');
            path_.assign(path_i, query_i);
            if (query_i != argument.end())
                ++query_i;
            query_.assign(query_i, argument.end());

            const auto port_delimiter = host_.find(':');
            ip_ = host_.substr(0, port_delimiter);
            if (port_delimiter != std::string::npos)
                port_ = host_.substr(ip_.length() + 1, host_.length() - ip_.length());
        }
    };
}