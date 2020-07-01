//
// Created by Administrator on 2020/7/1.
//

#ifndef TEST_TIMERMANAGER_H
#define TEST_TIMERMANAGER_H


#include <stdint.h>
#include <list>

#include <stdint.h>
#include <map>

#include "Timer.h"

struct TimerCompare
{
    bool operator () (const Timer* lhs, const Timer* rhs)
    {
        return lhs->getExpiredTime() <  rhs->getExpiredTime();
    }
};

void defaultTimerCallback()
{

}

class TimerManager
{
public:
    TimerManager() = default;
    ~TimerManager() = default;

    /** 添加定时器
     * @param repeatedCount 重复次数
     * @param interval      触发间隔
     * @param timerCallback 定时器回调函数
     * @return              返回创建成功的定时器id
     */
    int64_t addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback);

    /** 移除指定id的定时器
     * @param timerId 待移除的定时器id
     * @return 成功移除定时器返回true，反之返回false
     */
    bool removeTimer(int64_t timerId);

    /** 检测定时器是否到期，如果到期则触发定时器函数
     */
    void checkAndHandleTimers();


private:
    //key是定时器id，value是定时器对象，注意模板的第三个参数是自定义排序对象TimerCompare
    std::map<int64_t, Timer*, TimerCompare>   m_mapTimers;
};

#endif //TEST_TIMERMANAGER_H
