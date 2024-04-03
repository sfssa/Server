#pragma once

#include <memory>
#include <vector>
#include "thread.h"
#include "singleton.h"

namespace atpdxy {
// 文件描述符封装类
class FdCtx : public std::enable_shared_from_this<FdCtx> {
public:
    typedef std::shared_ptr<FdCtx> ptr;
    
    // 通过文件句柄构建文件描述符上下文
    FdCtx(int fd);

    ~FdCtx();

    // 返回是否初始化完成
    bool isInit() const { return m_isInit; }

    // 返回是否是socket
    bool isSocket() const { return m_isSocket; }

    // 返回是否已经关闭
    bool isClose() const { return m_isClosed; }

    // 设置用户非阻塞变量值
    void setUserNonblock(bool v) { m_userNonblock = v; }

    // 返回是否用户主动设置非阻塞
    bool getUserNonblock() const { return m_userNonblock; }

    // 设置系统非阻塞变量值
    void setSysNonblock(bool v) { m_sysNonblock = v; }

    // 返回是否设置系统非阻塞变量值
    bool getSysNonblock() const { return m_sysNonblock; }

    // 设置文件描述符操作的超时时间,type指定读操作或是写操作
    void setTimeout(int type, uint64_t v);

    // 返回文件描述符操作的超时时间
    uint64_t getTimeout(int type);
private:        
    // 内部初始化
    bool init();
private:
    // bool是一个字节（8位），下面的语法使得只占用1位，允许将多个布尔值打包到一个字节中，优化内存
    // 是否初始化
    bool m_isInit: 1;
    // 是否是socket
    bool m_isSocket: 1;
    // 是否hook设置非阻塞
    bool m_sysNonblock: 1;
    // 是否用户主动设置非阻塞
    bool m_userNonblock: 1;
    // 文件描述符是否已经关闭
    bool  m_isClosed: 1;
    // 文件句柄
    int m_fd;
    // 读超时时间毫秒数
    uint64_t m_recvTimeout;
    // 写超时时间毫秒数
    uint64_t m_sendTimeout;
};

// 文件描述符管理类
class FdManager {
public:
    typedef RWMutex RWMutexType;

    FdManager();

    // 获取/创建文件描述符上下文类
    FdCtx::ptr get(int fd, bool auto_create = false);

    // 删除某个文件描述符封装类
    void del(int fd);
private:
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_datas;
};

// 设置单例模式
typedef Singleton<FdManager> FdMgr;
}