//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_TIMER_H
#define NET_TIMER_H

#include <set>
#include <utility>
#include <vector>

#include <cstdint>
#include <cassert>
#include <cstring>
#include <atomic>

#include <unistd.h>

#include "noncopyable.h"
#include "Timestamp.h"
#include "logger.h"
#include "Callbacks.h"
#include "Channel.h"
#include "EventBase.h"
#include "platform.h"
#include <map>

#ifdef linux

#include <sys/timerfd.h>

#elif defined(__WINDOWS__)

#include <windows.h>
#include <queue>
#include "EventLoopThreadPool.h"

#endif

namespace net {

    using AtomicInt32 = std::atomic<int32_t>;
    using AtomicInt64 = std::atomic<int64_t>;

    class Timer : public noncopyable {

    public:
        Timer(TimerCallback cb, Timestamp when, double interval)
                : callback_(std::move(cb)),
                  expiration_(when),
                  interval_(interval),
                  repeat_(interval > 0.0),
                  sequence_(s_numCreated_++) {}

        ~Timer();

        void run() const {
            callback_();
        }

        Timestamp expiration() const { return expiration_; }

        bool repeat() const { return repeat_; }

        int64_t sequence() const { return sequence_; }

        void restart(Timestamp now);

        static int64_t numCreated() { return s_numCreated_; }

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool repeat_;
        const int64_t sequence_{};

        static AtomicInt64 s_numCreated_;
    };

    using TimerPtr = std::shared_ptr<Timer>;

}

namespace net {

    class TimerId {

        friend class TimerQueue;

    public:
        TimerId() : timer_(nullptr),
                    sequence_(0) {
        }

        TimerId(Timer *timer, int64_t seq)
                : timer_(timer),
                  sequence_(seq) {
        }

        TimerId(TimerPtr timer, int64_t seq) : timer_(std::move(timer)),
                                               sequence_(seq) {
        }

#ifdef __WINDOWS__

        friend bool operator<(const TimerId &lh, const TimerId &rh) {
            if (lh.timer_->expiration() < rh.timer_->expiration()) {
                return true;
            } else if (lh.timer_->expiration() == rh.timer_->expiration()) {
                return lh.timer_->sequence() < rh.timer_->sequence();
            }
            return false;
        }

#endif
    private:
        std::shared_ptr<Timer> timer_;
        int64_t sequence_;
    };


    class TimerQueue {

    public:
        explicit TimerQueue(EventBase *loop);

        ~TimerQueue();

        ///
        /// Schedules the callback to be run at given time,
        /// repeats if @c interval > 0.0.
        ///
        /// Must be thread safe. Usually be called from other threads.
        TimerId addTimer(const TimerCallback &cb,
                         Timestamp when,
                         double interval);

        void cancel(TimerId timerId);

    private:

        using Entry = std::pair<Timestamp, TimerPtr>;
        using TimerList = std::set<Entry>;
#ifdef linux
        using ActiveTimer = std::pair<TimerPtr, int64_t>;
        using ActiveTimerSet = std::set<ActiveTimer>;
#elif defined(__WINDOWS__)
        using ActiveTimerSet = std::map<Timestamp, TimerPtr>;
#endif

        void addTimerInLoop(const TimerPtr& timer);

        void cancelInLoop(TimerId timerId);

        // called when timerfd alarms
        void handleRead();

        // move out all expired timers
        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        bool insert(const TimerPtr &);

        EventBase *loop_;
        const int timerfd_;
        Channel timerfdChannel_;
        // Timer list sorted by expiration
        TimerList timers_;

        // for cancel()
        ActiveTimerSet activeTimers_;
        std::atomic_bool callingExpiredTimers_; /* atomic */
        ActiveTimerSet cancelingTimers_;
#ifdef __WINDOWS__
        std::atomic_bool stop_;
        std::mutex lock_;
#endif
    };

}


#endif //NET_TIMER_H
