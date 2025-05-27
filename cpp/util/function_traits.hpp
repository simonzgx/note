//
// Created by simon on 2020/4/22.
//

#ifndef INC_121_FUNCTION_TRAITS_H
#define INC_121_FUNCTION_TRAITS_H


#include <tuple>
using namespace std;

namespace util {

    template<typename T>
    struct function_traits;

    //普通函数
    template<typename Ret, typename... Args>
    struct function_traits<Ret(Args...)>
    {
    public:
        enum { arity = sizeof...(Args) };
        typedef Ret function_type(Args...);
        typedef Ret return_type;
        using stl_function_type = std::function<function_type>;
        typedef Ret(*pointer)(Args...);

        template<size_t I>
        struct args
        {
            static_assert(I < arity, "index is out of range, index must less than sizeof Args");
            using type = typename std::tuple_element<I, std::tuple<Args...> >::type;
        };
    };

    //函数指针
    template<typename Ret, typename... Args>
    struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>{};

    //std::function
    template <typename Ret, typename... Args>
    struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>{};

    //member function
#define FUNCTION_TRAITS(...) \
    template <typename ReturnType, typename ClassType, typename... Args>\
    struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{}; \

    FUNCTION_TRAITS()
    FUNCTION_TRAITS(const)
    FUNCTION_TRAITS(volatile)
    FUNCTION_TRAITS(const volatile)

    //函数对象
    template<typename Callable>
    struct function_traits : function_traits<decltype(&Callable::operator())>{};

} /* namespace util */


int main(){
    return 0;
}


#endif //INC_121_FUNCTION_TRAITS_H
