#include "fiber.h"

namespace atpdxy
{
// 正在执行的协程id
static std::atomic<uint64_t> s_fiber_id = {0};

// 协程总数
static std::atomic<uint64_t> s_fiber_count = {0};

// 正在执行的协程的指针
static thread_local Fiber* t_fiber = nullptr;

// 正在执行的协程的智能指针，t_threadFiber是主协程，作为中转站
static thread_local Fiber::ptr t_threadFiber = nullptr; 

// 定义栈空间的配置
static Config<uint32_t>::ptr g_fiber_stack_size = ConfigManager::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

// 日志器
static Logger::ptr g_logger = LOG_NAME("system");

// 方便使用其他的开辟内存方法，届时只需要更改MallocStackAllocator类内部的函数即可
class MallocStackAllocator
{
public:
    static void* Alloc(size_t size)
    {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size)
    {
        return free(vp);
    }
};

using StackAlloctor = MallocStackAllocator;

Fiber::Fiber()
{
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx))
    {
        ATPDXY_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;
    LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    :m_id(++s_fiber_id)
    ,m_cb(cb)
{
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAlloctor::Alloc(m_stacksize);
    if(getcontext(&m_ctx))
    {
        ATPDXY_ASSERT2(false, "getcontest");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    // 到此为止，创建了上下文，但是没有开始执行
    LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if(m_stack)
    {
        ATPDXY_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT)
        {
            StackAlloctor::Dealloc(m_stack, m_stacksize);
        }
    }
    else
    {
        // 主协程没有栈，没有回调函数，一直执行，不会导致状态的修改
        ATPDXY_ASSERT(!m_cb);
        ATPDXY_ASSERT(m_state = EXEC);
        Fiber* cur = t_fiber;
        if(cur == this)
        {
            SetThis(nullptr);
        }
    }
    LOG_DEBUG(g_logger) << "Fiber::~Fiber id = " << m_id << " total = " << s_fiber_count;
}

void Fiber::SetThis(Fiber* f) 
{
    t_fiber = f;
}

void Fiber::reset(std::function<void()> cb)
{
    // 保证有栈空间
    ATPDXY_ASSERT(m_stack);
    ATPDXY_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
    m_cb = cb;
    if(getcontext(&m_ctx))
    {
        ATPDXY_ASSERT2(false, "getcontet");
    }
    // 协程执行完毕后下一个协程的上下文
    m_ctx.uc_link = nullptr;
    // 指定协程栈空间的起始地址
    m_ctx.uc_stack.ss_sp = m_stack;
    // 指定协程的栈空间大小
    m_ctx.uc_stack.ss_size = m_stacksize;
    // 制作上下文，MainFunc是协程的入口点
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::swapIn()
{
    SetThis(this);
    ATPDXY_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx))
    {
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapOut()
{
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx))
    {
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}

Fiber::ptr Fiber::GetThis()
{
    if(t_fiber)
    {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    ATPDXY_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

uint64_t Fiber::GetFiberId()
{
    if(t_fiber)
    {
        return t_fiber->getId();
    }
    return 0;
}

void Fiber::YieldToReady()
{
    // Fiber::ptr cur = GetThis();
    // ATPDXY_ASSERT(cur->m_state == EXEC);
    // cur->m_state = READY;
    // cur->swapOut();
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold()
{
    // Fiber::ptr cur = GetThis();
    // ATPDXY_ASSERT(cur->m_state == EXEC);
    // cur->swapOut();
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}

void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    ATPDXY_ASSERT(cur);
    try
    {
        cur->m_cb();
        // 避免引用技术无法为0
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except: " << e.what() << " fiber_id = " << cur->getId()
            << std::endl << BacktraceToString();
    }
    catch(...)
    {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except: " << " fiber_id = " << cur->getId() << std::endl << BacktraceToString();       
    }
    // 获得裸指针
    auto raw_ptr = cur.get();
    // 释放智能指针持有的资源，协程结束后不需要该智能指针
    cur.reset();
    raw_ptr->swapOut();
    ATPDXY_ASSERT2(false, "nevet reach fiber_id = " + std::to_string(raw_ptr->getId()));
}

}