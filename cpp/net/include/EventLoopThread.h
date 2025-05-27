//
// Created by Administrator on 2020/8/20.
//

#ifndef TRANS_SDK_EVENTLOOPTHREAD_H
#define TRANS_SDK_EVENTLOOPTHREAD_H


#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "EventBase.h"

namespace net {
    class EventLoopThread {

        using string = std::string;

    public:
        typedef std::function<void(EventBase *)> ThreadInitCallback;

        explicit EventLoopThread(ThreadInitCallback cb = ThreadInitCallback(),
                        const string &name = string());

        ~EventLoopThread();

        EventBase *startLoop();

    private:
        void threadFunc();

        EventBase *loop_;
        bool exiting_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback callback_;
    };

}


#endif //TRANS_SDK_EVENTLOOPTHREAD_H
