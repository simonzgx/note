//
// Created by Administrator on 2020/8/20.
//


#include <cassert>
#include <cerrno>

#include "platform.h"
#ifdef __WINDOWS__

#elif defined(linux)
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#endif

#include "EPoll.h"
#include "logger.h"
#include "Channel.h"


#ifdef linux
// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.
static_assert(EPOLLIN == POLLIN, "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI, "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT, "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP, "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR, "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP, "epoll uses same flag values as poll");
#endif // linux


namespace {
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

using namespace net;

#ifdef linux

EPoll::EPoll(EventBase *loop)
        : Poller(loop),
          epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
          events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_SYSFATAL ("%s", "EPollPoller::EPollPoller");
    }
}

EPoll::~EPoll() {
    ::close(epollfd_);
}

Timestamp EPoll::poll(int timeoutMs, ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(epollfd_,
                                 &*events_.begin(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
//        LOG_TRACE << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG_TRACE ("%s", "nothing happened");
    } else {
        // error happens, log uncommon ones
        if (savedErrno != EINTR) {
            errno = savedErrno;
            LOG_SYSERR ("%s", "EPollPoller::poll()");
        }
    }
    return now;
}

void EPoll::fillActiveChannels(int numEvents,
                               ChannelList *activeChannels) const {
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; ++i) {
        auto *channel = static_cast<Channel *>(events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        auto it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
#endif
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoll::updateChannel(Channel *channel) {
    Poller::assertInLoopThread();
    const int index = channel->index();
    LOG_TRACE ("fd = %d  events = %d  index = %d", channel->fd(),  channel->events(),index );
    if (index == kNew || index == kDeleted) {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew) {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        } else // index == kDeleted
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        (void) fd;
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoll::removeChannel(Channel *channel) {
    Poller::assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE ("fd = %d ",fd );
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void) n;
    assert(n == 1);

    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPoll::update(int operation, Channel *channel) {
    struct epoll_event event{};
    memset(&event, '\0', sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_TRACE ("epoll_ctl op =  %s fd = %d,  event = {%s}",operationToString(operation), fd, channel->eventsToString().c_str() );
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_SYSERR ("epoll_ctl op =  %s fd = %d",operationToString(operation), fd);
        } else {
            LOG_SYSFATAL ("epoll_ctl op = %s fd = %d",operationToString(operation), fd);
        }
    }
}

const char *EPoll::operationToString(int op) {
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

#endif // linux
