#pragma once

#include <memory>
#include <ucontext.h>
#include <functional>
#include <atomic>
#include "log.h"
#include "thread.h"
#include "config.h"
#include "macro.h"

namespace atpdxy
{
class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    
    // 协程的状态
    enum State
    {
        // 初始化
        INIT,   
        // 暂停
        HOLD,
        // 执行
        EXEC,
        // 结束
        TERM,
        // 可执行
        READY,
        // 异常
        EXCEPT
    };

private:
    // 主协程的构造函数
    Fiber();
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    
    ~Fiber();

    // 重置协程上的函数（INIT,TERM）
    void reset(std::function<void()> cb);

    // 将当前协程切入CPU执行
    void swapIn();

    // 将当前协程从CPU切出
    void swapOut();

public:
    // 返回正在执行的协程
    static Fiber::ptr GetThis();

    // 协程切换到后台，设置为Ready状态
    static void YieldToReady();

    // 协程切换到后台，设置为Hold状态
    static void YieldToHold();

    // 返回总协程数
    static uint64_t TotalFibers();

    // 
    static void MainFunc();

    // 为正在执行协程变量赋值
    static void SetThis(Fiber* f);

    uint64_t getId() const { return m_id; }

    // 返回当前正在执行协程的id
    static uint64_t GetFiberId();
private:
    // 协程id
    uint64_t m_id = 0;

    // 协程栈空间大小
    uint64_t m_stacksize = 0;

    // 协程的状态
    State m_state = INIT;

    // 存储协程信息结构体
    ucontext_t m_ctx;

    // 协程栈指针
    void* m_stack = nullptr;

    // 协程上的执行函数
    std::function<void()> m_cb;
};
}