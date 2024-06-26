#pragma once

#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include "hook.h"
#include "fiber.h"
#include "thread.h"

namespace atpdxy {

// 协程调度器,封装的是N-M的协程调度器,内部有一个线程池,支持协程在线程池里面切换
class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    // 构造函数，指定线程数量和调度器名称
    // use_caller是否将创建调度器的线程纳入调度器
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");

    virtual ~Scheduler();

    // 返回协程调度器名称
    const std::string& getName() const { return m_name;}

    // 返回当前线程的协程调度器
    static Scheduler* GetThis();

    // 返回当前协程调度器的调度协程
    static Fiber* GetMainFiber();

    // 启动协程调度器
    void start();

    // 停止协程调度器
    void stop();

    // 停止协程调度器,协程执行的线程id,-1标识任意线程
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if(need_tickle) {
            tickle();
        }
    }

    // 批量调度协程
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end) {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if(need_tickle) {
            tickle();
        }
    }

    // 在不同线程间切换执行协程
    void switchTo(int thread = -1);

    // 打印调度器内部的信息
    std::ostream& dump(std::ostream& os);
protected:
    // 通知协程调度器有任务了
    virtual void tickle();

    // 协程调度函数
    void run();

    // 返回是否可以停止
    virtual bool stopping();

    // 协程无任务可调度时执行idle协程
    virtual void idle();

    // 设置当前线程的协程调度器
    void setThis();

    // 是否有空闲线程
    bool hasIdleThreads() { return m_idleThreadCount > 0;}
private:
    // 协程调度启动(无锁)
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    // 协程执行的任务
    struct FiberAndThread {
        // 协程
        Fiber::ptr fiber;
        // 协程执行函数
        std::function<void()> cb;
        // 线程id
        int thread;

        FiberAndThread(Fiber::ptr f, int thr)
            :fiber(f), thread(thr) {
        }

        FiberAndThread(Fiber::ptr* f, int thr)
            :thread(thr) {
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr)
            :cb(f), thread(thr) {
        }

        FiberAndThread(std::function<void()>* f, int thr)
            :thread(thr) {
            cb.swap(*f);
        }

        FiberAndThread()
            :thread(-1) {
        }

        // 清空任务的内容
        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
private:
    // Mutex
    MutexType m_mutex;
    // 线程池
    std::vector<Thread::ptr> m_threads;
    // 待执行的协程队列
    std::list<FiberAndThread> m_fibers;
    // use_caller为true时有效, 调度协程
    Fiber::ptr m_rootFiber;
    // 协程调度器名称
    std::string m_name;
protected:
    // 协程下的线程id数组
    std::vector<int> m_threadIds;
    // 线程数量
    size_t m_threadCount = 0;
    // 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    // 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    // 是否正在停止
    bool m_stopping = true;
    // 是否自动停止
    bool m_autoStop = false;
    // 主线程id(use_caller)
    int m_rootThread = 0;
};

// 调度器切换类
class SchedulerSwitcher : public Noncopyable {
public:
    SchedulerSwitcher(Scheduler* target = nullptr);
    ~SchedulerSwitcher();
private:
    Scheduler* m_caller;
};

}
