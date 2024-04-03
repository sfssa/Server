#include "hook.h"
#include "log.h"
#include "config.h"
#include "iomanager.h"
#include "macro.h"
#include <dlfcn.h>
#include "fd_manager.h"

atpdxy::Logger::ptr g_logger = GET_LOGGER_BY_NAME("system");

namespace atpdxy {

// 配置TCP连接的超时时间
static ConfigVar<int>::ptr g_tcp_connect_timeout = Config::Lookup<int>("tcp.connect.timeout", 5000, "tcp connect timeout");

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

void hook_init() {
    // static修饰的变量只初始化一次
    static bool is_inited = false;
    if(is_inited) {
        return;
    }
    // 将name替换为具体的函数名，构建出对应的函数指针变量，初始化为dlsym(RTLD_NEXT, #name)返回值
    // 初始化两个函数指针
    // 用来拦截系统调用，在动态链接库中找到重写的系统调用函数，避开系统调用的执行
    #define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
        HOOK_FUN(XX);
    #undef XX
}

// 超时时间
static uint64_t s_connect_timeout = -1;

struct _HookIniter {
    _HookIniter() {
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout->getValue();
        // 更新超时时间
        g_tcp_connect_timeout->addListener([](const int& old_value, const int& new_value){
            INFO(g_logger) << "tcp connect timeout changed from " << old_value << " to " << new_value; 
            s_connect_timeout = new_value;
        });
    }
};

// 在main函数前初始化
static _HookIniter s_hook_initer;

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}
}

// 存储定时器信息
struct timer_info {
    // 定时器是否被取消
    int cancelled = 0;
};

// 封装io操作，在阻塞期间让出执行权，提高并发性能，当触发EAGAIN后，当前资源不可用，添加对应的事件和定时器
// 等待任务被唤醒，此时需要让出执行权
// fd-文件描述符
// fun-I/O操作
// hook_fun_name-当前调用的函数名称
// event-进行IO操作时关注的事件类型，如读取、写入
// timeout_so超时设置
template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name, uint32_t event, int timeout_so, Args&&... args) {
    if(!atpdxy::t_hook_enable) {
        // 没有hook则正常执行
        return fun(fd, std::forward<Args>(args)...);
    }

    // 返回fd上下文，如果不存在上下文正常执行
    atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(fd);
    if(!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }
    // 如果文件描述符已经关闭，设置错误码无效的文件描述符
    if(ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    // 如果不是套接字或者没有设置hook则正常执行
    if(!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    // 返回读/写的超时时间
    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);
retry:
    // 执行系统调用函数
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    // 被中断了重新执行
    while(n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }
    // I/O操作会阻塞，EAGAIN表示当前资源不可用，需要阻塞
    if(n == -1 && errno == EAGAIN) {
        atpdxy::IOManager* iom = atpdxy::IOManager::GetThis();
        atpdxy::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);
        // 如果不等于-1，则设置了超时时间
        if(to != (uint64_t)-1) {
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();
                // 如果条件已经为空或者定时器已经被取消直接返回
                if(!t || t->cancelled) {
                    return;
                }
                // 定时器未取消，设置错误码为超时
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, (atpdxy::IOManager::Event)(event));
            }, winfo);
        }

        int rt = iom->addEvent(fd, (atpdxy::IOManager::Event)(event));
        if(UNLIKELY(rt)) {
            // 添加事件失败
            ERROR(g_logger) << hook_fun_name << " addEvent("
                << fd << ", " << event << ")";
            // 添加事件失败，如果已经添加了定时器，将定时器取消
            if(timer) {
                timer->cancel();
            }
            return -1;
        } else {
            atpdxy::Fiber::YieldToHold();
            // 取消上次的定时器
            if(timer) {
                timer->cancel();
            }
            // 如果超时了，设置错误码
            if(tinfo->cancelled) {
                errno = tinfo->cancelled;
                return -1;
            }
            // 继续下次尝试获取资源来执行
            goto retry;
        }
    }
    return n;
}

