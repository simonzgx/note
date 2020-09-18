//
// Created by Administrator on 2020/8/19.
//
#include <cstdio>

#include "CurrentThread.h"
#ifdef __WINDOWS__
#include <windows.h>
#endif

using namespace net;

__thread int CurrentThread::t_cachedTid = 0;
__thread char CurrentThread::t_tidString[32];
__thread int CurrentThread::t_tidStringLength = 6;
__thread const char *CurrentThread::t_threadName = "unknown";

void CurrentThread::cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

pid_t CurrentThread::gettid() {
#ifdef linux
    return static_cast<pid_t>(::syscall(SYS_gettid));
#elif defined(__WINDOWS__)
    return static_cast<pid_t>(GetCurrentThreadId());
#endif
}

inline const char *CurrentThread::tidString() {
    return t_tidString;
}

inline int CurrentThread::tidStringLength() {
    return t_tidStringLength;
}

inline const char *CurrentThread::name() {
    return t_threadName;
}

bool CurrentThread::isMainThread() {
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec) {
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, nullptr); //fixme use c++11 chrono
}