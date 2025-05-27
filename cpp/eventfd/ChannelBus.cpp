//
// Created by Administrator on 2020/9/29.
//

#include <cassert>

#include "Channel.h"
#include "ChannelBus.h"
#include "Epoll.h"

using namespace gochan;

namespace {
    const int kPollTimeMs = 10000;
}

ChannelBus *ChannelBus::instance = nullptr;

gochan::ChannelBus::ChannelBus() : poller_(new Epoll(this)) {

}

void ChannelBus::run() {
    while (!stop_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for (auto chan:activeChannels_) {
            currentActiveChannel_ = chan;
            currentActiveChannel_->handleEvent();
        }
    }
}

void ChannelBus::updateChannel(Channel *channel) {
    this->poller_->updateChannel(channel);
}

void ChannelBus::removeChannel(Channel *channel) {
    this->poller_->removeChannel(channel);
}


