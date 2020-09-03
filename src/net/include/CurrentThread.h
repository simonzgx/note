//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_CURRENTTHREAD_H
#define NET_CURRENTTHREAD_H

#include <thread>
#include <string>

#include <cstdlib>
#include <sys/syscall.h>
#include <unistd.h>
#include <chrono>


#include "Timestamp.h"

namespace net {
    namespace CurrentThread {
        using string = std::string;
        // internal
        extern __thread int t_cachedTid;
        extern __thread char t_tidString[32];
        extern __thread int t_tidStringLength;
        extern __thread const char *t_threadName;

        pid_t gettid();

        void cacheTid();

        inline int tid(){
            if (__builtin_expect(t_cachedTid == 0, 0)) {
                cacheTid();
            }
            return t_cachedTid;
        }

        inline const char *tidString();

        inline int tidStringLength();

        inline const char *name();

        bool isMainThread();

        void sleepUsec(int64_t usec);
    }

}  // namespace net



#endif //NET_CURRENTTHREAD_H
