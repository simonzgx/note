//
// Created by Administrator on 2020/7/3.
//

#ifndef TEST_INPUT_H
#define TEST_INPUT_H


#include <string>
#include <cstring>
#include <cstddef>
#include <fstream>
#include <vector>
#include <exception>
#include "file.h"

namespace util {

    class OverReadException : public std::exception {
    public:
        OverReadException(int wanted_bytes, int offset, int has_bytes) {
            snprintf(buffer_, sizeof(buffer_), "try to read %d bytes at %d:%d",
                     wanted_bytes, offset, has_bytes);
        }

        const char *what() const noexcept override { return buffer_; }

    private:
        char buffer_[512]{};
    };

    class InputStream {
    public:
        InputStream()
                : offset_(0) {
        }

        virtual size_t size() const = 0;

        virtual const char *buffer() const = 0;

        virtual void skip(size_t nbytes) {
            check_read(nbytes);
            offset_ += nbytes;
        }

        virtual void read_buffer(char *buffer, size_t len) const {
            check_read(len);
            memcpy(buffer, this->buffer(), len);
            offset_ += len;
        }

        virtual size_t offset() const {
            return offset_;
        }

        virtual size_t readable() const {
            return size() - offset();
        }

        template<typename Type>
        Type read() const {
            Type x;
            read(x);
            return x;
        }

        template<typename SizeType>
        std::string read_string() const {
            std::string x;
            SizeType len = read<SizeType>();
            if (len && len <= readable()) {
                x.resize(len);
                read_buffer(&x[0], len);
            }
            return x;
        }

        // Almost the same as read an array but ensure the string is terminated
        // by a nil character.
        template<size_t N>
        void read_cstring(char (&x)[N]) const {
            read<char, N>(x);
            x[N - 1] = '\0';
        }

        template<typename Type>
        const InputStream &read(Type &x) const {
            check_read(sizeof(Type));
            read_buffer((char *) &x, sizeof(Type));
            return *this;
        }

        template<typename ElementType, size_t Size>
        const InputStream &read(ElementType (&x)[Size]) const {
            size_t needed = Size * sizeof(ElementType);
            check_read(needed);
            read_buffer((char *) &x[0], needed);
            return *this;
        }

        // Read into a std::string or std::vector<T>
        template<typename SizeType, typename ElementType,
                template<typename T> class Container>
        const InputStream &read(Container<ElementType> &container) const {
            SizeType size = read<SizeType>();
            size_t needed = sizeof(ElementType) * size;
            check_read(needed);
            container.resize(size);
            read_buffer((char *) &container[0], needed);
            return *this;
        }

    protected:
        void check_read(size_t nbytes) const {
            if (nbytes > readable()) {
                throw OverReadException(nbytes, offset(), size());
            }
        }

        mutable size_t offset_;
    };

    class InputBuffer : public InputStream {
    public:
        InputBuffer(const char *buffer, size_t size)
                : buffer_(buffer), size_(size) {
        }

        InputBuffer(const char *begin, const char *end)
                : buffer_(begin), size_(end - begin) {
        }


        size_t size() const override { return size_; }

        const char *buffer() const override {
            if (offset() > size()) {
                return nullptr;
            }
            return buffer_ + offset();
        }

    private:
        const char *buffer_;
        const size_t size_;
    };

    class InputFile : public InputStream {
    public:
        explicit InputFile(const std::string &path, size_t pos = 0)
                : fr_(path) {
            offset_ = pos;
        }

        size_t size() const override { return fr_.size(); }

        const char *buffer() const override {
            if (offset() > size()) {
                return nullptr;
            }
            return fr_.begin() + offset();
        }

    private:
        mutable FileReader fr_;
    };

}


#endif //TEST_INPUT_H
