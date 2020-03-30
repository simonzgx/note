//
// Created by simon on 2020/3/29.
//
#include <atomic>
#include <mutex>


class spinlock {
    std::atomic_flag _lk{};
public:
    spinlock() { _lk.clear(); }

    inline void lock() { while (_lk.test_and_set(std::memory_order_acquire)); }

    inline void unlock() { _lk.clear(std::memory_order_acquire); }
};

class mutex {
private:
    std::mutex _lk;
public:
    inline void lock() { _lk.lock(); }

    inline void unlock() { _lk.unlock(); }
};
