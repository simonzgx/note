//
// Created by Administrator on 2020/8/20.
//

#include <cerrno>
#include <memory>

#include "Connector.h"

#include "logger.h"
#include "Channel.h"
#include "EventBase.h"
#include "SocketOption.h"
#include "Timer.h"


using namespace net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventBase *loop, const Address &serverAddr)
        : ev_base(loop),
          serverAddr_(serverAddr),
          connect_(false),
          state_(kDisconnected),
          retryDelayMs_(kInitRetryDelayMs) {
    LOG_DEBUG << "ctor[" << this << "]";
}

Connector::~Connector() {
    LOG_DEBUG << "dtor[" << this << "]";
    assert(!channel_);
}

void Connector::start() {
    connect_ = true;
    ev_base->runInLoop([this] { startInLoop(); }); // FIXME: unsafe
}

void Connector::startInLoop() {
    ev_base->assertInLoopThread();
    assert(state_ == kDisconnected); //fixme def NDEBUG
    if (connect_) {
        connect();
    } else {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::stop() {
    connect_ = false;
    ev_base->queueInLoop([this] { stopInLoop(); }); // FIXME: unsafe
    // FIXME: cancel timer
}

void Connector::stopInLoop() {
    ev_base->assertInLoopThread();
    if (state_ == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect() {
    int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;

        default:
            LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            // connectErrorCallback_();
            break;
    }
}

void Connector::restart() {
    ev_base->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd) {
    setState(kConnecting);
    assert(!channel_);
    channel_ = std::make_unique<Channel>(ev_base, sockfd);
    channel_->setWriteCallback(
            [this] { handleWrite(); }); // FIXME: unsafe
    channel_->setErrorCallback(
            [this] { handleError(); }); // FIXME: unsafe

    // channel_->tie(shared_from_this()); is not working,
    // as channel_ is not managed by shared_ptr
    channel_->enableWriting();
}

int Connector::removeAndResetChannel() {
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // Can't reset channel_ here, because we are inside Channel::handleEvent
    ev_base->queueInLoop(std::bind(&Connector::resetChannel, this)); // FIXME: unsafe
    return sockfd;
}

void Connector::resetChannel() {
    channel_.reset();
}

void Connector::handleWrite() {
    LOG_TRACE << "Connector::handleWrite " << state_;

    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err) {
            LOG_WARN << "Connector::handleWrite - SO_ERROR = "
                     << err << " " << gai_strerror(err);
            retry(sockfd);
        } else if (sockets::isSelfConnect(sockfd)) {
            LOG_WARN << "Connector::handleWrite - Self connect";
            retry(sockfd);
        } else {
            setState(kConnected);
            if (connect_) {
                newConnectionCallback_(sockfd);
            } else {
                sockets::close(sockfd);
            }
        }
    } else {
        // what happened?
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError() {
    LOG_ERROR << "Connector::handleError state=" << state_;
    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE << "SO_ERROR = " << err << " " << gai_strerror(err);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd) {
    sockets::close(sockfd);
    setState(kDisconnected);
    if (connect_) {
        LOG_INFO << "Connector::retry - Retry connecting to " << serverAddr_.toIpPort()
                 << " in " << retryDelayMs_ << " milliseconds. ";
        ev_base->runAfter(retryDelayMs_ / 1000.0,
                        std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    } else {
        LOG_DEBUG << "do not connect";
    }
}

