//
// Created by Administrator on 2020/7/3.
//

#ifndef UTIL_OUTPUT_H
#define UTIL_OUTPUT_H

#include <string>
#include <vector>
#include <cstddef>
#include <cassert>
#include <cstring>

namespace util {

    static const int SMALL_STACK_BUF_SIZE = 1 << 12; // 4K
    static const int MEDIAN_STACK_BUF_SIZE = 1 << 14; // 16K
    static const int LARGE_STACK_BUF_SIZE = 1 << 16; // 64K

    static const int SMALL_HEAP_BUF_SIZE = 1 << 15; // 32K
    static const int MEDIAN_HEAP_BUF_SIZE = 1 << 16; // 64K
    static const int LARGE_HEAP_BUF_SIZE = 1 << 18; // 256K

    class OutputBuffer {
    public:
        virtual ~OutputBuffer() = default;

        // Get the buffer at the specified offset.
        virtual uint8_t *buffer(size_t offset) = 0;

        virtual void alloc(size_t new_size) = 0;

        // The number of bytes could be written at current offset.
        virtual size_t writable() const = 0;

        virtual size_t capacity() const = 0;

        // Make sure there's enough space to put size bytes.
        // Basically, append() will allocate memory when necessary.
        // However, if we pass this buffer to a function which expects char*,
        // we may need to ensure the size.
        void reserve(size_t new_size) {
            if (capacity() < new_size) {
                alloc(round_up(new_size));
            }
        }

        void resize(size_t new_size) {
            if (new_size > size() + writable()) {
                reserve(new_size);
            }

            offset_ = new_size;
        }

        void reserve_append(size_t another) { return reserve(size() + another); }

        uint8_t &operator[](size_t index) { return buffer(0)[index]; }

        uint8_t *begin() { return buffer(0); }

        uint8_t *end() { return buffer(size()); }

        size_t size() const { return offset_; }

        // Forward the offset. When we need to use some C functions, this
        // help us make the offset correct.
        virtual void forward(size_t size) { offset_ += size; }

        // Basically, we only change the offset by append. But if we want to
        // clear some data, we may need to set the offset and we can only
        // set backwards(an offset less than the current one).
        virtual void reset(size_t offset = 0) {
            offset_ = offset;
        }

        template<typename Type>
        std::enable_if_t<std::is_pod<Type>::value &&
                         !std::is_pointer<Type>::value>
        write_at(const Type &x, size_t offset) {
            assert(offset + sizeof(x) <= capacity());

            ::memcpy(buffer(offset), &x, sizeof(x));
        }

        void append(const uint8_t *data, size_t len) {
            reserve_append(len);

            ::memcpy(buffer(offset_), data, len);

            offset_ += len;
        }

        void append(const char *data, size_t len) {
            return append(reinterpret_cast<const uint8_t *>(data), len);
        }

        template<typename Type>
        std::enable_if_t<std::is_pod<Type>::value &&
                         !std::is_pointer<Type>::value>
        append(Type x) {
            append(reinterpret_cast<const uint8_t *>(&x), sizeof(Type));
        }

        template<typename ElementType, size_t Size>
        std::enable_if_t<std::is_pod<ElementType>::value>
        append(const ElementType (&x)[Size]) {
            size_t needed = Size * sizeof(ElementType);
            reserve_append(needed);

            append(reinterpret_cast<const uint8_t *>(x), needed);
        }

        template<typename SizeType, typename E>
        std::enable_if_t<std::is_pod<E>::value>
        append(const std::vector<E> &v) {
            size_t needed = sizeof(SizeType) + v.size() * sizeof(E);
            reserve_append(needed);

            append(static_cast<SizeType>(v.size()));
            append(reinterpret_cast<const uint8_t *>(&*v.begin()), v.size() * sizeof(E));
        }

        template<typename SizeType, typename E,
                template<typename T> class Container>
        std::enable_if_t<!std::is_pod<E>::value>
        append(const Container<E> &v) {
            append(static_cast<SizeType>(v.size()));
            for (const auto &elem : v) {
                elem.to_stream(*this);
            }
        }

