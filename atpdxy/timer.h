#pragma once

#include <memory>
#include <set>
#include <vector>
#include "thread.h"

namespace atpdxy {
// 向前声明定时器管理类
class TimerManager;

// 定时器
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    // 取消当前定时器
    bool cancel();

    // 刷新定时器执行时间
    bool refresh();

    // 重置定时器间隔，是否从当前时间开始
    bool reset(uint64_t ms, bool from_now);
private:
    // 构造函数，指定定时器的循环周期，回调函数，是否循环执行，所在的管理器
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

    Timer(uint64_t next);
private:
    // 比较函数
    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
private:
    bool m_recurring = false;                   // 是否循环执行定时器
    uint64_t m_ms = 0;                          // 定时器的周期
    uint64_t m_next = 0;                        // 定时器下次执行的时间
    std::function<void()> m_cb;                 // 回调函数
    TimerManager* m_manager = nullptr;          // 定时器所在的管理器
};

// 定时器管理器
class TimerManager {
friend class Timer;    
public:
    typedef RWMutex RWMutexType;

    TimerManager();

    virtual ~TimerManager();

    // 添加定时器
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
    
    // 添加定时器加上环境依赖
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);

    // 到最近一个定时器的时间差
    uint64_t getNextTimer();

    // 返回所有已经超时的定时器的回调函数
    void listExpiredCb(std::vector<std::function<void()>>& cbs);

    // 是否有定时器
    bool hasTimer();
protected:
    // 有定时器被添加到容器头部
    virtual void onTimerInsertedAtFront() = 0;
    
    // 将定时器添加到管理器中
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);
private:
    // 检查系统时间是否被修改了
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutexType m_mutex;
    // 存放定时器的容器
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    // 是否触发了onTimerInsertedAtFront
    bool m_tickled = false;
    // 上次执行时间，用来更新系统时间是否被修改
    uint64_t m_previousTime = 0;
};
}