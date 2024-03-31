#include "iomanager.h"
#include "log.h"
#include "macro.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace atpdxy {
static atpdxy::Logger::ptr g_logger = GET_LOGGER_BY_NAME("system");

// 重载输出流运算符，将枚举类型和EPOLL_EVENTS转换成输出流形式，方便调试日志
enum EpollCtlOp {

};

static std::ostream& operator<<(std::ostream& os, const EpollCtlOp& op) {
    switch((int)op) {
    #define GET_EPOLL_OP(ctl) \
        case ctl: \
            return os << #ctl;
        GET_EPOLL_OP(EPOLL_CTL_ADD);
        GET_EPOLL_OP(EPOLL_CTL_MOD);
        GET_EPOLL_OP(EPOLL_CTL_DEL);
        default:
            return os << (int)op;
    }
    #undef GET_EPOLL_OP
}

static std::ostream& operator<<(std::ostream& os, EPOLL_EVENTS events) {
    if(!events) {
        return os << "0";
    }
    bool first = true;
    #define TO_OS(E) \
        if(events & E) { \
            if(!first) { \
                os << "|"; \
            } \
            os << #E;\
            first = false; \
        }
        TO_OS(EPOLLIN);             // 表示可读事件
        TO_OS(EPOLLPRI);            // 表示紧急可读事件
        TO_OS(EPOLLOUT);            // 表示可写事件
        TO_OS(EPOLLRDNORM);         // 表示普通数据可读事件
        TO_OS(EPOLLRDBAND);         // 表示优先数据可读事件
        TO_OS(EPOLLWRNORM);         // 普通数据可写事件
        TO_OS(EPOLLWRBAND);         // 优先数据可写事件
        TO_OS(EPOLLMSG);            // 文件描述符有消息可读取
        TO_OS(EPOLLERR);            // 文件描述符发生错误
        TO_OS(EPOLLHUP);            // 文件描述符被挂起或连接断开
        TO_OS(EPOLLRDHUP);          // 对端关闭或半连接
        TO_OS(EPOLLONESHOT);        // 一次性事件，只触发一次，之后从内核事件表删除
        TO_OS(EPOLLET);             // ET触发模式
    #undef TO_OS
    return os;
}

// 返回事件上下文
IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event) {
    switch(event) {
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            ASSERT_WITH_MSG(false, "getContext");
    }
    throw std::invalid_argument("getContext invalid event");
}

// 重置事件上下文
void IOManager::FdContext::resetContext(EventContext& ctx) {
    ctx.cb = nullptr;
    // 智能指针用reset来释放资源
    ctx.fiber.reset();
    ctx.scheduler = nullptr;
}

