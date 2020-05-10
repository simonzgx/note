//
// Created by Simon on 2020/5/10.
//

#ifndef QUOTE_MONITOR_BASEDATA_H
#define QUOTE_MONITOR_BASEDATA_H

#include <string>

class Node;
class MainEngine;

class BaseData {
public:
    virtual void init(Node &node) = 0;
    virtual void register_self(MainEngine&) = 0;
};


#endif //QUOTE_MONITOR_BASEDATA_H
