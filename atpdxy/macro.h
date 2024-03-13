#pragma once

#include <string.h>
#include <assert.h>
#include "log.h"
#include "util.h"

#if defined __GNUC__ || defined __llvm__ // 如果是 GCC 或 LLVM 编译器
    // 使用内置函数来标记条件的可能性，以帮助编译器优化代码
    // 先将x转换成bool值，然后告诉编译器期望值很有可能是1
    #define ATPDXY_LIKELY(x)  __builtin_expect(!!(x), 1)
    #define ATPDXY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else 
    // 在其他编译器下，简单地返回条件本身，不进行额外的标记
    #define ATPDXY_LIKELY(x)  (x)
    #define ATPDXY_UNLIKELY(x) (x)
#endif

// 条件x更多情况是成立的，减少不必要的跳转
#define ATPDXY_ASSERT(x) \
    if(ATPDXY_UNLIKELY(!(x))) \
    { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << atpdxy::BacktraceToString(100, 2, "  "); \
        assert(x); \
    }

#define ATPDXY_ASSERT2(x, w) \
    if(ATPDXY_UNLIKELY(!(x))) \
    { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << atpdxy::BacktraceToString(100, 2, "  "); \
        assert(x); \
    }
