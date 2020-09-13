//
// Created by Administrator on 2020/8/20.
//

#include <cstring>
#include <cstdio>

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#elif defined(linux)
#include <netinet/tcp.h>
#endif

#include "Address.h"
#include "Socket.h"
#include "logger.h"
#include "SocketOption.h"


using namespace net;


namespace {
    class WINSock {
    public:
        explicit WINSock() {
            WSADATA wsaData = {0};
            if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                fprintf(stderr, "WSAStartup failed! return code:%d", WSAGetLastError());
                exit(EXIT_FAILURE);
            }
        }

        ~WINSock() {
            ::WSACleanup();
        }
    };
}

static ::WINSock winSock;

Socket::~Socket() {
    sockets::close(sockfd_);
}

bool Socket::getTcpInfoString(char *buf, int len) const {
#ifdef WIN32
    return true;
#elif defined(linux)
    struct tcp_info tcpi{};
    bool ok = getTcpInfo(&tcpi);
    if (ok) {
        snprintf(buf, len, "unrecovered=%u "
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                 tcpi.tcpi_rto,          // Retransmit timeout in usec
                 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                 tcpi.tcpi_snd_mss,
                 tcpi.tcpi_rcv_mss,
                 tcpi.tcpi_lost,         // Lost packets
                 tcpi.tcpi_retrans,      // Retransmitted packets out
                 tcpi.tcpi_rtt,          // Smoothed round trip time in usec
                 tcpi.tcpi_rttvar,       // Medium deviation
                 tcpi.tcpi_snd_ssthresh,
                 tcpi.tcpi_snd_cwnd,
                 tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
#endif
}

bool Socket::getTcpInfo(struct tcp_info *tcpi) const {
#ifdef WIN32
    return true;
#elif defined(linux)
    socklen_t len = sizeof(*tcpi);
    memset(tcpi, '\0', len);
    return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
#endif
}

void Socket::bindAddress(const Address &addr) {
    sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen() {
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(Address *peeraddr) {
    struct sockaddr_in6 addr{};
//    memset(&addr, '\0', sizeof addr);
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0) {
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                 reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
#ifdef linux
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
#elif defined(WIN32)
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
#endif
}

void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
#ifdef linux
        int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                           &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
        LOG_SYSERR("%s", "SO_REUSEPORT failed.");
    }
#endif
#else
    if (on) {
        LOG_ERROR("%s", "SO_REUSEPORT is not supported.");
    }
#endif

}

void Socket::setKeepAlive(bool on) const {
    int optval = on ? 1 : 0;
#ifdef linux
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval))) {
        perror("ERROR: setsocketopt(), SO_KEEPIDLE");
    }
#elif defined(WIN32)
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval))) {
        perror("ERROR: setsocketopt(), SO_KEEPIDLE");
    }
#endif

    optval = 3;
    if (::setsockopt(sockfd_, SOL_TCP, TCP_KEEPIDLE, &optval, static_cast<socklen_t>(sizeof optval))) {
        perror("ERROR: setsocketopt(), TCP_KEEPIDLE");
    }

}
