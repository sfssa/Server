#pragma once
#include <string>
#include "mutex.h"
#include "noncopyable.h"

namespace atpdxy {

// 线程类封装
class Thread : Noncopyable {
public:
    // 线程智能指针类型
    typedef std::shared_ptr<Thread> ptr;

    // 构造函数，设置线程执行任务和线程名称
    Thread(std::function<void()> cb, const std::string& name);

    ~Thread();

    // 返回线程id
    pid_t getId() const { return m_id;}

    // 返回线程名称
    const std::string& getName() const { return m_name;}

    // 主线程等待线程执行完毕回收资源
    void join();

    // 获取当前正在执行的线程指针
    static Thread* GetThis();

    // 获取当前正在执行的线程名称
    static const std::string& GetName();

    // 设置当前正在执行线程的名称
    static void SetName(const std::string& name);
private:
    // 线程内部执行函数
    static void* run(void* arg);
private:
    // 线程id
    pid_t m_id = -1;
    // 线程结构
    pthread_t m_thread = 0;
    // 线程执行函数
    std::function<void()> m_cb;
    // 线程名称
    std::string m_name;
    // 信号量
    Semaphore m_semaphore;
};

}
