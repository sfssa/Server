#pragma once
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

namespace atpdxy {
// 当前线程是否已经hook
bool is_hook_enable();

// 设置当前线程的hook状态
void set_hook_enable(bool flag);
}

// 按照C语言的方式来编译和链接，C++中为了支持重载会对函数名加上一些符号，而C中函数名就是源码中的名字
// 如果不使用extern "C"来修饰，会导致链接时无法找到对应的C语言函数定义（指向的是系统调用）
extern "C" {
// 声明函数指针，执返回值是unsigned int，函数指针名字为sleep_fun，接受unsigned int参数
// 声明外部函数指针变量，一个名为sleep_f外部函数指针变量，指向一个与sleep_fun匹配的函数

// sleep暂停当前线程一段时间，秒
typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

// usleep暂停当前线程一段时间，微秒
typedef int (*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;

// nanosleep暂停当前线程一段时间，纳秒
typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
extern nanosleep_fun nanosleep_f;

// socket创建一个新的套接字
typedef int (*socket_fun)(int domain, int type, int protocol);
extern socket_fun socket_f;

// connect尝试连接到指定的套接字地址
typedef int (*connect_fun)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern connect_fun connect_f;

// accept等待并接受来自客户端的连接请求
typedef int (*accept_fun)(int s, struct sockaddr *addr, socklen_t *addrlen);
extern accept_fun accept_f;

// read从文件描述符读取数据
typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
extern read_fun read_f;

// readv从文件描述符中读取数据到多个缓冲区中
typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
extern readv_fun readv_f;

// recv从套接字读取数据
typedef ssize_t (*recv_fun)(int sockfd, void *buf, size_t len, int flags);
extern recv_fun recv_f;

// recvfrom从指定套接字读取数据并获取发送端的地址信息
typedef ssize_t (*recvfrom_fun)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
extern recvfrom_fun recvfrom_f;

// recvmsg从套接字接收数据和相关信息
typedef ssize_t (*recvmsg_fun)(int sockfd, struct msghdr *msg, int flags);
extern recvmsg_fun recvmsg_f;

// write向文件描述符中写入数据
typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
extern write_fun write_f;

// writev将多个缓冲区中的数据写入到文件描述符中
typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
extern writev_fun writev_f;

// send向套接字发送数据
typedef ssize_t (*send_fun)(int s, const void *msg, size_t len, int flags);
extern send_fun send_f;

// sendto向指定的套接字发送数据
typedef ssize_t (*sendto_fun)(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
extern sendto_fun sendto_f;

// sendmsg向套接字发送数据和相关信息
typedef ssize_t (*sendmsg_fun)(int s, const struct msghdr *msg, int flags);
extern sendmsg_fun sendmsg_f;

// close关闭一个已经打开的文件描述符
typedef int (*close_fun)(int fd);
extern close_fun close_f;

// fcntl操作文件描述符的属性
typedef int (*fcntl_fun)(int fd, int cmd, ...);
extern fcntl_fun fcntl_f;

// ioctl设备控制命令
typedef int (*ioctl_fun)(int d, unsigned long int request, ...);
extern ioctl_fun ioctl_f;

// getsockopt获取套接字选项
typedef int (*getsockopt_fun)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern getsockopt_fun getsockopt_f;

// setsockopt设置套接字选项
typedef int (*setsockopt_fun)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
extern setsockopt_fun setsockopt_f;

extern int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms);
}