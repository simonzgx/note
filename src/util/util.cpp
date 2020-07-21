//
// Created by simon on 2020/4/27.
//

#include "util.h"
#include <locale>
#include <codecvt>

void util::HexOutput(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = buf[i]; // must use unsigned char to print >128 value
        if (c < 16) {
            printf("0%x", c);
        } else {
            printf("%x", c);
        }
    }
    printf("\n");
}

void util::MakeRandStr(int sz, std::string &ret, bool printable) {
    ret.resize(sz);
    static const char CCH[] =
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i, x;
    std::srand(std::time(nullptr));
    if (printable) {
        for (i = 0; i < sz; ++i) {
            x = std::rand() % (sizeof(CCH) - 1);
            ret[i] = CCH[x];
        }
        return;
    }
    for (i = 0; i < sz; i++) {
        ret[i] = std::rand() % 0xFF;
    }
}


std::string util::GetCurrentProcessExeDir() {
    char current_proc_path[512] = {0};
#ifdef __MSVCRT__
    ::GetModuleFileNameA(nullptr, current_proc_path, 512);
#else
    int ret =  readlink("/proc/self/exe", current_proc_path, sizeof(current_proc_path)-1 );
#endif
    std::string current_proc_dir;
    current_proc_dir.append(current_proc_path);
    int separator_pos = current_proc_dir.rfind('/');

    if (std::string::npos == separator_pos) {
        current_proc_dir = "";
    } else {
        current_proc_dir = current_proc_dir.substr(0, separator_pos);
    }
    return current_proc_dir;
}

std::wstring util::String2WideString(const std::string &str) {
    if (str.empty()) {
        return L"";
    }
    unsigned len = str.size() + 1;
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
    mbstowcs(p.get(), str.c_str(), len);
    std::wstring w_str(p.get());
    return w_str;
}

std::string util::WideString2String(const std::wstring &w_str) {
    if (w_str.empty()) {
        return "";
    }
    unsigned len = w_str.size() * 4 + 1;
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<char[]> p(new char[len]);
    wcstombs(p.get(), w_str.c_str(), len);
    std::string str(p.get());
    return str;
}

std::string util::WideString2Utf8(const std::wstring &src) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.to_bytes(src);
}

int util::UTF82GBK(char *gbkStr, const char *srcStr, int maxGbkStrlen) {
    if (nullptr == srcStr) {
        printf("Bad Parameter\n");
        return -1;
    }
    //首先先将utf8编码转换为unicode编码
    if (nullptr == setlocale(LC_ALL, "zh_CN.utf8")) //设置转换为unicode前的码,当前为utf8编码
    {
        printf("Bad Parameter\n");
        return -1;
    }
    int unicodeLen = mbstowcs(nullptr, srcStr, 0);
    //计算转换后的长度
    if (unicodeLen <= 0) {
        printf("Can not Transfer!!!\n");
        return -1;
    }
    auto *unicodeStr = (wchar_t *) calloc(sizeof(wchar_t), unicodeLen + 1);
    mbstowcs(unicodeStr, srcStr, strlen(srcStr));
    //将utf8转换为unicode
    //将unicode编码转换为gbk编码
    if (nullptr == setlocale(LC_ALL, "zh_CN.gbk")) //设置unicode转换后的码,当前为gbk
    {
        printf("Bad Parameter\n");
        return -1;
    }
    int gbkLen = wcstombs(nullptr, unicodeStr, 0);
    //计算转换后的长度
    if (gbkLen <= 0) {
        printf("Can not Transfer!!!\n");
        return -1;
    } else if (gbkLen >= maxGbkStrlen) //判断空间是否足够
    {
        printf("Dst Str memory not enough\n");
        return -1;
    }
    wcstombs(gbkStr, unicodeStr, gbkLen);
    gbkStr[gbkLen] = 0; //添加结束符
    free(unicodeStr);
    return gbkLen;
}


int util::GBK2UTF8(char *utfstr, const char *srcstr, int maxutfstrlen) {
    if (nullptr == srcstr) {
        printf(" bad parameter\n");
        return -1;
    }
    //首先先将gbk编码转换为unicode编码
    if (nullptr == setlocale(LC_ALL, "zh_cn.gbk"))//设置转换为unicode前的码,当前为gbk编码
    {
        printf("setlocale bad parameter\n");
        return -1;
    }
    int unicodelen = mbstowcs(nullptr, srcstr, 0);//计算转换后的长度
    if (unicodelen <= 0) {
        printf("can not transfer!!!\n");
        return -1;
    }
    auto *unicodestr = (wchar_t *) calloc(sizeof(wchar_t), unicodelen + 1);
    mbstowcs(unicodestr, srcstr, strlen(srcstr));//将gbk转换为unicode


    //将unicode编码转换为utf8编码
    if (nullptr == setlocale(LC_ALL, "zh_cn.utf8"))//设置unicode转换后的码,当前为utf8
    {
        printf("bad parameter\n");
        return -1;
    }
    int utflen = wcstombs(nullptr, unicodestr, 0);//计算转换后的长度
    if (utflen <= 0) {
        printf("can not transfer!!!\n");
        return -1;
    } else if (utflen >= maxutfstrlen)//判断空间是否足够
    {
        printf("dst str memory not enough\n");
        return -1;
    }
    wcstombs(utfstr, unicodestr, utflen);
    utfstr[utflen] = 0;//添加结束符
    free(unicodestr);
    return utflen;
}
