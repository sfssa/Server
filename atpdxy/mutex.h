#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <atomic>

namespace atpdxy
{
class Semaphore
{
public:
    // 构造函数，count是共享资源的数量
    Semaphore(uint32_t count = 0);

    // 析构函数，释放信号量对象
    ~Semaphore();

    // 阻塞函数
    void wait();

    // 唤醒等待共享资源的线程
    void notify();
private:
    // 避免不必要的拷贝
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
private:
    sem_t m_semaphore;
};

// RAII机制实现自动释放锁资源
template <class T>
struct ScopedLockImpl
{
public:
    // 构造函数-枷锁
    ScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.lock();
        m_locked = true;
    }

    // 析构函数-解锁
    ~ScopedLockImpl()
    {
        unlock();
    }

    // 上锁
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    // 解锁
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // 传递来的锁
    T& m_mutex;

    // 是否上锁(避免未上锁时解锁和上锁后枷锁)
    bool m_locked;
};

template <class T>
struct ReadScopedLockImpl
{
public:
    // 构造函数-枷锁
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.rdlock();
        m_locked = true;
    }

    // 析构函数-解锁
    ~ReadScopedLockImpl()
    {
        unlock();
    }

    // 上锁
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.relock();
            m_locked = true;
        }
    }

    // 解锁
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // 传递来的锁
    T& m_mutex;

    // 是否上锁(避免未上锁时解锁和上锁后枷锁)
    bool m_locked;
};

template <class T>
struct WriteScopedLockImpl
{
public:
    // 构造函数-枷锁
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex)
    {
        m_mutex.wrlock();
        m_locked = true;
    }

    // 析构函数-解锁
    ~WriteScopedLockImpl()
    {
        unlock();
    }

    // 上锁
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    // 解锁
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    // 传递来的锁
    T& m_mutex;

    // 是否上锁(避免未上锁时解锁和上锁后枷锁)
    bool m_locked;
};

class RWMutex
{
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    RWMutex()
    {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_lock);
    }  

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

// 互斥量
class Mutex
{
public:
    typedef ScopedLockImpl<Mutex> Lock; 

    Mutex()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class NullMutex
{
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {} 
};

class NullRWMutex
{
public:
    typedef ReadScopedLockImpl<NullMutex> ReadLock;
    typedef WriteScopedLockImpl<NullMutex> WriteLock;
    NullRWMutex() {}
    ~NullRWMutex() {}
    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};

// 自旋锁
class Spinlock
{
public:
    typedef ScopedLockImpl<Spinlock> Lock;

    Spinlock()
    {
        pthread_spin_init(&m_mutex, 0);
    }

    ~Spinlock()
    {
        pthread_spin_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

// 原子锁
class CASLock
{
public:
    typedef ScopedLockImpl<CASLock> Lock;

    CASLock()
    {
        m_mutex.clear();
    }

    ~CASLock()
    {

    }

    void lock()
    {   
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock()
    {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};
}