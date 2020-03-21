//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_DISPATCHER_H
#define SAMPLE_ECHO_SERVER_DISPATCHER_H

#include <vector>
#include "TcpConnection.h"

namespace SZ {

    enum {
        SOCKET_READ = 0x1,
        SOCKET_WRITE = 0x2,
        SOCKET_EXCEP = 0x4,
    };

    class TcpConnection;

    class EchoServer;

    class Dispatcher {
        friend class TcpConnection;

        friend class EchoServer;

    public:
        virtual ~Dispatcher();

        virtual void DoEvent() = 0;

    protected:
        void AddServer(TcpConnection *svr);

        virtual void CreateEvent(int fd, unsigned char socket_event) = 0;

    protected:
        std::vector<TcpConnection *> _servers;
    };
}


#endif //SAMPLE_ECHO_SERVER_DISPATCHER_H
