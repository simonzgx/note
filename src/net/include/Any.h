//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_ANY_H
#define NET_ANY_H

#include <memory>

namespace net {
    class Any {
    public:
        using AnyPtr = std::shared_ptr<Any>;

        Any() = default;

        ~Any() = default;

        template<typename C, typename ...ArgsType>
        void set(ArgsType &&...args) {
            _data.reset(new C(std::forward<ArgsType>(args)...), [](void *ptr) {
                delete reinterpret_cast<C *>(ptr);
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
}


#endif //NET_ANY_H
