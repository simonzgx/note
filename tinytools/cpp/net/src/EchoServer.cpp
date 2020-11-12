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
            [this](auto &&PH1) { onConnection(PH1); });
    server_.setMessageCallback(
            [this](auto &&PH1, auto &&PH2, auto &&PH3) { onMessage(PH1, PH2, PH3); });
}

void EchoServer::start() {
    server_.start();
}

void EchoServer::onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO("EchoServer - %s -> %s is %s", conn->peerAddress().toIpPort().c_str(),
             conn->localAddress().toIpPort().c_str(), (conn->connected() ? "UP" : "DOWN"));
}

void EchoServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp time) {
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO("%s echo %lu bytes, data received at %s ", conn->name().c_str(), msg.size(), time.toString().c_str());
    conn->send(msg);
}
