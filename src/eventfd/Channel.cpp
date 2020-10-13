//
// Created by Administrator on 2020/9/29.
//

#include <poll.h>
#include <sys/eventfd.h>
#include <cassert>
#include <unistd.h>

#include "Channel.h"
#include "ChannelBus.h"

using namespace gochan;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

namespace {
    int createEventFD() {
        int evtFD = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        assert(evtFD >= 0);
        return evtFD;
    }
}


Channel::Channel(ChannelBus *bus, size_t size) : fd_(createEventFD()), bus_(bus),
                                                 events_(kReadEvent), size_(size) {
    update();
    //default close callback function
    closeCb_ = [](Channel *chan) -> void {
        chan->push(CLOSE_SIGNAL);
        chan->remove();
    };
}

Channel::~Channel() {
    events_ = kNoneEvent;
    update();
    if (fd_) {
        ::close(fd_);
    }
}

void Channel::remove() {
    bus_->removeChannel(this);
}

void Channel::update() {
    bus_->updateChannel(this);
}

void Channel::handleEvent() {
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        perror("POLLHUP warning");
        closeCb_(this);
        return;
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        for (auto &it:eventCb_) {
            it(this);
        }
        EVENT_SIGNAL val;
        ::read(fd_, &val, sizeof val);
    }
}

void Channel::close() {
    this->push(CLOSE_SIGNAL);
    EVENT_SIGNAL val;
    write(fd_, &val, sizeof val);
}
