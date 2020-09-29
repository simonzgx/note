//
// Created by Administrator on 2020/9/28.
//

#ifndef EVENTFD_SPIN_LOCK_H
#define EVENTFD_SPIN_LOCK_H

#include <atomic>

#include "noncopyable.h"

class spin_lock : public noncopyable {
public:
    spin_lock() { atom_.clear(); }

    inline void lock() { while (atom_.test_and_set(std::memory_order_acquire)); }

    inline void unlock() { atom_.clear(std::memory_order_acquire); }

private:
    std::atomic_flag atom_{};
};

class spin_lock_guard : public noncopyable {
public:
    explicit spin_lock_guard(spin_lock &lock) : lock_(lock) {
        lock_.lock();
    }

    ~spin_lock_guard() {
        lock_.unlock();
    }

private:
    spin_lock &lock_;
};


#endif //EVENTFD_SPIN_LOCK_H
