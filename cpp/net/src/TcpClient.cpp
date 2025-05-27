//
// Created by Administrator on 2020/8/20.
//

#include "EventBase.h"
#include "TcpClient.h"

#include "logger.h"
#include "Connector.h"
#include "Timer.h"

using namespace net;
using string = std::string;

void removeConnection(EventBase *loop, const TcpConnectionPtr &conn) {
    loop->queueInLoop([conn] { conn->connectDestroyed(); });
}

void removeConnector(const ConnectorPtr &connector) {
    //fixme
}


TcpClient::TcpClient(EventBase *loop,
                     const Address &serverAddr,
                     string nameArg)
        : loop_(loop),
          connector_(new Connector(loop, serverAddr)),
          name_(std::move(nameArg)),
          retry_(true),
          connect_(false),
          nextConnId_(1) {
    connector_->setNewConnectionCallback(
            [this](auto &&PH1) { newConnection(PH1); });
    //fixme : do something when connect failed
    this->connector_->setConnectFailedCallback([this]() {
        LOG_ERROR("%s", "Connect Failed!");
    });
    LOG_INFO("TcpClient::TcpClient[%s] - connector", this->name_.c_str());
}

TcpClient::~TcpClient() {
    LOG_INFO("TcpClient::~TcpClient[%s] - connector", this->name_.c_str());
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        unique = this->connection_.unique();
        conn = this->connection_;
    }
    if (conn) {
        assert(this->loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        CloseCallback cb = [this](auto &&PH1) {
            return ::removeConnection(this->loop_, PH1);
        };
        this->loop_->runInLoop(
                [conn, cb] { conn->setCloseCallback(cb); });
        if (unique) {
            conn->forceClose();
        }
    } else {
        this->connector_->stop();
        // FIXME: HACK
        this->loop_->runAfter(1, [this] { return ::removeConnector(this->connector_); });
    }
}


void TcpClient::connect() {
    LOG_INFO("TcpClient::connect[%s] - connecting to %s", this->name_.c_str(), this->connector_->serverAddress().toIpPort().c_str());
    this->connector_->start();
}

void TcpClient::disconnect() {
    this->connect_ = false;

    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        if (this->connection_) {
            this->connection_->shutdown();
        }
    }
}

void TcpClient::stop() {
    this->connect_ = false;
    this->connector_->stop();
}

bool TcpClient::getConnectedState() const {
    return this->connect_;
}


void TcpClient::newConnection(int sockfd) {
    this->loop_->assertInLoopThread();
    Address peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), this->nextConnId_);
    ++this->nextConnId_;
    string connName = this->name_ + buf;

    Address localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(this->loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    conn->setConnectionCallback(this->connectionCallback_);
    conn->setMessageCallback(this->messageCallback_);
    conn->setWriteCompleteCallback(this->writeCompleteCallback_);
    conn->setCloseCallback([this](auto &&PH1) {
        this->removeConnection(PH1);
    });
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        this->connection_ = conn;
    }
    this->connect_ = true;
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr &conn) {
    this->loop_->assertInLoopThread();
    assert(this->loop_ == conn->getLoop());

    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        assert(this->connection_ == conn);
        this->connection_.reset();
    }

    this->loop_->queueInLoop([conn] { conn->connectDestroyed(); });
    if (this->retry_ && this->connect_) {
        LOG_INFO("TcpClient::connect[%s] - Reconnecting to %s", this->name_.c_str(), this->connector_->serverAddress().toIpPort().c_str());
        this->connector_->restart();
    }
}

bool TcpClient::send(Buffer *buf, MessageCallback &cb) {
    this->send(buf);
}

bool TcpClient::send(Buffer *buf) {
    this->connection_->send(buf);
}

void TcpClient::setCallbacks(ConnectionCallback &conn_cb, MessageCallback &msg_cb) {
    this->connectionCallback_ = conn_cb;
    this->messageCallback_ = msg_cb;
}
