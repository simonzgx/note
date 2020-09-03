//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_TCPCLIENT_H
#define NET_TCPCLIENT_H

#include <mutex>
#include <memory>

#include <atomic>

#include "TcpConnection.h"
#include "noncopyable.h"


namespace net {

    class Connector;

    typedef std::shared_ptr<Connector> ConnectorPtr;

    class TcpClient : noncopyable {
        using string = std::string;
    public:
        TcpClient(EventBase *loop,
                  const Address &serverAddr,
                  string nameArg);

        ~TcpClient();

        void connect();

        void disconnect();

        void stop();

        bool getConnectedState() const;

        bool send(Buffer *buf);

        bool send(Buffer *buf, MessageCallback &cb);

        void setCallbacks(ConnectionCallback &conn_cb, MessageCallback &msg_cb);

    private:
        /// Not thread safe, but in loop
        void newConnection(int sockfd);

        /// Not thread safe, but in loop
        void removeConnection(const TcpConnectionPtr &conn);

        EventBase *loop_;
        ConnectorPtr connector_; // avoid revealing Connector
        const string name_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        bool retry_;   // atomic
        std::atomic<bool> connect_; // atomic
        // always in loop thread
        int nextConnId_;
        mutable std::mutex mutex_;
        TcpConnectionPtr connection_;

    };
}


#endif //NET_TCPCLIENT_H
