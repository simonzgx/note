//
// Created by Administrator on 2020/8/19.
//
#include <iostream>

#include <thread>
#include <cassert>
#include <csignal>
#include <unistd.h>
#include <sys/eventfd.h>

#include "Timer.h"
#include "Channel.h"
#include "EventBase.h"
#include "logger.h"
#include "Poller.h"
#include "CurrentThread.h"
#include "SocketOption.h"

using namespace net;

namespace {
    __thread EventBase *t_loopInThisThread = nullptr;

    const int kPollTimeMs = 10000;

    int createEventfd() {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            LOG_SYSERR << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }

#pragma GCC diagnostic ignored "-Wold-style-cast"

    class IgnoreSigPipe {
    public:
        IgnoreSigPipe() {
            ::signal(SIGPIPE, SIG_IGN);
            // LOG_TRACE << "Ignore SIGPIPE";
        }
    };

#pragma GCC diagnostic error "-Wold-style-cast"

    IgnoreSigPipe initObj;
}  // namespace

EventBase *EventBase::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

EventBase::EventBase()
        : looping_(false),
          quit_(false),
          eventHandling_(false),
          callingPendingFunctors_(false),
          iteration_(0),
          threadId_(CurrentThread::tid()),
          poller_(Poller::newDefaultPoller(this)),
          timerQueue_(new TimerQueue(this)),
          wakeupFd_(createEventfd()),
          wakeupChannel_(new Channel(this, wakeupFd_)),
          currentActiveChannel_(nullptr) {
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(
            std::bind(&EventBase::handleRead, this));
    // we are always reading the wakeupfd
    wakeupChannel_->enableReading();
}


EventBase::~EventBase() {
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
              << " destructs in thread " << CurrentThread::tid();
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventBase::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
    LOG_TRACE << "EventLoop " << this << " start looping";

    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        ++iteration_;
//        if (Logger::logLevel() <= Logger::TRACE) {
        printActiveChannels();
//        }
        // TODO sort channel by priority
        eventHandling_ = true;
        for (Channel *channel : activeChannels_) {
            currentActiveChannel_ = channel;
//            LOG_INFO << "channel fd:" << channel->fd_ << std::endl;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventBase::quit() {
    quit_ = true;
    // There is a chance that loop() just executes while(!quit_) and exits,
    // then EventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mutex_ in both places.
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventBase::runInLoop(const Functor &cb) {
    LOG_INFO << "runInLoop" << std::endl;
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventBase::queueInLoop(const Functor &cb) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

size_t EventBase::queueSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pendingFunctors_.size();
}

TimerId EventBase::runAt(Timestamp time, const TimerCallback &cb) {
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventBase::runAfter(double delay, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventBase::runEvery(double interval, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventBase::cancel(TimerId timerId) {
    return timerQueue_->cancel(timerId);
}

void EventBase::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventBase::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventBase::hasChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventBase::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

void EventBase::wakeup() const {
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventBase::handleRead() const {
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventBase::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor : functors) {
        functor();
    }
    callingPendingFunctors_ = false;
}

void EventBase::printActiveChannels() const {
    for (const Channel *channel : activeChannels_) {
//        LOG_TRACE << "{" << channel->reventsToString() << "} ";
    }
}

