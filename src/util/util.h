//
// Created by simon on 2020/4/27.
//

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <ctime>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>


namespace util {


    class noncopyable {
    public:
        noncopyable() = default;

        ~noncopyable() = default;

        noncopyable(const noncopyable &that) = delete;

        noncopyable(noncopyable &&that) = delete;

        noncopyable &operator=(const noncopyable &that) = delete;

        noncopyable &operator=(noncopyable &&that) = delete;
    };

    class Any {
    public:
        using AnyPtr=std::shared_ptr<Any>;

        Any() = default;

        ~Any() = default;

        template<typename C, typename ...ArgsType>
        void set(ArgsType &&...args) {
            _data.reset(new C(std::forward<ArgsType>(args)...), [](void *ptr) {
                delete (C *) ptr;
            });
        }

        template<typename C>
        C &get() {
            if (!_data) {
                return nullptr;
            }
            return static_cast<C *>(_data.get());
        }

        explicit operator bool() {
            return _data.operator bool();
        }

        static bool empty() {
            return !bool();
        }

    private:
        std::shared_ptr<void> _data;
    };

    //k-v map for any instance
    class AnyStorage : public std::unordered_map<std::string, Any> {
    public:
        AnyStorage() = default;

        ~AnyStorage() = default;

        typedef std::shared_ptr<AnyStorage> Ptr;
    };

    void makeRandStr(int sz, std::string& ret, bool printable = true);

    class util {

    };
}


#endif //TEST_UTIL_H
