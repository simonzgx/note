//
// Created by simon on 2020/4/27.
//

#include "util.h"


void util::makeRandStr(int sz, std::string &ret, bool printable) {
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