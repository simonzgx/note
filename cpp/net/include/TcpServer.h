//
// Created by Administrator on 2020/8/20.
//

#ifndef TRANS_SDK_TCPSERVER_H
#define TRANS_SDK_TCPSERVER_H

#include <string>
#include <atomic>
#include <functional>
#include <map>


#include "TcpConnection.h"

namespace net {


    class EventLoopThreadPool;
    class Acceptor;
    class EventBase;


    class TcpServer {

        using string = std::string;
    public:

        typedef std::function<void(EventBase *)> ThreadInitCallback;
        enum Option {
            kNoReusePort,
            kReusePort,
        };

        //TcpServer(EventLoop* loop, const InetAddress& listenAddr);
        TcpServer(EventBase *loop,
                  const Address &listenAddr,
                  string nameArg,
                  Option option = kNoReusePort);

        ~TcpServer();  // force out-line dtor, for std::unique_ptr members.

        const string &ipPort() const { return ipPort_; }

        const string &name() const { return name_; }

        EventBase *getLoop() const { return loop_; }

        /// Set the number of threads for handling input.
        ///
        /// Always accepts new connection in loop's thread.
        /// Must be called before @c start
        /// @param numThreads
        /// - 0 means all I/O in loop's thread, no thread will created.
        ///   this is the default value.
        /// - 1 means all I/O in another thread.
        /// - N means a thread pool with N threads, new connections
        ///   are assigned on a round-robin basis.
        void setThreadNum(int numThreads);

        void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }

        /// valid after calling start()
        std::shared_ptr<EventLoopThreadPool> threadPool() { return threadPool_; }

        /// Starts the server if it's not listenning.
        ///
        /// It's harmless to call it multiple times.
        /// Thread safe.
        void start();

        /// Set connection callback.
        /// Not thread safe.
        void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

        /// Set message callback.
        /// Not thread safe.
        void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

        /// Set write complete callback.
        /// Not thread safe.
        void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

    private:
        /// Not thread safe, but in loop
        void newConnection(int sockfd, const Address &peerAddr);

        /// Thread safe.
        void removeConnection(const TcpConnectionPtr &conn);

        /// Not thread safe, but in loop
        void removeConnectionInLoop(const TcpConnectionPtr &conn);

        typedef std::map<string, TcpConnectionPtr> ConnectionMap;

        EventBase *loop_;  // the acceptor loop
        const string ipPort_;
        const string name_;
        std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
        std::shared_ptr<EventLoopThreadPool> threadPool_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        ThreadInitCallback threadInitCallback_;
        std::atomic_int started_;
        // always in loop thread
        int nextConnId_;
        ConnectionMap connections_;
    };
}


#endif //TRANS_SDK_TCPSERVER_H
