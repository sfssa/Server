#pragma once

#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

namespace atpdxy
{
// 获取当前线程id
pid_t GetThreadId();

// 获取当前协程id
uint32_t GetFiberId();
}