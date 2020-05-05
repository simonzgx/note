//
// Created by simon on 2020/4/29.
//

#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include <sys/time.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <memory>
#include <mutex>
#include <list>
#include "util.h"

namespace util {
    typedef enum {
        LTrace = 0, Debug, Info, Warning, Error
    } LogLevel;


    class LogContext : public std::ostringstream {
    public:
        LogContext(LogLevel level, const char *file,
                   const char *function,
                   int line
        );

        ~LogContext() override = default;

        LogLevel level;
        int line;
        std::string file;
        std::string function;
        struct timeval tv{};
    };

    using LogContextPtr = std::shared_ptr<LogContext>;

    class Writer{

    };

    class Logger : public std::enable_shared_from_this<Logger>, public noncopyable {
    public:
        void write(const LogContextPtr &ctx);
    };

    class LogContextCapture {
    public:
        LogContextCapture(Logger &logger, LogLevel level, const char *file, const char *function, int line);

        ~LogContextCapture() = default;

        LogContextCapture &operator<<(std::ostream &(*f)(std::ostream &));

        template<typename T>
        LogContextCapture &operator<<(T &&data);

    private:
        LogContextPtr _ctx;
        Logger &_logger;
    };

    using CapturePtr = std::shared_ptr<LogContextCapture>;
}


#endif //TEST_LOGGER_H
