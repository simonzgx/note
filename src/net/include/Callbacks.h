//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <functional>
#include <memory>

#include "Timestamp.h"
#include "platform.h"
// should really belong to base/Types.h, but <memory> is not included there.

template<typename T>
inline T *get_pointer(const std::shared_ptr<T> &ptr) {
    return ptr.get();
}

template<typename T>
inline T *get_pointer(const std::unique_ptr<T> &ptr) {
    return ptr.get();
}

// Adapted from google-protobuf stubs/common.h
// see License in muduo/base/Types.h
template<typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From> &f) {
#ifndef NDEBUG
    assert(f == NULL || dynamic_cast<To *>(get_pointer(f)) != NULL);
#endif
    return ::std::static_pointer_cast<To>(f);
}

namespace net {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
// All client visible callbacks go here.

    class Buffer;

    class TcpConnection;

    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TimerCallback = std::function<void()>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
    using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;

    using MessageCallback = std::function<void(const TcpConnectionPtr &,
                                               Buffer *,
                                               Timestamp)>;

}  // namespace net


#endif //NET_CALLBACKS_H
