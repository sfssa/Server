#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "log.h"
#include "scheduler.h"
#include <atomic>

namespace atpdxy {

static Logger::ptr g_logger = GET_LOGGER_BY_NAME("system");

// 正在执行的协程id
static std::atomic<uint64_t> s_fiber_id {0};

// 协程总数
static std::atomic<uint64_t> s_fiber_count {0};

// 正在执行的协程指针
static thread_local Fiber* t_fiber = nullptr;

// 用来实现协程切换时的中转站的协程的智能指针
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

// 申请和释放栈空间，更方便更改开辟内存的方法
class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

// 返回正在执行的协程id
uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

// 主协程，当作协程切换的中转站
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        ASSERT_WITH_MSG(false, "getcontext");
    }

    ++s_fiber_count;

    DEBUG(g_logger) << "Fiber::Fiber main";
}

// 创建新的协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)) {
        ASSERT_WITH_MSG(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if(!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        ASSERT(!m_cb);
        ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total=" << s_fiber_count;
}

//重置协程函数，并重置状态
//INIT，TERM, EXCEPT
void Fiber::reset(std::function<void()> cb) {
    ASSERT(m_stack);
    ASSERT(m_state == TERM
            || m_state == EXCEPT
            || m_state == INIT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        ASSERT_WITH_MSG(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

// 通过交换中转协程的上下文和当前协程的上下文，将本协程切入执行
void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        ASSERT_WITH_MSG(false, "swapcontext");
    }
}

// 通过交换当前协程的上下文和中转协程的上下文，运行中转协程，停止本协程的运行
void Fiber::back() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        ASSERT_WITH_MSG(false, "swapcontext");
    }
}

// 交换调度器的调度协程和当前协程的上下文，实现切换到当前协程执行
void Fiber::swapIn() {
    SetThis(this);
    ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        ASSERT_WITH_MSG(false, "swapcontext");
    }
}

// 交换调度器的当前协程和调度协程的上下文，实现将本协程切换到后台
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        ASSERT_WITH_MSG(false, "swapcontext");
    }
}

// 设置正在执行的协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

// 返回正在执行的协程，如果没有设置当前正在执行的协程，返回中转协程
Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

// 协程切换到后台，并且设置为Ready状态
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

// 协程切换到后台，并且设置为Hold状态
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    ASSERT(cur->m_state == EXEC);
    cur->swapOut();
}

// 总协程数
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    }

    // 将智能指针reset后可以将计数减少1，从而释放资源，然后将协程切入后台执行
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    ASSERT_WITH_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    ASSERT_WITH_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

}