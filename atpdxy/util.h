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
// 返回当前线程的ID
pid_t GetThreadId();

// 返回当前协程的ID
uint32_t GetFiberId();

// 获取当前的调用栈信息
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

// 获取当前栈信息的字符串
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

// 返回变量T的类型名称
template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}
}