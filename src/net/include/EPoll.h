//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_EPOLL_H
#define NET_EPOLL_H

#include "Poller.h"

namespace net {
    class EventBase;

    class EPoll : public Poller {
    public:
        EPoll(EventBase* loop);

        ~EPoll() override;

        Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

        void updateChannel(Channel *channel) override;

        void removeChannel(Channel *channel) override;

    private:
        static const int kInitEventListSize = 16;

        static const char *operationToString(int op);

        void fillActiveChannels(int numEvents,
                                ChannelList *activeChannels) const;

        void update(int operation, Channel *channel);

        typedef std::vector<struct epoll_event> EventList;

        int epollfd_;
        EventList events_;
    };
}


#endif //NET_EPOLL_H
