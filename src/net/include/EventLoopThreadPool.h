//
// Created by Administrator on 2020/8/20.
//

#ifndef TRANS_SDK_EVENTLOOPTHREADPOOL_H
#define TRANS_SDK_EVENTLOOPTHREADPOOL_H


#include <functional>
#include <memory>
#include <vector>

#include "noncopyable.h"

namespace net {
    class EventBase;

    class EventLoopThread;
    class EventLoopThreadPool {


        using string = std::string;
    public:
        typedef std::function<void(EventBase *)> ThreadInitCallback;

        EventLoopThreadPool(EventBase *baseLoop, string nameArg);

        ~EventLoopThreadPool();

        void setThreadNum(int numThreads) { numThreads_ = numThreads; }

        void start(const ThreadInitCallback &cb = ThreadInitCallback());

        // valid after calling start()
        /// round-robin
        EventBase *getNextLoop();

        /// with the same hash code, it will always return the same EventLoop
        EventBase *getLoopForHash(size_t hashCode);

        std::vector<EventBase *> getAllLoops();

        bool started() const { return started_; }

        const string &name() const { return name_; }

    private:

        EventBase *baseLoop_;
        string name_;
        bool started_;
        int numThreads_;
        unsigned long next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventBase *> loops_;
    };
}


#endif //TRANS_SDK_EVENTLOOPTHREADPOOL_H
