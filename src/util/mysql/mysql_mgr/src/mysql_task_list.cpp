#include <sstream>
#include "mysql_task_list.h"


mysql_task_list::mysql_task_list() : _read_index(0), _write_index(0) {
}

mysql_task_list::~mysql_task_list() {
    for (auto &i : _task_node) {
        delete i;
    }
}

bool mysql_task_list::push(mysql_task *poTask) {
    auto next = static_cast<uint16_t>((_write_index + 1) % MAX_TASK_NUM);
    if (next == _read_index) {
        std::ostringstream ss;
        ss << "mysql task list full (read : " << _read_index
           << ", write : " << _write_index << ")\n";
        LOG_ERROR(ss.str().c_str());
        return false;
    }

    _task_node[_write_index] = poTask;
    _write_index = next;

    return true;
}

mysql_task *mysql_task_list::pop() {
    if (_write_index == _read_index) {
        return nullptr;
    }
    mysql_task *pTask = _task_node[_read_index];
    _read_index = static_cast<uint16_t>((_read_index + 1) % MAX_TASK_NUM);

    return pTask;
}