        // All containers whose memory are not continuous
        template<typename SizeType, typename ElementType,
                template<typename T> class Container>
        void append(const Container<ElementType> &container) {
            size_t needed = sizeof(SizeType) + sizeof(ElementType) * container.size();
            reserve_append(needed);

            append(static_cast<SizeType>(container.size()));
            for (size_t i = 0; i < container.size(); i++) {
                append(container[i]);
            }
        }

        template<typename SizeType, typename ElementType>
        void append_with_size(const ElementType *data, SizeType size) {
            size_t data_size = sizeof(ElementType) * size;
            size_t needed = sizeof(SizeType) + data_size;
            reserve_append(needed);

            append(static_cast<SizeType>(size));
            if (size) {
                ::memcpy(buffer(offset_), data, data_size);
                offset_ += data_size;
            }
        }

        // Append a string with its length as _SizeType.
        template<typename SizeType>
        void append_string(const char *s) {
            size_t len = strlen(s);
            size_t needed = sizeof(SizeType) + len;
            reserve_append(needed);

            append<SizeType>(len);
            if (len) {
                append(reinterpret_cast<const uint8_t *>(s), len);
            }
        }

        template<typename SizeType>
        void append_string(const std::string &s) {
            size_t needed = sizeof(SizeType) + s.size();
            reserve_append(needed);

            append<SizeType>(s.size());
            if (!s.empty()) {
                append(reinterpret_cast<const uint8_t *>(&*s.begin()), s.size());
            }
        }

        // Append a string with fixed length. If the string length is less than
        // len, '\0' will be padded.
        void append_fixed_len_string(const char *str, size_t len) {
            reserve_append(len);

            size_t n = strlen(str);
            if (n < len) {
                ::memcpy(buffer(offset_), str, n);
                ::memset(buffer(offset_ + n), 0, len - n);
            } else {
                ::memcpy(buffer(offset_), str, len);
            }
            offset_ += len;
        }

        void append_fixed_len_string(const std::string &str, size_t len) {
            reserve_append(len);

            size_t n = str.size();
            if (n < len) {
                ::memcpy(buffer(offset_), str.c_str(), n);
                ::memset(buffer(offset_ + n), 0, len - n);
            } else {
                ::memcpy(buffer(offset_), str.c_str(), len);
            }

            offset_ += len;
        }

    protected:
        OutputBuffer()
                : offset_(0) {
        }

        static unsigned int round_up(unsigned int v) {
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;
            return v;
        }

        mutable size_t offset_;
    };

// A buffer used on the stack. If the size is less than _BufSize, no heap
// memory is allocated. Otherwise, heap memory is used.
// If it's obvious that lots of bytes will be written, use HeapOutputBuffer
// directly.
// Use the stack buffer with care because of stack overflow.
    template<size_t BufSize = SMALL_STACK_BUF_SIZE>
    class StackOutputBuffer : public OutputBuffer {
    public:
        StackOutputBuffer()
                : buffer_(local_buffer_),
                  capacity_(sizeof(local_buffer_)) {
            assert(BufSize <= (size_t) LARGE_STACK_BUF_SIZE);
        }

        ~StackOutputBuffer() override {
            if (!is_local_buffer()) {
                delete[] buffer_;
            }
        }

        void alloc(size_t new_size) override {
            uint8_t *old_buffer = buffer_;
            buffer_ = new uint8_t[new_size];
            capacity_ = new_size;
            memcpy(buffer_, old_buffer, size());
            if (old_buffer != local_buffer_) {
                delete[] old_buffer;
            }
        }

        size_t writable() const override { return capacity_ - offset_; }

        size_t capacity() const override { return capacity_; }

        uint8_t *buffer(size_t size = 0) { return buffer_ + size; }

    private:
        bool is_local_buffer() const { return buffer_ == local_buffer_; }

