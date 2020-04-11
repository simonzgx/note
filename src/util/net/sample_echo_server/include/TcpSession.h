//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_TCP_SESSION_H
#define SAMPLE_ECHO_SERVER_TCP_SESSION_H

#include <cstring>
#include "TcpConnection.h"
#include "Dispatcher.h"

namespace SZ {
    class TcpConnection;

    class TcpSession {
        friend class TcpConnection;

    public:
        TcpSession(int fd, const char *ip, int port, SZ::TcpConnection *pServer);

        virtual ~TcpSession();

        virtual void OnRecv(const char *buf, int len) {};

        virtual void OnClose() {};

        virtual void Send(const void *data, unsigned int dataLen);

        virtual void Close();

    protected:
        int _fd;
        TcpConnection *_pServer;
        int _remote_port;
        std::string _remote_ip;
        char _dataToSend[1024];
        std::mutex _lockSend;

        void OnSend(int fd);
    };
}


#endif //SAMPLE_ECHO_SERVER_TCP_SESSION_H
