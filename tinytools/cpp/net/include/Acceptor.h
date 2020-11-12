//
// Created by Administrator on 2020/8/21.
//

#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include <functional>

#include "Channel.h"
#include "Socket.h"

namespace net {

    class EventBase;
    class Address;

    class Acceptor {

    public:
        typedef std::function<void(int sockfd, const Address &)> NewConnectionCallback;

        Acceptor(EventBase *loop, const Address &listenAddr, bool reuseport);

        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

        bool listenning() const { return listenning_; }

        void listen();

    private:
        void handleRead();

        EventBase *loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
        int idleFd_;
    };
}


#endif //NET_ACCEPTOR_H
