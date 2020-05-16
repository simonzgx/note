#pragma once

#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "mysql_task.h"

#define MAX_TASK_NUM 1000
#define LOG_ERROR perror

class mysql_task_list {
public:
    mysql_task_list();

    ~mysql_task_list();

    bool push(mysql_task *poTask);

    mysql_task *pop();

private:
    uint16_t _read_index;
    uint16_t _write_index;
    mysql_task *_task_node[MAX_TASK_NUM] = {nullptr};
};

