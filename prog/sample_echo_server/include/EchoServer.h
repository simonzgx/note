//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_ECHOSERVER_H
#define SAMPLE_ECHO_SERVER_ECHOSERVER_H

#include "TcpConnection.h"

namespace SZ {

    typedef std::function<TcpSession *(int fd, const char *remote_ip, int remote_port,
                                       TcpConnection *pServer)> TcpServerAcceptLisitener;


    class EchoServer : public TcpConnection {

    public:
        explicit EchoServer(Dispatcher *p);

        ~EchoServer() override;

        bool start(const char *name, int port, TcpServerAcceptLisitener cb);

        template<class T>
        bool start(const char *name, int port) {
            return start(name, port,
                         [this](int fd, const char *remote_ip, int remote_port,
                                SZ::TcpConnection *pServer) -> SZ::TcpSession * {
                             return new T(fd, remote_ip, remote_port, pServer);
                         });
        }

    protected:
        static int startup(int port);

        TcpSession *Accept(int fd, const char *remote_ip, int remote_port, TcpConnection *pServer) override;

        int _port{0};
        std::string _name;
        TcpServerAcceptLisitener _onAccept;
    };
}


#endif //SAMPLE_ECHO_SERVER_ECHOSERVER_H
