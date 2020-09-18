//
// Created by Administrator on 2020/8/20.
//

#ifndef TRANS_SDK_EVENTLOOPTHREADPOOL_H
#define TRANS_SDK_EVENTLOOPTHREADPOOL_H


#include <functional>
#include <memory>
#include <vector>
#include "noncopyable.h"
#include "platform.h"

#ifdef __WINDOWS__

#include <thread>
#include <queue>
#include <future>
#include <atomic>

#endif

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
        unsigned int numThreads_;
        unsigned long next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventBase *> loops_;
    };

#ifdef __WINDOWS__

    class ThreadPool : public noncopyable {
    public:

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> _lk(queue_mutex);
                stop = true;
            }
            condition.notify_all();
            for (auto &worker: workers) {
                worker.join();
            }
        }

        static ThreadPool &instance() {
            static ThreadPool instance;
            return instance;
        }

        template<class F, class... Args>
        auto enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<return_type()> >(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> _lk(queue_mutex);

                if (stop){
                    throw std::runtime_error("enqueue on stopped thread_poll");
                }

                tasks.emplace([task]() { (*task)(); });
            }
            condition.notify_one();
            return res;
        }

    private:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) : stop(false) {
            for (size_t i = 0; i < threads; ++i) {
                workers.emplace_back(
                        [this] ()->void {
                            for (;;) {
                                std::function<void()> task;

                                {
                                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                                    this->condition.wait(lock,
                                                         [this] { return this->stop || !this->tasks.empty(); });
                                    if (this->stop && this->tasks.empty())
                                        return;
                                    task = std::move(this->tasks.front());
                                    this->tasks.pop();
                                }

                                task();
                            }
                        }
                );
            }

        }

        // need to keep track of threads so we can join them
        std::vector<std::thread> workers;
        // the task queue
        std::queue<std::function<void()> > tasks;
        // synchronization
        std::mutex queue_mutex;
        static std::mutex lock;
        std::condition_variable condition;
        std::atomic_bool stop;
    };


#endif

}


#endif //TRANS_SDK_EVENTLOOPTHREADPOOL_H
