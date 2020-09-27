//
// Created by Administrator on 2020/9/27.
//

#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <future>
#include <iostream>
#include <thread>
#include <queue>
#include <exception>

#define LOCK_GUARD(mutex_) std::lock_guard<std::mutex> lk(mutex_);

template<typename T>
class Channel {
public:

    bool empty() {
        LOCK_GUARD(mutex_)
        return queue_.empty();
    }

    size_t size() {
        LOCK_GUARD(mutex_)
        return queue_.size();
    }

    bool try_pop(T &out) {
        LOCK_GUARD(mutex_)
        if (queue_.empty())return false;
        out = std::move(queue_.front());
        queue_.pop();
    }

    void push(T in) {
        LOCK_GUARD(mutex_)
        queue_.emplace(in);
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;

};

template<typename T>
class BufferedChannel {
public:

    bool is_open() {
        return open_ || !channel_.empty();
    }

    void drain() {
        open_.store(false);
    }


    void send(T val) {
        if (!open_) {
            throw std::logic_error("channel closed!");
        }
        channel_.push(std::move(val));
    }

    bool recv(T &val) {
        while (is_open()) {
            if (channel_.try_pop(val)) {
                return true;
            }
            std::this_thread::yield();
        }
        return false;
    }

private:
    Channel<T> channel_;
    std::atomic_bool open_{true};
};


#endif //NET_CHANNEL_H
