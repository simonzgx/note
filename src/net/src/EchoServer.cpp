//
// Created by Administrator on 2020/8/21.
//

#include <string>


#include "EchoServer.h"
#include "EventBase.h"
#include "Address.h"
#include "Timer.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "Callbacks.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using namespace net;

EchoServer::EchoServer(EventBase *loop,
                       const Address &listenAddr)
        : server_(loop, listenAddr, "EchoServer") {
    server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start() {
    server_.start();
}

void EchoServer::onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void EchoServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp time) {
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();
    conn->send(msg);
}
