//
// Created by Administrator on 2020/8/20.
//

#include "TcpConnection.h"


#include <memory>
#include <cstring>

#include "EventBase.h"
#include "Socket.h"
#include "SocketOption.h"
#include "Channel.h"


using namespace net;
using string = std::string;

TcpConnection::TcpConnection(EventBase *loop,
                             string nameArg,
                             int sockfd,
                             const Address &localAddr,
                             const Address &peerAddr)
        : loop_(loop),
          name_(std::move(nameArg)),
          state_(kConnecting),
          reading_(true),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr),
          highWaterMark_(64 * 1024 * 1024) {
    channel_->setReadCallback(
            [this](auto && PH1) { handleRead(PH1); });
    channel_->setWriteCallback(
            [this] { handleWrite(); });
    channel_->setCloseCallback(
            [this] { handleClose(); });
    channel_->setErrorCallback(
            [this] { handleError(); });
    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
              << " fd=" << sockfd;
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
              << " fd=" << channel_->fd()
              << " state=" << stateToString();
    assert(state_ == kDisconnected);
}

bool TcpConnection::getTcpInfo(struct tcp_info *tcpi) const {
    return socket_->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const {
    char buf[1024];
    buf[0] = '\0';
    socket_->getTcpInfoString(buf, sizeof buf);
    return buf;
}

void TcpConnection::send(const void *data, int len) {
//    send(StringPiece(static_cast<const char*>(data), len));
}

void TcpConnection::send(const string &message) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            void (TcpConnection::*fp)(const string &message) = &TcpConnection::sendInLoop;
            loop_->runInLoop(
                    std::bind(fp,
                              this,     // FIXME
                              message));
        }
    }
}

// FIXME efficiency!!!
void TcpConnection::send(Buffer *buf) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        } else {
            void (TcpConnection::*fp)(const string &message) = &TcpConnection::sendInLoop;
            loop_->runInLoop(
                    std::bind(fp,
                              this,     // FIXME
                              buf->retrieveAllAsString()));
            //std::forward<string>(message)));
        }
    }
}

void TcpConnection::sendInLoop(const string &message) {
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void *data, size_t len) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected) {
        LOG_WARN << "disconnected, give up writing";
        return;
    }
    // if no thing in output queue, try writing directly
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = sockets::write(channel_->fd(), data, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        } else // nwrote < 0
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << "TcpConnection::sendInLoop";
                if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
                {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);
    if (!faultError && remaining > 0) {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_
            && oldLen < highWaterMark_
            && highWaterMarkCallback_) {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }
        outputBuffer_.append(static_cast<const char *>(data) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown() {
    // FIXME: use compare and swap
    if (state_ == kConnected) {
        setState(kDisconnecting);
        // FIXME: shared_from_this()?
        loop_->runInLoop([this] { shutdownInLoop(); });
    }
}

void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        // we are not writing
        socket_->shutdownWrite();
    }
}


void TcpConnection::forceClose() {
    // FIXME: use compare and swap
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->queueInLoop([capture0 = shared_from_this()] { capture0->forceCloseInLoop(); });
    }
}

void TcpConnection::forceCloseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        // as if we received 0 byte in handleRead();
        handleClose();
    }
}

const char *TcpConnection::stateToString() const {
    switch (state_) {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket_->setTcpNoDelay(on);
}

void TcpConnection::startRead() {
    loop_->runInLoop([this] { startReadInLoop(); });
}

void TcpConnection::startReadInLoop() {
    loop_->assertInLoopThread();
    if (!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = true;
    }
}

void TcpConnection::stopRead() {
    loop_->runInLoop([this] { stopReadInLoop(); });
}

void TcpConnection::stopReadInLoop() {
    loop_->assertInLoopThread();
    if (reading_ || channel_->isReading()) {
        channel_->disableReading();
        reading_ = false;
    }
}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    loop_->assertInLoopThread();
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = sockets::write(channel_->fd(),
                                   outputBuffer_.peek(),
                                   outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            LOG_SYSERR << "TcpConnection::handleWrite";
            // if (state_ == kDisconnecting)
            // {
            //   shutdownInLoop();
            // }
        }
    } else {
        LOG_TRACE << "Connection fd = " << channel_->fd()
                  << " is down, no more writing";
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    LOG_TRACE << "fd = " << channel_->fd() << " state = " << stateToString();
    assert(state_ == kConnected || state_ == kDisconnecting);
    // we don't close fd, leave it to dtor, so we can find leaks easily.
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    // must be the last line
    closeCallback_(guardThis);
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(channel_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
              << "] - SO_ERROR = " << err << " " << gai_strerror(err);
}


