#include "timer.h"
#include "util.h"

namespace atpdxy {
bool Timer::Comparator::operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const {
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->m_next  < rhs->m_next) {
        return true;
    }
    if(rhs->m_next < lhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager):
    m_recurring(recurring),
    m_ms(ms),
    m_cb(cb),
    m_manager(manager) {
    m_next = GetCurrentMS() + m_ms;
}

TimerManager::TimerManager() {
    m_previousTime = GetCurrentMS();
}

TimerManager::~TimerManager() {

}

// 添加定时器
Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

// 添加定时器加上环境依赖
Timer::ptr TimerManager::addCondtionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}   

// 取消当前定时器
bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if(m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

// 刷新定时器执行时间
bool Timer::refresh() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    // 没有回调函数刷新执行时间没意义，则直接返回
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    // 先删除后插入是由于定时器需要重新排序
    m_manager->m_timers.erase(it);
    m_next = GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

// 重置定时器间隔，是否从当前时间开始
bool Timer::reset(uint64_t ms, bool from_now) {
    if(m_ms == ms && !from_now) {
        return true;
    }
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now) {
        start = GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

Timer::Timer(uint64_t next):
    m_next(next) {

}

// 到最近一个定时器的时间差
uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    // 定时器容器为空，返回最大值
    if(m_timers.empty()) {
        return ~0ull;
    }
    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    // 已经过了第一个定时器的超时时间
    if(now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

// 返回所有已经超时的定时器的回调函数
void TimerManager::listExpiredCb(std::vector<std::function<void()>>& cbs) {
    uint64_t now_ms = GetCurrentMS();
    // 存储已经超时的定时器
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if(m_timers.empty()) {
            return;
        }
    }
    RWMutexType::WriteLock lock(m_mutex);
    if(m_timers.empty()) {
        return;
    }
    // 检查系统时间是否被改变
    bool rollover = detectClockRollover(now_ms);
    // 最近的定时器都没有到达，没有定时器到期，直接返回
    if(!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }
    // 创建定时器，用于查找下次执行时间小于该定时器的定时器
    Timer::ptr now_timer(new Timer(now_ms));
    // 发生了时钟溢出，从结尾开始找，否则找到第一个大于等于now_timer的定时器
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    // 继续向下找，一直找到下一个即将执行的定时器
    while(it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    // 将过期的定时器插入到vector中
    expired.insert(expired.begin(), m_timers.begin(), it);
    // 从定时器管理器中删除已经过期的定时器
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());
    for(auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring) {
            // 需要循环执行，再次插入到定时器管理器中
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

// 是否有定时器
bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

// 将定时器添加到管理器中
void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock) {
    // 获取插入后的迭代器
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.end()) && !m_tickled;
    if(at_front) {
        m_tickled = true;
    }
    lock.unlock();
    if(at_front) {
        onTimerInsertedAtFront();
    }
}

// 检查系统时间是否被修改了
bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    // 如果当前时间比上次执行的一小时还小认为时间出现了溢出
    if(now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}
}