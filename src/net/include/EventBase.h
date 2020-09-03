//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_EVENTBASE_H
#define NET_EVENTBASE_H

#include <cstdint>
#include <functional>
#include <vector>
#include <mutex>
#include <atomic>

#include "Timestamp.h"
#include "Callbacks.h"
#include "Any.h"
#include "CurrentThread.h"

namespace net {

    class Channel;

    class TimerQueue;

    class TimerId;

    class Poller;

    class EventBase {

    public:
        typedef std::function<void()> Functor;

        EventBase();

        ~EventBase();  // force out-line dtor, for std::unique_ptr members.

        ///
        /// Loops forever.
        ///
        /// Must be called in the same thread as creation of the object.
        ///
        void loop();

        /// Quits loop.
        ///
        /// This is not 100% thread safe, if you call through a raw pointer,
        /// better to call through shared_ptr<EventLoop> for 100% safety.
        void quit();

        ///
        /// Time when poll returns, usually means data arrival.
        ///
        Timestamp pollReturnTime() const { return pollReturnTime_; }

        int64_t iteration() const { return iteration_; }

        /// Runs callback immediately in the loop thread.
        /// It wakes up the loop, and run the cb.
        /// If in the same loop thread, cb is run within the function.
        /// Safe to call from other threads.
        void runInLoop(const Functor &cb);

        /// Queues callback in the loop thread.
        /// Runs after finish pooling.
        /// Safe to call from other threads.
        void queueInLoop(const Functor &cb);

        size_t queueSize() const;

        // timers

        ///
        /// Runs callback at 'time'.
        /// Safe to call from other threads.
        ///
        TimerId runAt(Timestamp time, const TimerCallback &cb);

        ///
        /// Runs callback after @c delay seconds.
        /// Safe to call from other threads.
        ///
        TimerId runAfter(double delay, const TimerCallback &cb);

        ///
        /// Runs callback every @c interval seconds.
        /// Safe to call from other threads.
        ///
        TimerId runEvery(double interval, const TimerCallback &cb);

        ///
        /// Cancels the timer.
        /// Safe to call from other threads.
        ///
        void cancel(TimerId timerId);

        // internal usage
        void wakeup() const;

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

        bool hasChannel(Channel *channel);

        // pid_t threadId() const { return threadId_; }
        void assertInLoopThread() {
            if (!isInLoopThread()) {
                abortNotInLoopThread();
            }
        }

        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

        // bool callingPendingFunctors() const { return callingPendingFunctors_; }
        bool eventHandling() const { return eventHandling_; }

        void setContext(const Any &context) { context_ = context; }

//        const boost::any &getContext() const { return context_; }

//        boost::any *getMutableContext() { return &context_; }

        static EventBase *getEventLoopOfCurrentThread();

    private:
        void abortNotInLoopThread();

        void handleRead() const;  // waked up
        void doPendingFunctors();

        void printActiveChannels() const; // DEBUG

        typedef std::vector<Channel *> ChannelList;

        std::atomic_bool looping_; /* atomic */
        std::atomic_bool quit_;
        std::atomic_bool eventHandling_; /* atomic */
        std::atomic_bool callingPendingFunctors_; /* atomic */
        int64_t iteration_;
        const pid_t threadId_;
        Timestamp pollReturnTime_;
        std::unique_ptr<Poller> poller_;
        std::unique_ptr<TimerQueue> timerQueue_;
        int wakeupFd_;
        // unlike in TimerQueue, which is an internal class,
        // we don't expose Channel to client.
        std::unique_ptr<Channel> wakeupChannel_;
        Any context_;

        // scratch variables
        ChannelList activeChannels_;
        Channel *currentActiveChannel_;

        mutable std::mutex mutex_;
        std::vector<Functor> pendingFunctors_;
    };
}


#endif //NET_EVENTBASE_H
