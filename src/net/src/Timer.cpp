//
// Created by Administrator on 2020/8/19.
//

#include "Timer.h"

using namespace net;

#define MEM_ZERO(ptr, size) memset(ptr, 0, size);

AtomicInt64 Timer::s_numCreated_;

struct timespec howMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.microSecondsSinceEpoch()
                           - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts{};
    ts.tv_sec = static_cast<time_t>(
            microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(
            (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

inline int createTimerfd() {
#ifdef linux
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        LOG_SYSFATAL("%s", "Failed in timerfd_create");
    }
    return timerfd;
#elif defined(__WINDOWS__)
    static int timerfd = 0;
    return timerfd++;
#endif
}

inline void resetTimerfd(int timerfd, Timestamp expiration) {
#ifdef linux
    // wake up loop by timerfd_settime()
    struct itimerspec newValue{};
    struct itimerspec oldValue{};
    MEM_ZERO(&newValue, sizeof newValue);
    MEM_ZERO(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        LOG_SYSFATAL("%s", "timerfd_settime()");
    }
#elif defined(__WINDOWS__)
    (void) timerfd;
    (void) expiration;
#endif
}

void Timer::restart(Timestamp now) {
    if (repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = Timestamp::invalid();
    }
}

Timer::~Timer() {
    std::cout << "destructor : " << this << std::endl;
}

void readTimerfd(int timerfd, Timestamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE("TimerQueue::handleRead() %lu at %s", howmany, now.toString().c_str());
    if (n != sizeof howmany) {
        LOG_ERROR("TimerQueue::handleRead() reads  %lu bytes instead of 8", n);
    }
}

TimerQueue::TimerQueue(EventBase *loop)
        : loop_(loop),
          timerfd_(createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          timers_(),
          callingExpiredTimers_(false) {

#ifdef linux
    timerfdChannel_.setReadCallback([this](Timestamp) -> void {
        this->handleRead();
    });
    // we are always reading the timerfd, we disarm it with timerfd_settime.
    // add timer fd into poller
    timerfdChannel_.enableReading();
#elif defined(__WINDOWS__)
    this->stop_ = false;
    //fixme do not expose [this] in constructor
    auto doLoop = [this]() -> void {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (this->stop_)
                break;
            Timestamp now = Timestamp::now();
            auto entries = this->getExpired(now);
            for (auto &entry: entries) {
                ThreadPool::instance().enqueue([entry]() -> void {
                    entry.second->run();
                });
                if (entry.second->repeat()) {
                    entry.second->restart(now);
                    this->insert(entry.second);
                }
            }

        }
    };
    ThreadPool::instance().enqueue(doLoop);
#endif
}

TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
#ifdef __WINDOWS__
    this->stop_ = true;
#endif
}

TimerId TimerQueue::addTimer(const TimerCallback &cb,
                             Timestamp when,
                             double interval) {
#ifdef linux
//    auto *timer = new Timer(cb, when, interval);
    auto timer = std::make_shared<Timer>(cb, when, interval);
    loop_->runInLoop(
            [this, timer] { addTimerInLoop(timer); });
    return {timer, timer->sequence()};
#elif defined(__WINDOWS__)
#endif
}

void TimerQueue::cancel(TimerId timerId) {
#ifdef linux
    loop_->runInLoop(
            [this, timerId] { cancelInLoop(timerId); });
#elif defined(__WINDOWS__)
#endif
}

void TimerQueue::addTimerInLoop(const TimerPtr& timer) {
#ifdef linux
    loop_->assertInLoopThread();
    bool earliestChanged = this->insert(timer);

    if (earliestChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }
#elif defined(__WINDOWS__)

#endif
}

void TimerQueue::cancelInLoop(TimerId timerId) {
#ifdef linux
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1);
        (void) n;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
#elif defined(__WINDOWS__)
#endif
}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    // safe to callback outside critical section
    for (const Entry &it : expired) {
        it.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
#ifdef linux
    assert(timers_.size() == activeTimers_.size());
    Entry sentry(now, TimerPtr());
    auto end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for (const Entry &it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        //fixme To improve the efficiency of erase here
        auto eraseItem = activeTimers_.begin();
        for (; eraseItem != activeTimers_.end(); ++eraseItem) {
            if (eraseItem->first->sequence() == it.second->sequence()) {
                activeTimers_.erase(eraseItem);
                break;
            }
        }
    }

    assert(timers_.size() == activeTimers_.size());
#elif defined(__WINDOWS__)
    {
        std::lock_guard<std::mutex> lock(this->lock_);
        auto end = this->activeTimers_.lower_bound(now);
        for (auto it = this->activeTimers_.begin(); it != end; ++it) {
            expired.emplace_back(Entry{it->first, it->second});
        }
        this->activeTimers_.erase(this->activeTimers_.begin(), end);
    }

#endif
    return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {
#ifdef linux
    Timestamp nextExpire;
    for (const Entry &it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat()
            && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            it.second->restart(now);
            insert(it.second);
        }
    }

    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        resetTimerfd(timerfd_, nextExpire);
    }
#elif defined(__WINDOWS__)
#endif
}

bool TimerQueue::insert(const TimerPtr &timer) {
#ifdef linux
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    auto it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result
                = timers_.insert(Entry(when, timer));
        assert(result.second);
        (void) result;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result
                = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
        (void) result;
    }

    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
#elif defined(__WINDOWS__)
    std::lock_guard<std::mutex> lk(this->lock_);
    this->activeTimers_.insert({timer->expiration(), timer});
#endif
}

