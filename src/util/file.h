//
// Created by simon on 2020/4/27.
//

#ifndef TEST_FILE_H
#define TEST_FILE_H

#include <string>
#include <sys/stat.h>
#include <string>
#include "File.h"
#include "Util/util.h"
#include "Util/logger.h"
#include "Util/uv_errno.h"
#include "Util/onceToken.h"

namespace util {
    class file {
    public:
        static bool create_path(const char *file, unsigned int mod);
    };
}


#endif //TEST_FILE_H
