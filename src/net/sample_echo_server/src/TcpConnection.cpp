//
// Created by simon on 2020/3/14.
//

#include "../include/TcpConnection.h"

SZ::TcpConnection::TcpConnection(SZ::Dispatcher *dispathcer) : _p(dispathcer) {

}

SZ::TcpConnection::~TcpConnection() = default;

void SZ::TcpConnection::OnRead(int fd) {
    char buf[10240];
    auto it = _clients.find(fd);
    if (it == _clients.end()) return;
    int s = recv(fd, buf, sizeof(buf) - 1, 0);
    if (s > 0) {
        buf[s] = 0;
        {
            it->second->OnRecv(buf, s);
        }
    } else if (s == 0) {
        printf(" client quit...\n ");
        // DeleteEvent(fd);
        // closesocket(fd);
        {
            it->second->OnClose();
            delete it->second;
            it = _clients.erase(it);
        }
    } else {
        printf("read fai ...\n");
        // DeleteEvent(fd);
        // closesocket(fd);

        {
            it->second->OnClose();
            delete it->second;
            it = _clients.erase(it);
        }
    }
}

void SZ::TcpConnection::OnRecv(const char *buf, int len) {

}

void SZ::TcpConnection::OnClose() {

}

void SZ::TcpConnection::RemoveClient(int fd) {
    auto it = _clients.find(fd);
    if (it != _clients.end()) {
        _clients.erase(it);
    }
}

void SZ::TcpConnection::OnWrite(int fd) {
    auto it = _clients.find(fd);
    if (it != _clients.end()) {
        it->second->OnSend(fd);
    }
}

void SZ::TcpConnection::OnError(int fd) {
    auto it = _clients.find(fd);
    if (it != _clients.end()) {
        it->second->OnClose();
        delete it->second;
        it = _clients.erase(it);
    }
}

void SZ::TcpConnection::OnAccept() {
    sockaddr_in client;

    socklen_t addr_len = sizeof(sockaddr_in);


    int new_sock = accept(_sock, (sockaddr *) &client, &addr_len);
    if (new_sock < 0) {
        DEBUG_LOG_INFO__("accept fail ... \n");
        return;
    }

    SetNonblock(new_sock);

    DEBUG_LOG_INFO__("A new client connected. [%s:%d] \n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    CreateEvent(new_sock, SOCKET_READ | SOCKET_EXCEP);
    TcpSession *ct = Accept(new_sock, inet_ntoa(client.sin_addr), ntohs(client.sin_port), this);
    if (ct != nullptr) {
        _clients.insert(std::pair<int, TcpSession *>(new_sock, ct));
    }
}

void SZ::TcpConnection::SetNonblock(int fd) {
    //设置fd为非阻塞
    int ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
    if (ret < 0) {
        DEBUG_LOG_INFO__("_SetNonblock failed, ret_code=%d", ret);
    }
}

void SZ::TcpConnection::CreateEvent(int fd, uint8_t socket_event) {
    _p->CreateEvent(fd, socket_event);
}

