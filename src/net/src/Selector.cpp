//
// Created by Administrator on 2020/9/12.
//

#include "Selector.h"
#ifdef __WINDOWS__
net::Selector::Selector(net::EventBase *ev) : Poller(ev) {

}

net::Selector::~Selector() {

}

net::Timestamp net::Selector::poll(int timeoutMs, net::Poller::ChannelList *activeChannels) {
    return net::Timestamp();
}

void net::Selector::updateChannel(net::Channel *channel) {

}

void net::Selector::removeChannel(net::Channel *channel) {

}
#endif

