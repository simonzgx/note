//
// Created by Administrator on 2020/8/19.
//

#include "SocketOption.h"
#include "End.h"

#ifdef __WINDOWS__
#include <ws2tcpip.h>
#include <Winsock.h>
#include <winsock2.h>
#elif defined(linux)
#include <sys/uio.h>  // readv
#include <unistd.h>
#endif

using namespace net;
using namespace net::sockets;


namespace {

    typedef struct sockaddr SA;
#ifdef linux
#if VALGRIND || defined (NO_ACCEPT4)
    void setNonBlockAndCloseOnExec(int sockfd){
      // non-block
      int flags = ::fcntl(sockfd, F_GETFL, 0);
      flags |= O_NONBLOCK;
      int ret = ::fcntl(sockfd, F_SETFL, flags);
      // FIXME check

      // close-on-exec
      flags = ::fcntl(sockfd, F_GETFD, 0);
      flags |= FD_CLOEXEC;
      ret = ::fcntl(sockfd, F_SETFD, flags);
      // FIXME check

      (void)ret;
    }
#endif
#elif defined(__WINDOWS__)

    void setNonBlockAndCloseOnExec(int sockfd) {
        unsigned long on_windows = 1;
        if (ioctlsocket(sockfd, FIONBIO, &on_windows) < 0) {
            LOG_SYSFATAL("%s", "set socket nonblocking failed!");
            return;
        }
        closesocket(sockfd);
    }

#endif

}  // namespace


const struct sockaddr *sockets::sockaddr_cast(const struct sockaddr_in6 *addr) {
    return reinterpret_cast<const struct sockaddr *>(addr);
}

struct sockaddr *sockets::sockaddr_cast(struct sockaddr_in6 *addr) {
    return reinterpret_cast<struct sockaddr *>(addr);
}

const struct sockaddr *sockets::sockaddr_cast(const struct sockaddr_in *addr) {
    return reinterpret_cast<const struct sockaddr *>(addr);
}

const struct sockaddr_in *sockets::sockaddr_in_cast(const struct sockaddr *addr) {
    return reinterpret_cast<const struct sockaddr_in *>(addr);
}

const struct sockaddr_in6 *sockets::sockaddr_in6_cast(const struct sockaddr *addr) {
    return reinterpret_cast<const struct sockaddr_in6 *>(addr);
}

int sockets::createNonblockingOrDie(sa_family_t family) {
#ifdef linux
#if VALGRIND
    int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0)
  {
    LOG_SYSFATAL("%s", "sockets::createNonblockingOrDie");
  }

  setNonBlockAndCloseOnExec(sockfd);
#else
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_SYSFATAL("%s", "sockets::createNonblockingOrDie");
    }
#endif
#else
    int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_SYSFATAL("%s", "sockets::createNonblockingOrDie");
    }
    setNonBlockAndCloseOnExec(sockfd);
#endif
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr *addr) {
    int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if (ret < 0) {
        LOG_SYSFATAL("%s", "sockets::bindOrDie");
    } else {
        LOG_INFO("%s", "bindOrDie success!");
    }
}

void sockets::listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG_SYSFATAL("%s", "sockets::listenOrDie");
    } else {
        LOG_INFO("%s", "listen success!");
    }
}

int sockets::accept(int sockfd, struct sockaddr_in6 *addr) {
    auto addrlen = static_cast<socklen_t>(sizeof *addr);
#ifdef linux
#if VALGRIND || defined (NO_ACCEPT4)
    int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
#else
    int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                           &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
    if (connfd < 0) {
        int savedErrno = errno;
        switch (savedErrno) {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                LOG_SYSFATAL("%s %d", "unexpected error of ::accept ", savedErrno);
                break;
            default:
                LOG_SYSFATAL("%s %d", "unknown error of ::accept ", savedErrno);
                break;
        }
    }
    return connfd;
#else
    int connfd = ::accept(sockfd, nullptr, nullptr);
    if (connfd == INVALID_SOCKET) {
        LOG_SYSFATAL("%s%d", "accept error! retcode:", connfd);
        closesocket(connfd);
        return -1;
    }
    return connfd;
#endif
}

