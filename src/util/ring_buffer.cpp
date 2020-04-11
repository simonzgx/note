//
// Created by simon on 2020/4/11.
//

#include "ring_buffer.h"

template<class _Tp>
ring_buffer<_Tp>::ring_buffer(size_t size): _size(size) {
    _buf = static_cast<_Tp *>(malloc(sizeof(_Tp) * size));
}

template<class _Tp>
void ring_buffer<_Tp>::skip(int size) {
    if (size > _used) {
        _r_offset = _w_offset;
        _used = 0;
    } else {
        _used -= size;
        _r_offset = (_r_offset + size) % _size;
    }
}

template<class _Tp>
int ring_buffer<_Tp>::read(_Tp *out, int size) {
    int read_size = size > _used ? _used : size;
    int to_end = std::min(read_size, _size - _r_offset);
    memcpy(out, _buf[_r_offset], to_end);
    if (to_end < read_size) {
        memcpy(out + to_end, _buf, read_size - to_end);
    }
    _used -= read_size;
    _r_offset = (_r_offset + read_size) % _size;
    return read_size;
}

template<class _Tp>
int ring_buffer<_Tp>::write(const _Tp *value, int size) {
    if (!value || !size)return 0;
    int write_size = std::min(_size - _used, size);
    int to_end = std::min(write_size, _size - _w_offset);
    memcpy(_buf[_w_offset], value, to_end);
    if (to_end < write_size) {
        memcpy(_buf, value, write_size - to_end);
    }
    _used += write_size;
    _w_offset = (_w_offset + write_size) % _size;
    return write_size;
}

template<class _Tp>
ring_buffer<_Tp>::~ring_buffer() {
    free(_buf);
}

