//
// Created by Administrator on 2020/8/20.
//


#include "Poller.h"
#include "EventBase.h"
#include "Channel.h"
#include "EPoll.h"
#include "Selector.h"

#include "platform.h"

using namespace net;


Poller::Poller(EventBase *loop)
        : ownerLoop_(loop) {
}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel *channel) const {
    assertInLoopThread();
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

Poller *Poller::newDefaultPoller(EventBase *loop) {
#ifdef linux
    return new EPoll(loop);
#elif defined(__WINDOWS__)
    return new Selector(loop);
#endif
}

