//
// Created by Administrator on 2020/9/28.
//

#ifndef EVENTFD_CHANNEL_H
#define EVENTFD_CHANNEL_H

#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <memory>
#include <vector>
#include <condition_variable>

#define EVENT_SIGNAL uint64_t
#define CLOSE_SIGNAL nullptr

namespace gochan {

    class ChannelBus;

    class Channel;

    using ReadCallback = std::function<void(Channel *)>;

    class Channel {
    public:

        explicit Channel(ChannelBus *bus, size_t size = 0);

        ~Channel();

        void push(void *val) {
            std::unique_lock<std::mutex> lk(lock_);
            queue_.push(val);
            cv_.notify_one();
        }

        void *pop() {
            std::unique_lock<std::mutex> lk(lock_);
            while (queue_.empty()) {
                cv_.wait(lk);
            }
            void *ret = queue_.front();
            queue_.pop();
            return ret;
        }

        bool empty() {
            std::lock_guard<std::mutex> lk(lock_);
            return queue_.empty();
        }

        size_t size() {
            std::lock_guard<std::mutex> lk(lock_);
            return queue_.size();
        }

        int fd() const { return fd_; }

        int events() const { return events_; }

        void set_revents(int revt) { revents_ = revt; }

        void remove();

        void close() const;

        void registerMsgCallback(ReadCallback &cb) { eventCb_.emplace_back(cb); };

        void setCloseCallback(ReadCallback &cb) { closeCb_ = std::move(cb); }

        bool isNoneEvent() const { return events_ == kNoneEvent; }

        void handleEvent();

    private:

        void update();

    private:

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        std::queue<void *> queue_;
        std::mutex lock_;
        std::condition_variable cv_;
        ChannelBus *bus_;
        int fd_{};
        int events_{};
        int revents_{};
        std::vector<ReadCallback> eventCb_;
        ReadCallback closeCb_;
        size_t size_;
    };

}


#endif //EVENTFD_CHANNEL_H
