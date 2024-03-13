/*
 * @Author: sfssa 1664549131@qq.com
 * @Date: 2024-03-05 16:14:18
 * @LastEditors: sfssa 1664549131@qq.com
 * @LastEditTime: 2024-03-05 16:22:09
 * @FilePath: /Server/atpdxy/log.h
 * @Description: 日志模块头文件
 */

#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <stdarg.h>
#include <unordered_map>
#include <time.h>
#include <functional>
#include <vector>
#include <stdexcept> 
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include "singleton.h"
#include "util.h"
#include "mutex.h"
#include "thread.h"

#define LOG(logger, level) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent(logger, level, __FILE__, __LINE__, 0, atpdxy::GetThreadId(), \
            atpdxy::GetFiberId(), time(0), atpdxy::Thread::GetName()))).getSS()

#define LOG_DEBUG(logger) LOG(logger, atpdxy::LogLevel::DEBUG)
#define LOG_INFO(logger) LOG(logger, atpdxy::LogLevel::INFO)
#define LOG_WARN(logger) LOG(logger, atpdxy::LogLevel::WARN)
#define LOG_ERROR(logger) LOG(logger, atpdxy::LogLevel::ERROR)
#define LOG_FATAL(logger) LOG(logger, atpdxy::LogLevel::FATAL)

