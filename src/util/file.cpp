//
// Created by simon on 2020/4/27.
//

#include "file.h"

bool util::file::create_path(const char *file, unsigned int mod) {
    std::string path = file;
    std::string dir;
    int index = 1;
    while (1) {
        index = path.find('/', index) + 1;
        dir = path.substr(0, index);
        if (dir.length() == 0) {
            break;
        }
        if (access(dir.c_str(), 0) == -1) { //access函数是查看是不是存在
            if (mkdir(dir.c_str(), mod) == -1) {  //如果不存在就用mkdir函数来创建
                WarnL << dir << ":" << get_uv_errmsg();
                return false;
            }
        }
    }
    return true;
}
