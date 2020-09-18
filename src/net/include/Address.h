//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_ADDRESS_H
#define NET_ADDRESS_H


#include <cstdint>
#include <string>
#include <cstring>

#include "SocketOption.h"

#ifdef linux

#include <netdb.h>

#elif defined(__WINDOWS__)
#include <inaddr.h>
// INADDR_ANY use (type)value casting.
static in_addr kInaddrAny;
#endif

namespace net {
    class Address {
    public:
        using string = std::string;

        /// Constructs an endpoint with given port number.
        /// Mostly used in TcpServer listening.
        explicit Address(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

        /// Constructs an endpoint with given ip and port.
        /// @c ip should be "1.2.3.4"
        Address(const string &ip, uint16_t port, bool ipv6 = false);

        /// Constructs an endpoint with given struct @c sockaddr_in
        /// Mostly used when accepting new connections
        explicit Address(const struct sockaddr_in &addr)
                : addr_(addr) {}

        explicit Address(const struct sockaddr_in6 &addr)
                : addr6_(addr) {}

        sa_family_t family() const { return addr_.sin_family; }

        string toIp() const;

        string toIpPort() const;

        uint16_t toPort() const;

        // default copy/assignment are Okay

        const struct sockaddr *getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }

        void setSockAddrInet6(const struct sockaddr_in6 &addr6) { addr6_ = addr6; }

        uint32_t ipNetEndian() const;

        uint16_t portNetEndian() const { return addr_.sin_port; }

        // resolve hostname to IP address, not changing port or sin_family
        // return true on success.
        // thread safe
        static bool resolve(const string &hostname, Address *result);
        // static std::vector<Address> resolveAll(const char* hostname, uint16_t port = 0);

        // set IPv6 ScopeID
        void setScopeId(uint32_t scope_id);

    private:
        union {
            struct sockaddr_in addr_;
            struct sockaddr_in6 addr6_;
        };
    };
}


#endif //NET_ADDRESS_H