int sockets::connect(int sockfd, const struct sockaddr *addr) {
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

ssize_t sockets::read(int sockfd, void *buf, size_t count) {
#ifdef linux
    return ::read(sockfd, buf, count);
#elif defined(__WINDOWS__)

#endif
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt) {
#ifdef linux
    return ::readv(sockfd, iov, iovcnt);
#elif defined(__WINDOWS__)

#endif
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
#ifdef linux
    return ::write(sockfd, buf, count);
#elif defined(__WINDOWS__)

#endif
}

void sockets::close(int sockfd) {
#ifdef linux
    if (::close(sockfd) < 0) {
        LOG_SYSERR("%s", "sockets::close");
    }
#else
    if (::closesocket(sockfd) < 0) {
        LOG_SYSERR("%s", "sockets::close");
    }
#endif
}

void sockets::shutdownWrite(int sockfd) {
#ifdef linux
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOG_SYSERR("%s", "sockets::shutdownWrite");
    }
#elif defined(__WINDOWS__)
    if (::shutdown(sockfd, SD_SEND) != 0) {
        LOG_SYSERR("%s%D", "sockets::shutdownWrite error. error code :", WSAGetLastError());
    }
#endif
}

void sockets::toIpPort(char *buf, size_t size,
                       const struct sockaddr *addr) {
    toIp(buf, size, addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in *addr4 = sockaddr_in_cast(addr);
    uint16_t port = sockets::networkToHost16(addr4->sin_port);
    assert(size > end);
    snprintf(buf + end, size - end, ":%u", port);
}

void sockets::toIp(char *buf, size_t size,
                   const struct sockaddr *addr) {
#ifdef linux
    if (addr->sa_family == AF_INET) {
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in *addr4 = sockaddr_in_cast(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    } else if (addr->sa_family == AF_INET6) {
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6 *addr6 = sockaddr_in6_cast(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
#elif defined(__WINDOWS__)

#endif
}

void sockets::fromIpPort(const char *ip, uint16_t port,
                         struct sockaddr_in *addr) {
#ifdef linux
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
        LOG_SYSERR("%s", "sockets::fromIpPort");
    }
#elif defined(__WINDOWS__)

#endif
}

void sockets::fromIpPort(const char *ip, uint16_t port,
                         struct sockaddr_in6 *addr) {
#ifdef linux
    addr->sin6_family = AF_INET6;
    addr->sin6_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0) {
        LOG_SYSERR("%s", "sockets::fromIpPort");
    }
#elif defined(__WINDOWS__)
#endif
}

int sockets::getSocketError(int sockfd) {
    int optval;
    auto optlen = static_cast<socklen_t>(sizeof optval);
#ifdef linux
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
#elif defined(__WINDOWS__)
#endif
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd) {
    struct sockaddr_in6 localaddr{};
    auto addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
        LOG_SYSERR("%s", "sockets::getLocalAddr");
    }
    return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd) {
    struct sockaddr_in6 peeraddr{};
    auto addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
        LOG_SYSERR("%s", "sockets::getPeerAddr");
    }
    return peeraddr;
}

bool sockets::isSelfConnect(int sockfd) {
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if (localaddr.sin6_family == AF_INET) {
        const struct sockaddr_in *laddr4 = reinterpret_cast<struct sockaddr_in *>(&localaddr);
        const struct sockaddr_in *raddr4 = reinterpret_cast<struct sockaddr_in *>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port
               && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    } else if (localaddr.sin6_family == AF_INET6) {
        return localaddr.sin6_port == peeraddr.sin6_port
               && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
    } else {
        return false;
    }
}