// 触发事件
void IOManager::FdContext::triggerEvent(IOManager::Event event) {
    INFO(g_logger) << "fd=" << fd << " triggerEvent event =" << event << " events=" << events;
    // 保证事件集合中有要触发的事件
    ASSERT(events & event);
    // 取消要触发的事件
    events = (Event)(events & ~event);
    // 返回的是内部结构体EventContext的read/write上下文结构体
    EventContext& ctx = getContext(event);
    if(ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } else {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

// 构造函数，设置线程数量、是否将调用线程纳入调度器以及调度器的名称
IOManager::IOManager(size_t threads, bool use_caller, const std::string name):
    Scheduler(threads, use_caller, name){
    // 初始化epoll文件描述符
    m_epfd = epoll_create(5);
    ASSERT(m_epfd > 0);
    // 创建管道
    int rt = pipe(m_tickleFds);
    ASSERT(!rt);
    // 将管道的读加入到epoll事件表中，等待通知
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];
    // 设置非阻塞
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    ASSERT(!rt);
    // 将读端添加到事件表中
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    ASSERT(!rt);
    // 设置初始文件描述符容器大小
    contextResize(64);
    // 开始执行调度器
    start();
}

// 析构函数，释放资源
IOManager::~IOManager() {
    stop();
    // 关闭文件描述符并释放指针
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for(size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

// 向fd添加事件
int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // 添加事件时不应该重复添加事件
    if(UNLIKELY(fd_ctx->events & event)) {
        ERROR(g_logger) << "addEvent assert fd=" << fd << " event=" << (EPOLL_EVENTS)event 
            << " fd_ctx.event=" << (EPOLL_EVENTS)fd_ctx->events;
        ASSERT(!(fd_ctx->events & event));
    }
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ") fd_ctx->events="
            << (EPOLL_EVENTS)fd_ctx->events;
        return -1;
    }
    // 增加正在等待处理的事件数量
    ++m_pendingEventCount;
    // 将新添加的事件添加到已有事件中
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    ASSERT(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
    // 设置事件的上下文是当前调度器
    event_ctx.scheduler = Scheduler::GetThis();
    if(cb) {
        // 如果设置了回调函数，将事件上下文和传入的参数进行交换
        event_ctx.cb.swap(cb);
    } else {
        // 将事件上下文设置成当前协程
        event_ctx.fiber = Fiber::GetThis();
        ASSERT_WITH_MSG(event_ctx.fiber->getState() == Fiber::EXEC, "state=" << event_ctx.fiber->getState());
    }
    return 0;
}

// 向fd删除事件
bool IOManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    // 没有要删除的事件则返回false
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if(UNLIKELY(!(fd_ctx->events & event))) {
        return false;
    }

    // 从原来的事件集合中删除事件
    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    // 重新设置ET触发模式
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    --m_pendingEventCount;
    fd_ctx->events = new_events;
    // 删除事件并将调度器、协程和回调函数重置
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

// 取消fd的事件
bool IOManager::cancelEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    // 没有要取消的事件则返回
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if(UNLIKELY(!(fd_ctx->events & event))) {
        return false;
    }
    // 从原来的事件集合中删除要取消的事件
    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    // 取消后会执行被取消事件上下文的回调函数或者协程
    // 可以确保取消后立即执行相应的处理逻辑，确保在取消事件后执行一些清理操作或释放资源，保证稳定性和正确性
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

// 取消fd的所有事件
bool IOManager::cancelAll(int fd) {
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if(!fd_ctx->events) {
        return false;
    }
    // 清除所有的事件，DEL用来停止监听某个文件描述符的事件，如果要删除某个事件用MOD
    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    if(fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }
    ASSERT(fd_ctx->events == 0);
    return true;
}

// 返回当前线程正在运行的IOManager
IOManager* IOManager::GetThis() {
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

// 通知调度器有任务可以执行了
void IOManager::tickle() {
    // 没有空闲线程
    if(!hasIdleThreads()) {
        return;
    }
    // 向管道写，通知有事件到达
    int rt = write(m_tickleFds[1], "T", 1);
    ASSERT(rt == 1);
}

// 停止调度器的执行
bool IOManager::stopping() {
    uint64_t timeout = 0;
    ;return stopping(timeout);
}

// 调度器没有任务时执行idle
void IOManager::idle() {
    // 后面的括号是为了初始化，默认0
    epoll_event* events = new epoll_event[64]();
    // 只能指针，指定析构函数，创建一个shared_events智能指针，该指针的析构函数由lambda表达式给出
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });
    while(true) {
        if(stopping()) {
            INFO(g_logger) << "name=" << getName() << " idle stopping exit";
            break;
        }
        int rt = 0;
        do {
            static const int MAX_TIMEOUT = 5000;
            // 等待事件发生，最多5秒返回
            rt = epoll_wait(m_epfd, events, 64, MAX_TIMEOUT);
            if(rt < 0 && errno == EINTR) {
                // 被中断信号打断，重新调用epoll_wait等待事件
            } else {
                break;
            }
        } while(true);
        
        // 遍历监听到的事件处理
        for(int i = 0; i < rt; ++i) {
            epoll_event& event = events[i];
            // 管道的写端写了数据
            if(event.data.fd == m_tickleFds[0]) {
                uint64_t dummy;
                while(read(m_tickleFds[0], &dummy, 1) == 1);
                continue;
            }
            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            // 如果当前事件是可读或挂起，则修改为同时监听可读和可写
            if(event.events & (EPOLLIN | EPOLLHUP)) {
                event.events |= EPOLLIN | EPOLLOUT;
            }
            // 根据当前事件的类型，转换成实际的事件类型
            int real_events = NONE;
            if(event.events & EPOLLIN) {
                real_events |= READ;
            }
            if(event.events & EPOLLOUT) {
                real_events |= WRITE;
            }
            // 没有待处理的事件
            if((fd_ctx->events & real_events) == NONE) {
                continue;
            }
            // 剩余事件                                                         
            int left_events = (fd_ctx->events & ~real_events);      
            // 如果剩余事件不为空，则需要修改，否则表示不需要继续监听
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;
            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(rt2) {
                ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                    << (EpollCtlOp)op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                    << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }
            if(real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }

            if(real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }
        // 将控制权让出去
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOut();
    }
}

// 重置句柄上下文容器大小
void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);
    // 原来的不用变，新开辟的则新建上下文
    for(size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

// 是否可以停止，timeout是最近要触发的定时器事件间隔
bool IOManager::stopping(uint64_t& timeout) {
    // 获取下一个定时器的超时时间，如果为~0ull（uint64_t最大值）表示没有定时器，定时器队列为空
    // timeout = getNextTimer();
    // return timeout == ~0ull
    //     && m_pendingEventCount == 0
    //     && Scheduler::stopping();
    return Scheduler::stopping() && m_pendingEventCount == 0;
}
}

