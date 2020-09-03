//
// Created by Administrator on 2020/8/20.
//

#include "EventLoopThreadPool.h"

#include <utility>
#include <cassert>
#include "EventBase.h"
#include "EventLoopThread.h"


using namespace net;

EventLoopThreadPool::EventLoopThreadPool(EventBase *baseLoop, string nameArg)
        : baseLoop_(baseLoop),
          name_(std::move(nameArg)),
          started_(false),
          numThreads_(0),
          next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
    // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        auto *t = new EventLoopThread(cb, buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }
}

EventBase *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventBase *loop = baseLoop_;

    if (!loops_.empty()) {
        // round-robin
        loop = loops_[next_];
        ++next_;
        if (reinterpret_cast<size_t>(next_) >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

EventBase *EventLoopThreadPool::getLoopForHash(size_t hashCode) {
    baseLoop_->assertInLoopThread();
    EventBase *loop = baseLoop_;

    if (!loops_.empty()) {
        loop = loops_[hashCode % loops_.size()];
    }
    return loop;
}

std::vector<EventBase *> EventLoopThreadPool::getAllLoops() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    if (loops_.empty()) {
        return std::vector<EventBase *>(1, baseLoop_);
    } else {
        return loops_;
    }
}
