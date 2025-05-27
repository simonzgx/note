//
// Created by simon on 2020/4/11.
//

#ifndef RW_LOCK_RING_BUFFER_H
#define RW_LOCK_RING_BUFFER_H

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <algorithm>
#include <sys/types.h>


template<class Tp>
class ring_buffer {
    static_assert(std::is_pod<Tp>::value, "only support POD type");
public:
    explicit ring_buffer(size_t size) : _size(size) {
        _buf = static_cast<Tp *>(malloc(sizeof(Tp) * size));
    }

    ~ring_buffer() {
        free(_buf);
    }

    void skip(size_t size = 1) {
        if (size > _used) {
            _r_offset = _w_offset;
            _used = 0;
        } else {
            _used -= size;
            _r_offset = (_r_offset + size) % _size;
        }
    }

    int read(Tp *out, size_t size = 1) {
        size_t read_size = std::min(size, _used);
        int to_end = std::min(read_size, _size - _r_offset);
        memcpy(out, _buf[_r_offset], to_end);
        if (to_end < read_size) {
            memcpy(out + to_end, _buf, read_size - to_end);
        }
        _used -= read_size;
        _r_offset = (_r_offset + read_size) % _size;
        return read_size;
    }

    int write(const Tp *value, size_t size = 1) {
        if (!value || !size)return 0;
        size_t write_size = std::min(_size - _used, size);
        size_t to_end = std::min(write_size, _size - _w_offset);
        memcpy(_buf[_w_offset], value, to_end);
        if (to_end < write_size) {
            memcpy(_buf, value, write_size - to_end);
        }
        _used += write_size;
        _w_offset = (_w_offset + write_size) % _size;
        return write_size;
    }

    size_t capacity() {
        return _size;
    }

    size_t size() {
        return _used;
    }

    void clear() {
        _used = 0;
        _r_offset = _w_offset = 0;
    }

    void resize(size_t size) {
        Tp *new_buf = static_cast<Tp *>(malloc(sizeof(Tp) * size));
        memcpy(_buf, new_buf, _size);
        _size = size;
        _buf = new_buf;
    }

private:
    Tp *_buf;
    size_t _size;
    size_t _used{0};
    int _r_offset{0};
    int _w_offset{0};
};


#endif //RW_LOCK_RING_BUFFER_H