#define LOG_FMT(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent(logger, level, __FILE__, __LINE__, 0, atpdxy::GetThreadId(), \
            atpdxy::GetFiberId(), time(0), atpdxy::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT(logger, atpdxy::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...) LOG_FMT(logger, atpdxy::LogLevel::INFO, fmt, __VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...) LOG_FMT(logger, atpdxy::LogLevel::WARN, fmt, __VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT(logger, atpdxy::LogLevel::ERROR, fmt, __VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT(logger, atpdxy::LogLevel::FATAL, fmt, __VA_ARGS__)

#define LOG_ROOT() atpdxy::LoggerMgr::GetInstance()->getRoot()
#define LOG_NAME(name) atpdxy::LoggerMgr::GetInstance()->getLogger(name)
namespace atpdxy
{
class LoggerManager;
// 向前声明Logger类
class Logger;

// 日志级别
class LogLevel
{
public:
    enum Level
    {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2, 
        WARN = 3,
        ERROR = 4, 
        FATAL = 5
    };

    // 将日志级别转换成string类型
    static const char* ToString(LogLevel::Level level);

    // 将string转换成enum
    static LogLevel::Level FromString(const std::string& str);
};

// 日志事件
class LogEvent
{
public:
    // 定义智能指针
    typedef  std::shared_ptr<LogEvent> ptr;

    // 构造函数，对日志事件进行初始化
    LogEvent(std::shared_ptr<Logger> logger, 
        LogLevel::Level level, const char* file, int32_t line,
        uint32_t elapse, uint32_t threadId, uint32_t fiberId, 
        uint64_t time, const std::string& thread_name);

    // 返回私有变量 
    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    std::string getContent() const { return m_ss.str(); }
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    std::stringstream& getSS() { return m_ss; }
    LogLevel::Level getLevel() const { return m_level; }

    // 接受可变参数
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);

    // 返回线程名称
    const std::string& getThreadName() const { return m_threadName; }
private:
    // 文件名
    const char* m_file = nullptr;

    // 行号
    int32_t m_line = 0;

    // 程序启动开始到现在的毫秒数
    uint32_t m_elapse = 0;

    // 线程id
    uint32_t m_threadId = 0;

    // 协程id
    uint32_t m_fiberId = 0;

    // 时间戳
    uint64_t m_time;

    // 内容
    std::stringstream m_ss;

    // 日志事件所在日志器
    std::shared_ptr<Logger> m_logger;

    // 日志事件级别
    LogLevel::Level m_level;

    // 线程名称
    std::string m_threadName;
};

// 日志包装器，实现RAII
class LogEventWrap
{
public:
    // 构造函数
    LogEventWrap(LogEvent::ptr e);

    // 析构函数
    ~LogEventWrap();

    // 返回字符串流
    std::stringstream& getSS();

    // 返回日志事件
    LogEvent::ptr getEvent() const { return m_event; }
private:
    LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter
{
public:
    // 日志格式
    typedef std::shared_ptr<LogFormatter> ptr;

    // 格式化提供给LoggerAppender输出
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    // 构造函数，初始化格式
    LogFormatter(const std::string& pattern);
public:
    // 单独的小项的基类，将线程id，行号等内容作为单独的小项来输出，根据pattern解析出有哪些item
    class FormatItem
    {
    public:
        // 智能指针
        typedef std::shared_ptr<FormatItem> ptr;

        // 基类将析构函数虚化
        virtual ~FormatItem() {}


        FormatItem(const std::string& fmt = "") {};

        // 输出日志事件
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    // %m 消息%p 日志级别%r 累计毫秒数%c 日志名称%t 线程id%n 换行%d 时间%f 文件名%l 行号%T 制表符%F 协程id%N 线程名称
    // 默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
    void init();

    bool isError() const { return m_error; }

    const std::string getPattern() const { return m_pattern; }
private:
    // 格式
    std::string m_pattern;

    // 存放所有format的数组
    std::vector<FormatItem::ptr> m_items;

    // 判断格式是否出错
    bool m_error = false;
};

// 日志输出目标
class LogAppender
{
friend class Logger;
public:
    // 日志输出模板的智能指针
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock MutexType;
    // 虚析构函数，用于释放所有的资源
    virtual ~LogAppender() {}

    // 输出日志内容
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    // 设置输出格式
    void setFormatter(LogFormatter::ptr val);

    // 获得输出格式
    LogFormatter::ptr getFormatter();

    // 设置输出器日志级别
    void setLevel(LogLevel::Level  val) { m_level = val; }

    // 获得输出器日志级别
    LogLevel::Level  getLevel() const { return m_level; }

    virtual std::string toYamlString() = 0;
protected:
    // 输出目标的级别
    LogLevel::Level m_level = LogLevel::DEBUG;

    // 输出目标的格式
    LogFormatter::ptr m_formatter;

    // 是否有自己的格式
    bool m_hasFormatter = false;

    // 互斥量
    MutexType m_mutex;
};

// 日志输出器
class Logger : public std::enable_shared_from_this<Logger>
{
friend class LoggerManager;
public:
    // 定义日志输出器的智能指针
    typedef std::shared_ptr<Logger> ptr;
    typedef Spinlock MutexType;
    // 构造函数，默认日志器的名称是root
    Logger(const std::string& name = "root");

    // 日志器生成日志
    void log(LogLevel::Level level, LogEvent::ptr event);

    // 输出debug级别的日志
    void debug(LogEvent::ptr event);

    // 输出info级别的日志
    void info(LogEvent::ptr event);

    // 输出warn级别的日志
    void warn(LogEvent::ptr event);
    
    // 输出error级别的日志
    void error(LogEvent::ptr event);

    // 输出fatal级别的日志
    void fatal(LogEvent::ptr event);

    // 添加输出目标
    void addAppender(LogAppender::ptr appender);

    // 删除输出目标
    void delAppender(LogAppender::ptr appender);

    // 清空appenders
    void clearAppenders();

    // 获取日志器级别
    LogLevel::Level getLevel() const { return m_level; }

    // 设置日志器级别
    void setLevel(LogLevel::Level level) { m_level = level; }

    // 返回日志器名称
    const std::string getName() const { return m_name; }

    // 设置日志器的输出格式
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();
private:
    // 日志器的输出目标
    std::list<LogAppender::ptr> m_appenders;

    // 日志器的名字
    std::string m_name;

    // 日志器的级别，大于这个级别的日志才会输出
    LogLevel::Level m_level;

    // 日志器的默认输出格式
    LogFormatter::ptr m_formatter;

    // 主日志器
    Logger::ptr m_root;

    // 互斥量
    MutexType m_mutex;
};

// 控制台输出目标
class StdoutLogAppender : public LogAppender
{

public:
    // 控制台输出目标智能指针
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    // 向控制台打印日志
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    std::string toYamlString() override;
private:

};

// 文件输出目标
class FileLogAppender : public LogAppender
{
public:
    // 文件输出目标智能指针
    typedef std::shared_ptr<FileLogAppender> ptr;

    // 向指定文件内输出
    FileLogAppender(const std::string& filenanme);

    // 向文件打印日志
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 重新打开文件
    bool reopen();

    std::string toYamlString() override;
private:
    // 文件输出目标名字
    std::string m_filename;

    // 输出流
    std::ofstream m_filestream;

    // 上次打开时间
    uint64_t m_lastTime = 0;
};

// 日志管理器
class LoggerManager
{
public:
    typedef Spinlock MutexType;
    // 根据日志器名称返回日志器
    Logger::ptr getLogger(const std::string& name);

    // 构造函数
    LoggerManager();

    // 初始化（可以从配置文件读取）
    void init();

    Logger::ptr getRoot() const { return m_root; }

    std::string toYamlString();
private:
    // 名字和日志器的映射
    std::map<std::string, Logger::ptr> m_loggers;

    // 默认主logger
    Logger::ptr m_root;

    // 互斥量
    MutexType m_mutex;
};

typedef atpdxy::Singleton<LoggerManager> LoggerMgr;

}