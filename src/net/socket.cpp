//
// Created by Administrator on 2020/8/10.
//

#include "socket.h"

net::Socket::Socket(int af, int sock_type, int protocol) {
    this->m_fd = socket(af, sock_type, protocol);
    if (this->m_fd < 0) {
        char err[ERR_MSG_LEN];
        NetSetError(err, "create socket error: %s(errno: %d)\n", strerror(errno), errno);
        perror(err);
        exit(EXIT_FAILURE);
    }
}

int net::Socket::SetReuseAddr(char *err_msg) {
    int yes = 1;
    if (setsockopt(this->m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        NetSetError(err_msg, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

net::Socket::~Socket() {
    if (m_fd != -1) {
        close(m_fd);
    }
}

net::TCPServer::TCPServer(int port, const char *bind_addr) : m_port(port) {
    int rv;
    char _port[6];
    struct addrinfo hints{}, *serv_info;
    char err[ERR_MSG_LEN];

    snprintf(_port,6,"%d",port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(bind_addr, _port, &hints, &serv_info)) != 0) {
        NetSetError(err, "%s", gai_strerror(rv));
        perror(err);
        exit(EXIT_FAILURE);
    }

    this->m_socket = std::make_shared<Socket>(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
    do {
        if (this->m_socket->SetReuseAddr(err) != NET_OK) {
            break;
        }

        if (this->Listen(serv_info, err) != NET_OK) {
            break;
        }
        freeaddrinfo(serv_info);
        return;
    } while (false);

    perror(err);
    freeaddrinfo(serv_info);
    exit(EXIT_FAILURE);
}

int net::TCPServer::Listen(const struct addrinfo *addr, char *err_msg) {
    if (bind(this->m_socket->m_fd, addr->ai_addr, addr->ai_addrlen) == -1) {
        NetSetError(err_msg, "bind: %s", strerror(errno));
        return NET_ERR;
    }

    if (listen(this->m_socket->m_fd, 10) == -1) {
        NetSetError(err_msg, "listen: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

int net::TCPServer::Accept(char *err_msg) {
    int conn_fd = accept(this->m_socket->m_fd, static_cast<struct sockaddr *>(nullptr), nullptr);
    if (conn_fd == -1) {
        NetSetError(err_msg, "accept socket error: %s(errno: %d)", strerror(errno), errno);
        return NET_ERR;
    }
    return conn_fd;
}
