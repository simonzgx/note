//
// Created by simon on 2020/3/14.
//
#include <iostream>
#include <utility>
#include "EchoServer.h"

#define NETLOGINFO__ std::cout<<

SZ::EchoServer::EchoServer(SZ::Dispatcher *p) : TcpConnection(p) {
    p->AddServer(this);
}

SZ::EchoServer::~EchoServer() {
    for (auto it : _clients) {
        delete it.second;
    }
}

SZ::TcpSession *SZ::EchoServer::Accept(int fd, const char *remote_ip, int remote_port, SZ::TcpConnection *pServer) {
    if (_onAccept != nullptr) {
        return _onAccept(fd, remote_ip, remote_port, pServer);
    }
    return nullptr;
}

int SZ::EchoServer::startup(int port) {
// 1. 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        NETLOGINFO__ ("socket fail...\n");
        return -2;
    }

    struct sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);// 地址为任意类型
    local.sin_port = htons(port);
    // 3. 绑定端口号

    if (bind(sock, (struct sockaddr *) &local, sizeof(local)) < 0) {
        NETLOGINFO__ ("bind fail...\n");
        return -3;
    }

    // 4. 获得监听套接字
    if (listen(sock, 5) < 0) {
        NETLOGINFO__ ("listen fail...\n");
        return -4;
    }
    return sock;
}

bool SZ::EchoServer::start(const char *name, int port, SZ::TcpServerAcceptLisitener cb) {
    _onAccept = std::move(cb);
    _port = port;
    _name = name;

    // 获得监听套接字
    _sock = startup(_port);
    if (_sock < 0) {
        return false;
    }

    CreateEvent(_sock, SOCKET_READ | SOCKET_EXCEP);

    return true;
}
