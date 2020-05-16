//
// Created by Simon on 2020/5/16.
//

#include "mysql_thread.h"

mysql_thread::mysql_thread() {

}

mysql_thread::~mysql_thread() {

}

void mysql_thread::run() {
    this->_mainLoop();
    this->_unit();

    if (nullptr != this->_worker) {
        this->_worker->join();
    }
}

bool mysql_thread::_init() {
    return false;
}

void mysql_thread::_mainLoop() {
    _is_start = true;

    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.notify_all();
    }

    IMysqlTask* poTask;

    while(!m_bTerminate)
    {
        if(NULL != (poTask = m_oTask.Pop()))
        {
            poTask->Execute(m_poConn);
            m_oReplyTask.Push(poTask);
            continue;
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void mysql_thread::_unit() {

}