        uint8_t local_buffer_[BufSize]{};
        uint8_t *buffer_;
        mutable size_t capacity_;
    };

    class HeapOutputBuffer : public OutputBuffer {
    public:
        explicit HeapOutputBuffer(size_t init_size = SMALL_HEAP_BUF_SIZE) {
            if (init_size) {
                buffer_.resize(init_size);
            }
        }

        // Allocate is actually resize the underlying buffer.
        void alloc(size_t new_size) {
            buffer_.resize(new_size);
        }

        // Cannot use buffer_.writable() here since we use the vector as a char
        // buffer and the size().
        size_t writable() const override { return buffer_.capacity() - offset_; }

        size_t capacity() const override { return buffer_.capacity(); }

        uint8_t *buffer(size_t offset = 0) { return &*(buffer_.begin() + offset); }

    private:
        std::vector<uint8_t> buffer_;
    };

    template<class _ResponseHeader, class BufferType>
    class BaseOutputPacket : public BufferType {
    public:
        BaseOutputPacket() {
            // The buffer cannot be that small!
            assert(this->writable() >= sizeof(_ResponseHeader));
            memset(this->buffer(0), 0, sizeof(_ResponseHeader));
            this->forward(sizeof(_ResponseHeader));
        }

        virtual void reset(size_t offset = 0) {
            assert(offset >= sizeof(_ResponseHeader) || offset == 0);
            if (offset == 0) {
                memset(this->buffer(0), 0, sizeof(_ResponseHeader));
                this->offset_ = sizeof(_ResponseHeader);
            } else {
                this->offset_ = offset;
            }
        }

        virtual void clear_body() {
            this->offset_ = sizeof(_ResponseHeader);
            header()->SetPkgSize(0);
        }

        template<typename T>
        explicit BaseOutputPacket(const T &t) {
            // The buffer cannot be that small!
            assert(this->writable() >= sizeof(_ResponseHeader));
            new(this->buffer(0)) _ResponseHeader(t);
            this->forward(sizeof(_ResponseHeader));
        }

        _ResponseHeader *header() { return reinterpret_cast<_ResponseHeader *>(this->buffer(0)); }

        uint8_t *data() { return this->buffer(sizeof(_ResponseHeader)); }

        size_t data_size() const { return this->size() - sizeof(_ResponseHeader); }

        // Call this or rebuild_header() to update the package size in the header
        // before sending the packet out.
        void update_pkgsize() {
            header()->SetPkgSize(this->size() - sizeof(_ResponseHeader));
        }

        template<typename T>
        void rebuild_header(const T &t) {
            header()->~_ResponseHeader();
            new(this->buffer(0)) _ResponseHeader(t);
            update_pkgsize();
        }
    };

    template<class ResponseHeader, size_t BufSize = SMALL_STACK_BUF_SIZE>
    class StackOutputPacket : public BaseOutputPacket<ResponseHeader,
            StackOutputBuffer<BufSize> > {
    public:
        StackOutputPacket() = default;

        template<typename T>
        explicit StackOutputPacket(const T &t)
                : BaseOutputPacket<ResponseHeader, StackOutputBuffer<BufSize> >(t) {
        }
    };

    template<class ResponseHeader>
    class HeapOutputPacket : public BaseOutputPacket<ResponseHeader, HeapOutputBuffer> {
    public:
        HeapOutputPacket() = default;

        template<typename T>
        explicit HeapOutputPacket(const T &t)
                : BaseOutputPacket<ResponseHeader, HeapOutputBuffer>(t) {
        }
    };

    typedef StackOutputBuffer<SMALL_STACK_BUF_SIZE> SmallStackBuffer;
    typedef StackOutputBuffer<MEDIAN_STACK_BUF_SIZE> MedianStackBuffer;
    typedef StackOutputBuffer<LARGE_STACK_BUF_SIZE> LargeStackBuffer;

}//namespace util

#endif //UTIL_OUTPUT_H
