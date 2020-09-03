//
// Created by Administrator on 2020/8/20.
//

#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <vector>
#include <map>

#include "EventBase.h"

namespace net {

    class Channel;

    class EventBase;

    class Timestamp;

    class Poller {
    public:
        typedef std::vector<Channel *> ChannelList;

        Poller(EventBase *loop);

        virtual ~Poller();

        /// Polls the I/O events.
        /// Must be called in the loop thread.
        virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

        /// Changes the interested I/O events.
        /// Must be called in the loop thread.
        virtual void updateChannel(Channel *channel) = 0;

        /// Remove the channel, when it destructs.
        /// Must be called in the loop thread.
        virtual void removeChannel(Channel *channel) = 0;

        virtual bool hasChannel(Channel *channel) const;

        static Poller *newDefaultPoller(EventBase *loop);

        void assertInLoopThread() const {
            ownerLoop_->assertInLoopThread();
        }

    protected:
        typedef std::map<int, Channel *> ChannelMap;
        ChannelMap channels_;

    private:
        EventBase *ownerLoop_;
    };

}


#endif //NET_POLLER_H
