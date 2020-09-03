//
// Created by Administrator on 2020/8/21.
//

#ifndef NET_ECHOSERVER_H
#define NET_ECHOSERVER_H


#include "TcpServer.h"

using namespace net;

class EchoServer {

public:
    EchoServer(EventBase *loop,
               const Address &listenAddr);

    void start();  // calls server_.start();

private:
    void onConnection(const TcpConnectionPtr &conn);

    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);

    net::TcpServer server_;
};


#endif //NET_ECHOSERVER_H
