//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include "noncopyable.h"
#include "platform.h"

struct tcp_info;

namespace net {

    class Address;

    class Socket {
    public:
        explicit Socket(int sockfd)
                : sockfd_(sockfd) {}

        // Socket(Socket&&) // move constructor in C++11
        ~Socket();

        int fd() const { return sockfd_; }

        // return true if success.
        bool getTcpInfo(struct tcp_info *tcpi) const;

        bool getTcpInfoString(char *buf, int len) const;

        /// abort if address in use
        void bindAddress(const Address &localaddr);

        /// abort if address in use
        void listen();

        /// On success, returns a non-negative integer that is
        /// a descriptor for the accepted socket, which has been
        /// set to non-blocking and close-on-exec. *peeraddr is assigned.
        /// On error, -1 is returned, and *peeraddr is untouched.
        int accept(Address *peeraddr);

        void shutdownWrite();

        ///
        /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
        ///
        void setTcpNoDelay(bool on);

        ///
        /// Enable/disable SO_REUSEADDR
        ///
        void setReuseAddr(bool on);

        ///
        /// Enable/disable SO_REUSEPORT
        ///
        void setReusePort(bool on);

        ///
        /// Enable/disable SO_KEEPALIVE
        ///
        void setKeepAlive(bool on) const;

    private:
        const int sockfd_;
    };
}


#endif //NET_SOCKET_H
