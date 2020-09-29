//
// Created by Administrator on 2020/9/29.
//

#ifndef EVENTFD_CHANNELBUS_H
#define EVENTFD_CHANNELBUS_H

#include <sys/epoll.h>
#include <memory>
#include <map>
#include <atomic>

#include "noncopyable.h"

namespace gochan {

    class Channel;

    class Epoll;

    using ChannelMap = std::map<int, Channel *>;
    using ChannelList = std::vector<Channel *>;

    class ChannelBus : public noncopyable, public std::enable_shared_from_this<ChannelBus> {

    public:
        static ChannelBus *getInstance() {
            if (instance == nullptr) {
                instance = new ChannelBus;
                return instance;
            }
            return instance;
        }

        ~ChannelBus() = default;

        void stop() { stop_ = true; }

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

        void run();

    private:

        ChannelBus();

    private:
        Channel *currentActiveChannel_;
        ChannelList activeChannels_;
        static ChannelBus *instance;
        ChannelMap channelMap_;
        Epoll *poller_;
        std::atomic_bool stop_{false};
    };
}


#endif //EVENTFD_CHANNELBUS_H
