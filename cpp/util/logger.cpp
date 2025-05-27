//
// Created by simon on 2020/4/29.
//

#include "logger.h"

util::LogContext::LogContext(LogLevel level, const char *file, const char *function, int line)
        : level(level), file(file), function(function), line(line) {
    gettimeofday(&tv, nullptr);
}

util::LogContextCapture::LogContextCapture(Logger &logger, LogLevel level, const char *file,
                                           const char *function, int line) :
        _logger(logger), _ctx(new LogContext(level, file, function, line)) {

}

template<typename T>
util::LogContextCapture &util::LogContextCapture::operator<<(T &&data) {
    if (!_ctx) {
        return *this;
    }
    (*_ctx) << std::forward<T>(data);
    return *this;
}

util::LogContextCapture &util::LogContextCapture::operator<<(std::ostream &(*f)(std::ostream &)) {
    if (!_ctx) {
        return *this;
    }
    _logger.write(_ctx);
    _ctx.reset();
    return *this;
}

void util::Logger::write(const LogContextPtr &ctx) {

}
