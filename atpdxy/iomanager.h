#pragma once

#include "scheduler.h"
#include "timer.h"

namespace atpdxy {
class IOManager : public Scheduler, public TimerManager {
public:
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<IOManager> ptr;

    // IO事件
    enum Event {
        // 无事件
        NONE = 0x0,
        // 读事件
        READ = 0x1,
        // 写事件
        WRITE = 0x4
    };
private:
    // 文件描述符上下文结构体，用于异步I/O操作中跟踪事件状态和处理事件回调
    struct FdContext {
        typedef Mutex MutexType;
        
        // I/O事件上下文
        struct EventContext {
            // 执行I/O事件的调度器
            Scheduler* scheduler = nullptr;
            // 协程智能指针
            Fiber::ptr fiber;
            // 回调函数
            std::function<void()> cb;
        };
        
        // 返回事件上下文
        EventContext& getContext(Event event);

        // 重置事件上下文
        void resetContext(EventContext& ctx);

        // 触发事件
        void triggerEvent(Event event);

        // 读事件上下文
        EventContext read;
        // 写事件上下文
        EventContext write;
        // 文件描述符
        int fd;
        // 当前文件描述符的事件
        Event events = NONE;
        // 事件的互斥锁
        MutexType mutex;
    };
public:
    // 构造函数，设置线程数量、是否将调用线程纳入调度器以及调度器的名称
    IOManager(size_t threads = 1, bool use_caller = true, const std::string name = "");

    // 析构函数，释放资源
    ~IOManager();

    // 向fd添加事件
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    // 向fd删除事件
    bool delEvent(int fd, Event event);

    // 取消fd的事件
    bool cancelEvent(int fd, Event event);

    // 取消fd的所有事件
    bool cancelAll(int fd);

    // 返回当前线程正在运行的IOManager
    static IOManager* GetThis();
protected:
    // 通知调度器有任务可以执行了
    void tickle() override;

    // 停止调度器的执行
    bool stopping() override;

    // 调度器没有任务时执行idle
    void idle() override;

    // 重置句柄上下文容器大小
    void contextResize(size_t size);

    // 有新的定时器插入到set中需要唤醒
    void onTimerInsertedAtFront() override;

    // 是否可以停止，timeout是最近要触发的定时器事件间隔
    bool stopping(uint64_t& timeout);
private:
    // epoll句柄
    int m_epfd = 0;
    // pipe管道句柄
    int m_tickleFds[2];
    // 等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount = {0};
    // 读写锁
    RWMutexType m_mutex;
    // socket事件上下文的容器
    std::vector<FdContext*> m_fdContexts;
};

}