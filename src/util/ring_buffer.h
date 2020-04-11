//
// Created by simon on 2020/4/11.
//

#ifndef RW_LOCK_RING_BUFFER_H
#define RW_LOCK_RING_BUFFER_H

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <sys/types.h>


template<class _Tp>
class ring_buffer {

public:
    explicit ring_buffer(size_t size);

    ~ring_buffer();

    void skip(int size = 1);

    int read(_Tp *out, int size = 1);

    int write(const _Tp *value, int size = 1);

private:
    _Tp *_buf;
    size_t _size;
    size_t _used{0};
    int _r_offset{0};
    int _w_offset{0};


};


#endif //RW_LOCK_RING_BUFFER_H
