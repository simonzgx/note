//
// Created by Administrator on 2020/8/19.
//

#ifndef TRANS_SDK_NONCOPYABLE_H
#define TRANS_SDK_NONCOPYABLE_H


class noncopyable {
public:
    noncopyable(const noncopyable &that) = delete;

    noncopyable(noncopyable &&that) = delete;

    noncopyable &operator=(const noncopyable &that) = delete;

    noncopyable &operator=(noncopyable &&that) = delete;

protected:
    noncopyable() = default;

    ~noncopyable() = default;
};


#endif //TRANS_SDK_NONCOPYABLE_H
