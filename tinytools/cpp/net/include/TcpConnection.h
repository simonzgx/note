//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_TCPCONNECTION_H
#define NET_TCPCONNECTION_H

#include <string>

#include "Address.h"
#include "Callbacks.h"
#include "Any.h"
#include "Buffer.h"
#include "noncopyable.h"

struct tcp_info;

namespace net {

    class Channel;

    class EventBase;

    class Socket;

    class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {

        using string = std::string;

    public:
        /// Constructs a TcpConnection with a connected sockfd
        ///
        /// User should not create this object.
        TcpConnection(EventBase *loop,
                      string name,
                      int sockfd,
                      const Address &localAddr,
                      const Address &peerAddr);

        ~TcpConnection();

        EventBase *getLoop() const { return loop_; }

        const string &name() const { return name_; }

        const Address &localAddress() const { return localAddr_; }

        const Address &peerAddress() const { return peerAddr_; }

        bool connected() const { return state_ == kConnected; }

        bool disconnected() const { return state_ == kDisconnected; }

        // return true if success.
        bool getTcpInfo(struct tcp_info *) const;

        string getTcpInfoString() const;

        // void send(string&& message); // C++11
        void send(const void *message, int len);

        void send(const string &message);

        // void send(Buffer&& message); // C++11
        void send(Buffer *message);  // this one will swap data
        void shutdown(); // NOT thread safe, no simultaneous calling
        // void shutdownAndForceCloseAfter(double seconds); // NOT thread safe, no simultaneous calling
        void forceClose();

        void setTcpNoDelay(bool on);

        // reading or not
        void startRead();

        void stopRead();

        bool isReading() const { return reading_; }; // NOT thread safe, may race with start/stopReadInLoop

        void setContext(const Any &context) { context_ = context; }

        const Any &getContext() const { return context_; }

        Any *getMutableContext() { return &context_; }

        void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

        void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

        void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

        void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
            highWaterMarkCallback_ = cb;
            highWaterMark_ = highWaterMark;
        }

        /// Advanced interface
        Buffer *inputBuffer() { return &inputBuffer_; }

        Buffer *outputBuffer() { return &outputBuffer_; }

        /// Internal use only.
        void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

        // called when TcpServer accepts a new connection
        void connectEstablished();   // should be called only once
        // called when TcpServer has removed me from its map
        void connectDestroyed();  // should be called only once

    private:
        enum StateE {
            kDisconnected, kConnecting, kConnected, kDisconnecting
        };

        void handleRead(Timestamp receiveTime);

        void handleWrite();

        void handleClose();

        void handleError();

        // void sendInLoop(string&& message);
        void sendInLoop(const string &message);

        void sendInLoop(const void *message, size_t len);

        void shutdownInLoop();

        // void shutdownAndForceCloseInLoop(double seconds);
        void forceCloseInLoop();

        void setState(StateE s) { state_ = s; }

        const char *stateToString() const;

        void startReadInLoop();

        void stopReadInLoop();

        EventBase *loop_;
        const string name_;
        StateE state_;  // FIXME: use atomic variable
        bool reading_;
        // we don't expose those classes to client.
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        const Address localAddr_;
        const Address peerAddr_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        HighWaterMarkCallback highWaterMarkCallback_;
        CloseCallback closeCallback_;
        size_t highWaterMark_;
        Buffer inputBuffer_;
        Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
        Any context_;
        // FIXME: creationTime_, lastReceiveTime_
        //        bytesReceived_, bytesSent_
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
}


#endif //NET_TCPCONNECTION_H
