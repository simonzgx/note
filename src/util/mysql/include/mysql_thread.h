//
// Created by Simon on 2020/5/16.
//

#ifndef MY_MYSQL_LIB_MYSQL_THREAD_H
#define MY_MYSQL_LIB_MYSQL_THREAD_H

#include <condition_variable>
#include <thread>
#include "mysql_task.h"
#include "mysql_task_list.h"


class mysql_thread {
public:
    mysql_thread();

    ~mysql_thread();

    void run();

protected:
    bool _init();
    void _mainLoop();
    void _unit();

private:
    bool _is_stop{false};
    std::shared_ptr<std::thread> _worker;
    bool _is_start{false};
    CDatabaseMysql *conn;
    mysql_task_list _task;
    mysql_task_list _reply_task;

    std::mutex _mutex;
    std::condition_variable _cond;
};


#endif //MY_MYSQL_LIB_MYSQL_THREAD_H
