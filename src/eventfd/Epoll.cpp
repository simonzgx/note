//
// Created by Administrator on 2020/9/29.
//

#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>

#include "Epoll.h"
#include "ChannelBus.h"
#include "Channel.h"

const char *operationToString(int op) {
    switch (op) {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            assert(false && "ERROR op");
            return "Unknown Operation";
    }
}

gochan::Epoll::Epoll(gochan::ChannelBus *bus) : bus_(bus), events_(EVENTS_INIT_SIZE),
                                                epfd_(epoll_create1(EPOLL_CLOEXEC)) {

}

gochan::Epoll::~Epoll() {
    if (epfd_) {
        ::close(epfd_);
    }
}

void gochan::Epoll::poll(int timeoutMs, gochan::ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(epfd_,
                                 &*events_.begin(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        printf("nothing happened\n");
    } else {
        if (savedErrno != EINTR) {
            errno = savedErrno;
            perror("poll error occurs! \n");
        }
    }
}

void gochan::Epoll::updateChannel(gochan::Channel *channel) {
    struct epoll_event event{};
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    auto it = channelMap_.find(fd);
    if (it == channelMap_.end()) {
        channelMap_[fd] = channel;
        update(EPOLL_CTL_ADD, channel);
    } else {
        update(EPOLL_CTL_DEL, channel);
        channelMap_.erase(it);
    }
}

void gochan::Epoll::removeChannel(gochan::Channel *channel) {
    int fd = channel->fd();
    auto it = channelMap_.find(fd);
    assert(it != channelMap_.end());
    assert(it->second == channel);
    channelMap_.erase(it);
    update(EPOLL_CTL_DEL, channel);
}

void gochan::Epoll::update(int operation, gochan::Channel *channel) const {
    struct epoll_event event{};
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            printf("epoll_ctl op =  %s fd = %d", operationToString(operation), fd);
        } else {
            printf("epoll_ctl op = %s fd = %d", operationToString(operation), fd);
        }
    }
}

void gochan::Epoll::fillActiveChannels(int numEvents, gochan::ChannelList *activeChannels) const {
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; ++i) {
        auto *channel = static_cast<Channel *>(events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        auto it = channelMap_.find(fd);
        assert(it != channelMap_.end());
        assert(it->second == channel);
#endif
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}
