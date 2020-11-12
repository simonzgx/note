//
// Created by Administrator on 2020/8/10.
//

#include <iostream>
#include "socket.h"

net::Socket::Socket(int af, int sock_type, int protocol) {
    this->m_sfd = socket(af, sock_type, protocol);
    if (this->m_sfd < 0) {
        char err[ERR_MSG_LEN];
        NetSetError(err, "create socket error: %s(errno: %d)\n", strerror(errno), errno);
        perror(err);
        exit(EXIT_FAILURE);
    }
}

int net::Socket::SetReuseAddr(char *err_msg) {
    int yes = 1;
    if (setsockopt(this->m_sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        NetSetError(err_msg, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

net::Socket::~Socket() {
    if (this->m_sfd != -1) {
        close(this->m_sfd);
    }
}

int net::Socket::SetBlock(char *err_msg, int non_block) {
    int flag;
    if ((flag = fcntl(this->m_sfd, F_GETFL)) == -1) {
        NetSetError(err_msg, "fcntl(F_GETFL): %s", strerror(errno));
        return NET_ERR;
    }

    if (non_block) {
        flag |= O_NONBLOCK;
    } else {
        flag &= ~O_NONBLOCK;
    }

    if (fcntl(this->m_sfd, F_SETFL, flag) == -1) {
        NetSetError(err_msg, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return NET_ERR;
    }

    return NET_OK;
}

int net::Socket::SetNonBlock(char *err_msg) {
    return this->SetBlock(err_msg, 1);
}

net::Socket *net::Socket::New(int domain, char *err_msg) {
    int sfd;
    if ((sfd = socket(domain, SOCK_STREAM, 0)) == -1) {
        NetSetError(err_msg, "creating socket: %s", strerror(errno));
        return nullptr;
    }

    auto s = new Socket(sfd);
    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (s->SetReuseAddr(err_msg) == NET_ERR) {
        close(s->m_sfd);
        return nullptr;
    }
    return s;
}

net::TCPServer::TCPServer(int port, const char *bind_addr) : m_port(port) {
    int rv;
    char _port[6];
    struct addrinfo hints{}, *serv_info;
    char err[ERR_MSG_LEN];

    snprintf(_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(bind_addr, _port, &hints, &serv_info)) != 0) {
        NetSetError(err, "%s", gai_strerror(rv));
        perror(err);
        exit(EXIT_FAILURE);
    }

    for (auto p = serv_info; p != nullptr; p = p->ai_next) {
        this->m_socket = std::make_shared<Socket>(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);

        if (this->m_socket->SetReuseAddr(err) != NET_OK) {
            continue;
        }

        if (this->Listen(serv_info, err) != NET_OK) {
            continue;
        }
        freeaddrinfo(serv_info);
        return;
    }
    perror(err);
    freeaddrinfo(serv_info);
    exit(EXIT_FAILURE);
}

int net::TCPServer::Listen(const struct addrinfo *addr, char *err_msg) {
    if (bind(this->m_socket->m_sfd, addr->ai_addr, addr->ai_addrlen) == -1) {
        NetSetError(err_msg, "bind: %s", strerror(errno));
        return NET_ERR;
    }

    if (listen(this->m_socket->m_sfd, 10) == -1) {
        NetSetError(err_msg, "listen: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

int net::TCPServer::Accept(char *err_msg) {
    int conn_fd = accept(this->m_socket->m_sfd, static_cast<struct sockaddr *>(nullptr), nullptr);
    if (conn_fd == -1) {
        NetSetError(err_msg, "accept socket error: %s(errno: %d)", strerror(errno), errno);
        return NET_ERR;
    }
    return conn_fd;
}

int net::TCPServer::SetKeepAlive(int fd, int interval, char *err_msg) {
    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1) {
        NetSetError(err_msg, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return NET_ERR;
    }

#if defined(__linux__)
    val = interval;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        NetSetError(err_msg, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return NET_ERR;
    }

    val = interval / 3;
    if (val == 0)val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        NetSetError(err_msg, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return NET_ERR;
    }

    val = 3;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        NetSetError(err_msg, "setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
        return NET_ERR;
    }
#else
    ((void) interval);
#endif

    return NET_OK;
}


int net::GenericResolve(char *err_msg, char *host, char *ipbuf, size_t ipbuf_len, int flags) {
    struct addrinfo hints{}, *info;
    int rv;

    memset(&hints, 0, sizeof(hints));
    if (flags & ANET_IP_ONLY) hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(host, nullptr, &hints, &info)) != 0) {
        NetSetError(err_msg, "%s", gai_strerror(rv));
        return NET_ERR;
    }
    if (info->ai_family == AF_INET) {
        auto *sa = (struct sockaddr_in *) info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, ipbuf_len);
    } else {
        auto *sa = (struct sockaddr_in6 *) info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, ipbuf_len);
    }

    freeaddrinfo(info);
    return NET_OK;
}

int net::Resolve(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return GenericResolve(err, host, ipbuf, ipbuf_len, ANET_NONE);
}

int net::ResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return GenericResolve(err, host, ipbuf, ipbuf_len, ANET_IP_ONLY);
}

int net::Read(int fd, char *buf, size_t count) {
    ssize_t n_read, totlen = 0;
    while (totlen != count) {
        n_read = read(fd, buf, count - totlen);
        std::cout<<"read(2) read"<<std::endl;
        if (n_read == 0) return totlen;
        if (n_read == -1) return -1;
        totlen += n_read;
        buf += n_read;
    }
    return totlen;
}

int net::Write(int fd, const char *buf, size_t count) {
    ssize_t nwritten, totlen = 0;
    while (totlen != count) {
        nwritten = write(fd, buf, count - totlen);
        if (nwritten == 0) return totlen;
        if (nwritten == -1) return -1;
        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}