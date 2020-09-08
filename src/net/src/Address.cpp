//
// Created by Administrator on 2020/8/20.
//

#include "Address.h"
#include "End.h"

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

using namespace net;
using string = std::string;


static_assert(sizeof(Address) == sizeof(struct sockaddr_in6),
              "Address is same size as sockaddr_in6");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");

Address::Address(uint16_t port, bool loopbackOnly, bool ipv6) {
    static_assert(offsetof(Address, addr6_) == 0, "addr6_ offset 0");
    static_assert(offsetof(Address, addr_) == 0, "addr_ offset 0");
    if (ipv6) {
        memset(&addr6_, '\0', sizeof addr6_);
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetwork16(port);
    } else {
        memset(&addr_, '\0', sizeof addr_);
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }
}

Address::Address(const string &ip, uint16_t port, bool ipv6) {
    if (ipv6) {
        sockets::fromIpPort(ip.c_str(), port, &addr6_);
    } else {
        sockets::fromIpPort(ip.c_str(), port, &addr_);
    }
}

string Address::toIpPort() const {
    char buf[64] = "";
    sockets::toIpPort(buf, sizeof buf, getSockAddr());
    return buf;
}

string Address::toIp() const {
    char buf[64] = "";
    sockets::toIp(buf, sizeof buf, getSockAddr());
    return buf;
}

uint32_t Address::ipNetEndian() const {
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr;
}

uint16_t Address::toPort() const {
    return sockets::networkToHost16(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool Address::resolve(const string &hostname, Address *out) {
    assert(out != nullptr);
    struct hostent hent{};
    struct hostent *he = nullptr;
    int herrno = 0;

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != nullptr) {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->addr_.sin_addr = *reinterpret_cast<struct in_addr *>(he->h_addr);
        return true;
    } else {
        if (ret) {
            LOG_SYSERR("%s", "Address::resolve");
        }
        return false;
    }
}

void Address::setScopeId(uint32_t scope_id) {
    if (family() == AF_INET6) {
        addr6_.sin6_scope_id = scope_id;
    }
}