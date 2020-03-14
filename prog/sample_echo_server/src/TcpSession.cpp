//
// Created by simon on 2020/3/14.
//

#include "../include/TcpSession.h"

SZ::TcpSession::~TcpSession() = default;

SZ::TcpSession::TcpSession(int fd, const char *remote_ip, int remote_port, TcpConnection *pServer) : _dataToSend{0} {
    _fd = fd;
    _pServer = pServer;
    _remote_port = remote_port;
    _remote_ip = remote_ip;
}

void SZ::TcpSession::Send(const void *data, unsigned int dataLen) {
    send(_fd, (const char *) data, dataLen, 0);
}

void SZ::TcpSession::Close() {
    OnClose();
    _pServer->RemoveClient(_fd);
    delete this;
}

void SZ::TcpSession::OnSend(int fd) {
    int n = send(fd, _dataToSend, strlen(_dataToSend), 0);
}