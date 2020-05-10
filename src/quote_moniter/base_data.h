//
// Created by Simon on 2020/5/10.
//

#ifndef QUOTE_MONITOR_BASE_DATA_H
#define QUOTE_MONITOR_BASE_DATA_H

#include <string>
#include "def.h"


class base_data {
public:
    virtual void init(Node &node) = 0;
    //注册一些函数，比如check
    virtual void register_self(main_engine&) = 0;
};


#endif //QUOTE_MONITOR_BASE_DATA_H
