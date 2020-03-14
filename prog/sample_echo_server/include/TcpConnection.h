//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_TCP_CONNECTION_H
#define SAMPLE_ECHO_SERVER_TCP_CONNECTION_H

#include <map>
#include <mutex>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include "Dispatcher.h"
#include "TcpSession.h"


namespace SZ {

    class Dispatcher;

    class TcpSession;

    class TcpConnection {
        friend class Epoller;

        friend class TcpSession;

        friend class Poller;

        friend class Dispatcher;

    public:
        explicit TcpConnection(Dispatcher *dispathcer);

        virtual ~TcpConnection();

    protected:
        void OnAccept();

        void OnRead(int fd);

        virtual void OnClose();

        virtual void OnRecv(const char *buf, int len);

        void OnWrite(int fd);

        void OnError(int fd);

        static void SetNonblock(int fd);

        void RemoveClient(int fd);

        virtual TcpSession *
        Accept(int fd, const char *remote_ip, int remote_port, TcpConnection *pServer) { return nullptr; }

        void CreateEvent(int fd, uint8_t socket_event);

    protected:
        int _sock;
        Dispatcher *_p;
        std::map<int, TcpSession *> _clients;

    };
}


#endif //SAMPLE_ECHO_SERVER_TCPCONNECTION_H
