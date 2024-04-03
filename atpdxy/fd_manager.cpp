#include "fd_manager.h"
#include "hook.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace atpdxy {

// 通过文件句柄构建文件描述符上下文
FdCtx::FdCtx(int fd):
    m_isInit(false), 
    m_isSocket(false), 
    m_sysNonblock(false), 
    m_userNonblock(false),
    m_isClosed(false), 
    m_fd(fd), 
    m_recvTimeout(-1),
    m_sendTimeout(-1) {
    init();
}

FdCtx::~FdCtx() {

}

// 设置文件描述符对应操作的超时时间,type指定读操作或是写操作
void FdCtx::setTimeout(int type, uint64_t v) {
    if(type == SO_RCVTIMEO) {
        m_recvTimeout = v;
    } else {
        m_sendTimeout = v;
    }
}

// 返回文件描述符操作的超时时间
uint64_t FdCtx::getTimeout(int type) {
    if(type == SO_RCVTIMEO) {
        return m_recvTimeout;
    } else {
        return m_sendTimeout;
    }
}

// 内部初始化
bool FdCtx::init() {
    // 已经初始化好了，返回false
    if(m_isInit) {
        return false;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;
    struct stat fd_stat;
    // 获取文件描述符信息失败
    if(-1 == fstat(m_fd, &fd_stat)) {
        m_isInit = false;
        m_isSocket = false;
    } else {
        m_isInit = true;
        // 检查fd是否是套接字
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }
    if(m_isSocket) {
        // 是否设置了非阻塞标志，没有则设置成非阻塞
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        if(!(flags & O_NONBLOCK)) {
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    } else {
        m_sysNonblock = false;
    }
    // 默认不开启hook
    m_userNonblock = false;
    m_isClosed = false;
    return m_isInit;
}

FdManager::FdManager() {
    m_datas.resize(64);
}

// 获取/创建文件描述符上下文类
FdCtx::ptr FdManager::get(int fd, bool auto_create) {
    if(fd == -1) {
        return nullptr;
    }
    // 检查是否已经有该文件描述符
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_datas.size() <= fd) {
        // 没有对应的fd且并不允许自动创建则返回空指针
        if(auto_create == false) {
            return nullptr;
        }
    } else {
        if(m_datas[fd] || !auto_create) {
            return m_datas[fd];
        }
    }
    lock.unlock();
    RWMutexType::WriteLock lock2(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(fd >= (int)m_datas.size()) {
        m_datas.resize(fd * 1.5);
    }
    m_datas[fd] = ctx;
    return ctx;
}

// 删除某个文件描述符封装类
void FdManager::del(int fd) {
    RWMutexType::WriteLock lock(m_mutex);
    // 如果没有对应的fd则直接返回
    if((int)m_datas.size() <= fd) {
        return;
    }
    m_datas[fd].reset();
}
}