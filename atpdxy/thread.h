/*
 * @Author: sfssa 1664549131@qq.com
 * @Date: 2024-03-11 17:02:43
 * @LastEditors: sfssa 1664549131@qq.com
 * @LastEditTime: 2024-03-11 17:12:58
 * @FilePath: \projects\C++\thread.h
 * @Description: Do not edit
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <pthread.h>
#include "mutex.h"

namespace atpdxy
{
class Thread
{
public:
    // 线程的智能指针
    typedef std::shared_ptr<Thread> ptr;

    // 构造函数，接受一个回调函数和线程名
    Thread(std::function<void()> cb, const std::string& name);

    // 析构函数
    ~Thread();

    // 返回pid
    pid_t getId() const { return m_id; }

    // 返回线程名称
    const std::string getName() const { return m_name; }

    // 等待回收资源
    void join();

    // 返回当前正在执行的线程
    static Thread* GetThis();

    // 返回当前线程名称（为日志服务）
    static const std::string& GetName();

    // 设置线程名称
    static void SetName(const std::string& name);
private:
    // 避免线程被拷贝
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    static void* run(void* arg);
private:
    // 线程标识符：线程的id
    pid_t m_id = -1;

    // 线程控制块指针
    pthread_t m_thread = 0;
    
    // 回调函数
    std::function<void()> m_cb;

    // 线程名
    std::string m_name;

    // 信号量
    Semaphore m_semaphore;
};
}