//
// Created by Administrator on 2020/8/20.
//

#include "EventLoopThread.h"

#include <utility>

using namespace net;

EventLoopThread::EventLoopThread(ThreadInitCallback cb,
                                 const string &name)
        : loop_(nullptr),
          exiting_(false),
          callback_(std::move(cb)) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) // not 100% race-free, eg. threadFunc could be running callback_.
    {
        // still a tiny chance to call destructed object, if threadFunc exits just now.
        // but when EventLoopThread destructs, usually programming is exiting anyway.
        loop_->quit();
        thread_.join();
    }
}

EventBase *EventLoopThread::startLoop() {

    EventBase *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == nullptr) {
            cond_.wait(lock, [this]() -> bool { return loop_ != nullptr; });
        }
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::threadFunc() {
    EventBase loop;

    if (callback_) {
        callback_(&loop);
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
    //assert(exiting_);
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = nullptr;
}