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
        using AnyPtr = std::shared_ptr<Any>;

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

    //得到一个随机字符串
    void MakeRandStr(int sz, std::string &ret, bool printable = true);

    //以16进制的格式输出一块内存
    void HexOutput(const char *buf, size_t len);

    //获取当前进程可执行文件的绝对目录
    std::string GetCurrentProcessExeDir();

    /*
     * DESCRIPTION:convert std::string to wide string
     * Input: str 待转换的字符串
     * Returns: utf8str
     * Err: range_error
     */
    //convert std::string to wide string
    std::wstring String2WideString(const std::string &str);

    /*
     * DESCRIPTION: 实现wide string 到string的转换
     * Input: w_str,待转换的字符串;
     * Returns: utf8str
     * err:range_error
     */
    std::string WideString2String(const std::wstring &w_str);

    /*
     * DESCRIPTION: 实现由wstromg 到utf8的转码
     * Input: src,待转换的字符串;
     * Returns: utf8str
     * err:range_error
    */
    std::string WideString2Utf8(const std::wstring &src);

    /*
     * DESCRIPTION: 实现由utf8编码到gbk编码的转换
     * Input: gbkStr,转换后的字符串;
     * srcStr,待转换的字符串;
     * maxGbkStrlen, gbkStr的最大长度
     * Output: gbkStr
     * Returns: -1,fail;>0,success
    */
    int UTF82GBK(char *gbkStr, const char *srcStr, int maxGbkStrlen);

    /*
     * Description: 实现由gbk编码到utf8编码的转换
     * Input: utfstr,转换后的字符串; srcstr,待转换的字符串; maxutfstrlen, utfstr的最大长度
     * Output: utfstr
     * Returns: -1,fail;>0,success
    */
    int GBK2UTF8(char *utfstr, const char *srcstr, int maxutfstrlen);

}


#endif //TEST_UTIL_H
