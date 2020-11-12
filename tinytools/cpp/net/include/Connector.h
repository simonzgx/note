//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_CONNECTOR_H
#define NET_CONNECTOR_H


#include <functional>
#include <memory>
#include <cstring>

#include "noncopyable.h"
#include "Address.h"

namespace net {

    class Channel;

    class EventBase;

    class Connector : noncopyable,
                      public std::enable_shared_from_this<Connector> {
    public:
        typedef std::function<void(int sockfd)> NewConnectionCallback;
        typedef std::function<void()> ConnectFailedCallback;

        Connector(EventBase *loop, const Address &serverAddr);

        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

        void setConnectFailedCallback(const ConnectFailedCallback &cb) { newConnectFailedCallback = cb; }

        void start();  // can be called in any thread
        void restart();  // must be called in loop thread
        void stop();  // can be called in any thread

        const Address &serverAddress() const { return serverAddr_; }

    private:
        enum States {
            kDisconnected, kConnecting, kConnected
        };
        static const int kMaxRetryDelayMs = 30 * 1000;
        static const int kInitRetryDelayMs = 500;

        void setState(States s) { state_ = s; }

        void startInLoop();

        void stopInLoop();

        void connect();

        void connecting(int sockfd);

        void handleWrite();

        void handleError();

        void retry(int sockfd);

        int removeAndResetChannel();

        void resetChannel();

        EventBase *ev_base;
        Address serverAddr_;
        bool connect_; // atomic
        States state_;  // FIXME: use atomic variable
        std::unique_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        ConnectFailedCallback newConnectFailedCallback;
        int retryDelayMs_;
    };
}


#endif //NET_CONNECTOR_H
