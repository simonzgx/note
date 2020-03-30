//
// Created by simon on 2020/3/14.
//
#include <iostream>
#include <utility>
#include "../include/EchoServer.h"

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
        DEBUG_LOG_INFO__("socket fail...\n");
        return -2;
    }

    //测试发现server重启会报端口冲突，需要增加reuse port参数
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt))) {
        DEBUG_LOG_INFO__("set socket option failed");
        return -3;
    }

    struct sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);// 地址为任意类型
    local.sin_port = htons(port);
    // 3. 绑定端口号

    if (bind(sock, (struct sockaddr *) &local, sizeof(local)) < 0) {
        DEBUG_LOG_INFO__("bind fail...\n");
        return -4;
    }

    // 4. 获得监听套接字
    if (listen(sock, 5) < 0) {
        DEBUG_LOG_INFO__("listen fail...\n");
        return -5;
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
