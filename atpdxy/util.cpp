#include "util.h"
#include "fiber.h"

namespace atpdxy
{

atpdxy::Logger::ptr g_logger = LOG_NAME("system");

pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}

uint32_t GetFiberId()
{
    return Fiber::GetFiberId();
}

// void Backtrace(std::vector<std::string>& bt, int size, int skip)
// {
//     void** array = (void**)malloc((sizeof(void*) * size));
//     size_t s = ::backtrace(array, size);

//     char** strings = backtrace_symbols(array, s);
//     if(strings == NULL)
//     {
//         LOG_ERROR(g_logger) << "backtrace_symbols error";
//         return;
//     }

//     for(size_t i = skip; i < s; ++i)
//     {
//         bt.push_back(strings[i]);
//     }
//     free(strings);
//     free(array);
// }

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    // 使用智能指针替代手动分配的内存，以确保在函数退出时自动释放内存，避免内存泄漏
    std::unique_ptr<void*[]> array(new void * [size]);
    size_t s = ::backtrace(array.get(), size);

    char** strings = backtrace_symbols(array.get(), s);
    if (strings == nullptr) {
        // 输出错误日志并返回
        LOG_ERROR(g_logger) << "backtrace_synbols error";
        return;
    }

    for (size_t i = skip; i < s; ++i) {
        // 使用 std::string 代替 char* 来存储字符串，并避免内存泄漏
        bt.push_back(demangle(strings[i]));
    }

    // 使用智能指针替代手动释放内存，确保在函数退出时自动释放内存，避免内存泄漏
    free(strings);
}

// 辅助函数：将符号名进行反编译
std::string demangle(const char* symbol) {
    size_t size;
    int status;
    char* demangled = abi::__cxa_demangle(symbol, nullptr, &size, &status);
    std::string result = (status == 0) ? demangled : symbol;
    free(demangled);
    return result;
}

// static std::string demangle(const char* str) 
// {
//     size_t size = 0;
//     int status = 0;
//     std::string rt;
//     rt.resize(256);
//     if(1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) 
//     {
//         char* v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
//         if(v) 
//         {
//             std::string result(v);
//             free(v);
//             return result;
//         }
//     }
//     if(1 == sscanf(str, "%255s", &rt[0])) 
//     {
//         return rt;
//     }
//     return str;
// }

// std::string BacktraceToString(int size, int skip, const std::string& prefix)
// {
//     std::vector<std::string> bt;
//     Backtrace(bt, size, skip);
//     std::stringstream ss;
//     for(size_t i = 0; i < bt.size(); ++i)
//     {
//         ss << prefix << bt[i] << std::endl;
//     }
//     return ss.str();
// }

std::string BacktraceToString(int size, int skip, const std::string& prefix) 
{
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    
    std::ostringstream ss; // 使用 std::ostringstream 代替 std::stringstream
    
    for (const auto& line : bt) { // 使用范围遍历语法
        ss << prefix << line << '\n'; // 使用 '\n' 替代 std::endl，避免刷新缓冲区
    }
    
    return ss.str();
}
}