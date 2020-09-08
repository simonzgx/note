//
// Created by Administrator on 2020/8/20.
//

#include "TcpServer.h"

#include <utility>
#include "EventBase.h"
#include "Address.h"
#include "EventLoopThreadPool.h"
#include "Acceptor.h"

using namespace net;

TcpServer::TcpServer(EventBase *loop,
                     const Address &listenAddr,
                     string nameArg,
                     Option option)
        : loop_(loop),
          ipPort_(listenAddr.toIpPort()),
          name_(std::move(nameArg)),
          acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
          threadPool_(new EventLoopThreadPool(loop, name_)),
//          connectionCallback_(defaultConnectionCallback),
//          messageCallback_(defaultMessageCallback),
          nextConnId_(1){
    started_ = 0;
    acceptor_->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
    LOG_TRACE ("TcpServer::~TcpServer [%s] destructing", name_.c_str());

    for (auto &item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(
                [conn] { conn->connectDestroyed(); });
    }
}

void TcpServer::setThreadNum(int numThreads) {
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if (started_ == 0) {
        threadPool_->start(threadInitCallback_);

        assert(!acceptor_->listenning());
        loop_->runInLoop(
                std::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int sockfd, const Address &peerAddr) {
    loop_->assertInLoopThread();
    EventBase *ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;
    LOG_INFO ("TcpServer::newConnection [%s] - new connection [%s] from %s", name_.c_str(),connName.c_str(),peerAddr.toIpPort().c_str());
    Address localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            [this](auto && PH1) { removeConnection(PH1); }); // FIXME: unsafe
    ioLoop->runInLoop([conn] { conn->connectEstablished(); });
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    // FIXME: unsafe
    loop_->runInLoop([this, conn] { removeConnectionInLoop(conn); });
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();

    LOG_INFO ( "TcpServer::removeConnectionInLoop [%s] - connection %s", name_.c_str(), conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    (void) n;
    assert(n == 1);
    EventBase *ioLoop = conn->getLoop();
    ioLoop->queueInLoop([conn] { conn->connectDestroyed(); });
}

