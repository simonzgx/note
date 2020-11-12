//
// Created by Administrator on 2020/9/29.
//

#ifndef EVENTFD_EPOLL_H
#define EVENTFD_EPOLL_H

#include <vector>
#include <map>

namespace gochan {

    class Channel;

    class ChannelBus;

    using ChannelList = std::vector<Channel *>;
    using EventList = std::vector<struct epoll_event>;
    using ChannelMap = std::map<int, Channel *>;
    const int EVENTS_INIT_SIZE = 16;

    class Epoll {
    public:
        explicit Epoll(ChannelBus *bus);

        ~Epoll();

        void poll(int timeoutMs, ChannelList *activeChannels);

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

    private:

        void update(int operation, Channel *channel) const;

        void fillActiveChannels(int numEvents,
                                ChannelList *activeChannels) const;

    private:
        ChannelBus *bus_;
        int epfd_;
        EventList events_;
        ChannelList channelList_;
        ChannelMap channelMap_;
    };
}


#endif //EVENTFD_EPOLL_H