extern "C" {
// 初始化函数指针指向nullptr，在预处理阶段完成宏替换，之后编译的时候同init函数完成初始化
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

unsigned int sleep(unsigned int seconds) {
    // 如果没有hook则正常执行函数（在init函数中已经指向了sleep函数）
    if(!atpdxy::t_hook_enable) {
        return sleep_f(seconds);
    }
    atpdxy::Fiber::ptr fiber = atpdxy::Fiber::GetThis();
    atpdxy::IOManager* iom = atpdxy::IOManager::GetThis();
    iom->addTimer(seconds * 1000, std::bind((void(atpdxy::Scheduler::*)(atpdxy::Fiber::ptr, int thread))&atpdxy::IOManager::schedule, iom, fiber, -1));
    atpdxy::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec) {
    // 如果没有hook则正常执行函数（在init函数中已经指向了usleep函数）
    if(!atpdxy::t_hook_enable) {
        return usleep_f(usec);
    }
    atpdxy::Fiber::ptr fiber = atpdxy::Fiber::GetThis();
    atpdxy::IOManager* iom = atpdxy::IOManager::GetThis();
    iom->addTimer(usec / 1000, std::bind((void(atpdxy::Scheduler::*)(atpdxy::Fiber::ptr, int thread))&atpdxy::IOManager::schedule, iom, fiber, -1));
    atpdxy::Fiber::YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if(!atpdxy::t_hook_enable) {
        return nanosleep_f(req, rem);
    }
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 /1000;
    atpdxy::Fiber::ptr fiber = atpdxy::Fiber::GetThis();
    atpdxy::IOManager* iom = atpdxy::IOManager::GetThis();
    iom->addTimer(timeout_ms, std::bind((void(atpdxy::Scheduler::*)
            (atpdxy::Fiber::ptr, int thread))&atpdxy::IOManager::schedule
            ,iom, fiber, -1));
    atpdxy::Fiber::YieldToHold();
    return 0;
}

int socket(int domain, int type, int protocol) {
    if(!atpdxy::t_hook_enable) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if(fd == -1) {
        return fd;
    }
    // 创建上下文
    atpdxy::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

// 
int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms) {
    if(!atpdxy::t_hook_enable) {
        return connect_f(fd, addr, addrlen);
    }
    atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(fd);
    // 没有上下文或者fd已经关闭了
    if(!ctx || ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    // 不是套接字，正常执行
    if(!ctx->isSocket()) {
        return connect_f(fd, addr, addrlen);
    }

    // 使用hook
    if(ctx->getUserNonblock()) {
        return connect_f(fd, addr, addrlen);
    }
    // 执行系统调用
    int n = connect_f(fd, addr, addrlen);
    if(n == 0) {
        return 0;
    } else if(n != -1 || errno != EINPROGRESS) {
        //EINPROGRESS意味着造作已经启动但未完成
        return n;
    }

    atpdxy::IOManager* iom = atpdxy::IOManager::GetThis();
    atpdxy::Timer::ptr timer;
    std::shared_ptr<timer_info> tinfo(new timer_info);
    std::weak_ptr<timer_info> winfo(tinfo);
    // timer_info用来跟踪定时器的状态
    if(timeout_ms != (uint64_t)-1) {
        timer = iom->addConditionTimer(timeout_ms, [winfo, fd, iom]() {
                auto t = winfo.lock();
                if(!t || t->cancelled) {
                    return;
                }
                // 超时后取消事件
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, atpdxy::IOManager::WRITE);
        }, winfo);
    }

    int rt = iom->addEvent(fd, atpdxy::IOManager::WRITE);
    if(rt == 0) {
        atpdxy::Fiber::YieldToHold();
        if(timer) {
            timer->cancel();
        }
        if(tinfo->cancelled) {
            errno = tinfo->cancelled;
            return -1;
        }
    } else {
        // 添加事件失败，取消定时器
        if(timer) {
            timer->cancel();
        }
        ERROR(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
    }
    // 获取并返回错误码
    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if(!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen, atpdxy::s_connect_timeout);
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(s, accept_f, "accept", atpdxy::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >= 0) {
        // 保存上下文
        atpdxy::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", atpdxy::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", atpdxy::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", atpdxy::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", atpdxy::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", atpdxy::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", atpdxy::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", atpdxy::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags) {
    return do_io(s, send_f, "send", atpdxy::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {
    return do_io(s, sendto_f, "sendto", atpdxy::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    return do_io(s, sendmsg_f, "sendmsg", atpdxy::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if(!atpdxy::t_hook_enable) {
        return close_f(fd);
    }
    // 关闭所有的事件并删除上下文
    atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(fd);
    if(ctx) {
        auto iom = atpdxy::IOManager::GetThis();
        if(iom) {
            iom->cancelAll(fd);
        }
        atpdxy::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

int fcntl(int fd, int cmd, ...) {
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        case F_SETFL:   
            {
                // 设置文件描述符的状态标志
                int arg = va_arg(va, int);
                va_end(va);
                atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if(ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;
                } else {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:   
            {
                // 获得文件描述符的状态标志
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return arg;
                }
                if(ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ: 
#endif
            {
                // 设置管道的最大容量
                // 从va_list中获取一个int参数赋值给arg
                int arg = va_arg(va, int);
                // 清理va不再使用它
                va_end(va);
                // 调用fcntl系统调用
                return fcntl_f(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
            {
                // 返回管道的最大容量
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                // 返回文件的锁信息
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                // 设置套接字异步通知
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int d, unsigned long int request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);
    // FIONBIO意味着将套接字设置为非阻塞模式
    if(FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(d);
        if(!ctx || ctx->isClose() || !ctx->isSocket()) {
            return ioctl_f(d, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(d, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if(!atpdxy::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    // 级别是套接字
    if(level == SOL_SOCKET) {
        // 在接收超时或者发送超时的情况下
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            atpdxy::FdCtx::ptr ctx = atpdxy::FdMgr::GetInstance()->get(sockfd);
            if(ctx) {
                // 获取上下文，设置超时时间
                const timeval* v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}
}