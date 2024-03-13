#pragma once

#include <sys/syscall.h>
#include <unistd.h>
#include <vector>
#include <execinfo.h>
#include <string>
#include <stdint.h>
#include <cxxabi.h>
#include "log.h"

namespace atpdxy
{
// 获取当前线程id
pid_t GetThreadId();

// 获取当前协程id
uint32_t GetFiberId();

// 获取调用栈信息
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

// 将调用栈的信息转换成string字符串
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

// 辅助函数，解析符号名并反编译
std::string demangle(const char* symbol);
}