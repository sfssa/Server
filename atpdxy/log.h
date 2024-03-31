#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"
#include "thread.h"

// 使用流式方式将日志级别level的日志写入到logger
#define LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent( \
        logger, level, __FILE__, __LINE__, 0, atpdxy::GetThreadId(),\
        atpdxy::GetFiberId(), time(0), atpdxy::Thread::GetName()))).getSS()

// 写debug级别日志
#define DEBUG(logger) LOG_LEVEL(logger, atpdxy::LogLevel::DEBUG)

// 写info级别日志
#define INFO(logger) LOG_LEVEL(logger, atpdxy::LogLevel::INFO)

// 写warn级别日志
#define WARN(logger) LOG_LEVEL(logger, atpdxy::LogLevel::WARN)

// 写error级别日志
#define ERROR(logger) LOG_LEVEL(logger, atpdxy::LogLevel::ERROR)

// 写fatal级别日志
#define FATAL(logger) LOG_LEVEL(logger, atpdxy::LogLevel::FATAL)

// 使用格式化方式将日志级别level的日志写入到logger
#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        atpdxy::LogEventWrap(atpdxy::LogEvent::ptr(new atpdxy::LogEvent(logger, level, \
        __FILE__, __LINE__, 0, atpdxy::GetThreadId(), atpdxy::GetFiberId(), \
        time(0), atpdxy::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

// 写debug级别日志
#define FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, atpdxy::LogLevel::DEBUG, fmt, __VA_ARGS__)

// 写debug级别日志
#define FMT_INFO(logger, fmt, ...)  LOG_FMT_LEVEL(logger, atpdxy::LogLevel::INFO, fmt, __VA_ARGS__)

// 写debug级别日志
#define FMT_WARN(logger, fmt, ...)  LOG_FMT_LEVEL(logger, atpdxy::LogLevel::WARN, fmt, __VA_ARGS__)

// 写debug级别日志
#define FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, atpdxy::LogLevel::ERROR, fmt, __VA_ARGS__)

// 写debug级别日志
#define FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, atpdxy::LogLevel::FATAL, fmt, __VA_ARGS__)

// 写debug级别日志
#define GET_ROOT_LOGGER() atpdxy::LoggerMgr::GetInstance()->getRoot()

// 写debug级别日志
#define GET_LOGGER_BY_NAME(name) atpdxy::LoggerMgr::GetInstance()->getLogger(name)

namespace atpdxy {

class Logger;
class LoggerManager;

// 日志级别
class LogLevel {
public:
    enum Level {
        // 未知级别
        UNKNOW = 0,
        // DEBUG 级别，调试时输出
        DEBUG = 1,
        // INFO 级别，程序运行时提示
        INFO = 2,
        // WARN 级别，警告级别，可能会有问题
        WARN = 3,
        // ERROR 级别，出现了错误但不会导致程序崩溃
        ERROR = 4,
        // FATAL 级别，出现了使的程序无法继续运行的错误
        FATAL = 5
    };

    // 从枚举类型转换成字符串
    static const char* ToString(LogLevel::Level level);
    
    // 从字符串转换成枚举类型
    static LogLevel::Level FromString(const std::string& str);
};

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    // 构造函数
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
        ,const char* file, int32_t line, uint32_t elapse
        ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
        ,const std::string& thread_name);

    // 返回文件名
    const char* getFile() const { return m_file;}

    // 返回行号
    int32_t getLine() const { return m_line;}

    // 返回从程序启动到打印日志经过的时间
    uint32_t getElapse() const { return m_elapse;}

    // 返回线程id
    uint32_t getThreadId() const { return m_threadId;}

    // 返回协程id
    uint32_t getFiberId() const { return m_fiberId;}

    // 返回时间戳
    uint64_t getTime() const { return m_time;}

    // 返回线程名称
    const std::string& getThreadName() const { return m_threadName;}

    // 返回日志内容
    std::string getContent() const { return m_ss.str();}

    // 返回日志所在的日志器
    std::shared_ptr<Logger> getLogger() const { return m_logger;}

    // 返回日志级别
    LogLevel::Level getLevel() const { return m_level;}

    // 返回日志内容的字符串流
    std::stringstream& getSS() { return m_ss;}

    // 格式化写入日志内容辅助函数
    void format(const char* fmt, ...);

    // 格式化写入日志内容
    void format(const char* fmt, va_list al);
private:
    // 文件名
    const char* m_file = nullptr;
    // 行号
    int32_t m_line = 0;
    // 程序启动开始到现在的毫秒数
    uint32_t m_elapse = 0;
    // 线程ID
    uint32_t m_threadId = 0;
    // 协程ID
    uint32_t m_fiberId = 0;
    // 时间戳
    uint64_t m_time = 0;
    // 线程名称
    std::string m_threadName;
    // 日志内容流
    std::stringstream m_ss;
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志等级
    LogLevel::Level m_level;
};

// 日志包装器，实现RAII机制
class LogEventWrap {
public:
    // 构造函数
    LogEventWrap(LogEvent::ptr e);

    // 析构函数
    ~LogEventWrap();

    // 返回日志事件
    LogEvent::ptr getEvent() const { return m_event;}

    // 返回日志事件的内容流
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

// 打印日志的格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    // 默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
    LogFormatter(const std::string& pattern);

    // 返回格式化日志内容的字符串
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    
    // 返回格式化日志内容的输出流
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    // 日志内容项基类
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        
        virtual ~FormatItem() {}
        
        // 将日志格式化并放到输出流中
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    // 解析格式
    void init();

    // 解析格式是否出错
    bool isError() const { return m_error;}

    // 返回格式
    const std::string getPattern() const { return m_pattern;}
private:
    // 日志格式模板
    std::string m_pattern;
    // 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
    // 是否有错误
    bool m_error = false;
};

// 日志输出目标：控制台/文件
class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock MutexType;

    // 虚函数，释放资源
    virtual ~LogAppender() {}

    // 向目标写日志
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    // 将日志输出目标的信息转换成yaml格式
    virtual std::string toYamlString() = 0;

    // 更改输出器的格式器
    void setFormatter(LogFormatter::ptr val);

    // 返回日志输出器的格式器
    LogFormatter::ptr getFormatter();

    // 返回输出器的级别
    LogLevel::Level getLevel() const { return m_level;}

    // 设置输出器的日志级别
    void setLevel(LogLevel::Level val) { m_level = val;}
protected:
    // 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 是否有自己的日志格式器
    bool m_hasFormatter = false;
    // Mutex
    MutexType m_mutex;
    // 日志格式器
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Spinlock MutexType;

    // 构造函数
    Logger(const std::string& name = "root");

    // 写日志器中包含的日志
    void log(LogLevel::Level level, LogEvent::ptr event);

    // 写debug级别的日志
    void debug(LogEvent::ptr event);

    // 写info级别的日志
    void info(LogEvent::ptr event);

    // 写warn级别的日志
    void warn(LogEvent::ptr event);

    // 写error级别的日志
    void error(LogEvent::ptr event);

    // 写fatal级别的日志
    void fatal(LogEvent::ptr event);

    // 添加输出器
    void addAppender(LogAppender::ptr appender);

    // 删除输出器
    void delAppender(LogAppender::ptr appender);

    // 清空输出器
    void clearAppenders();

    // 返回日志器的级别
    LogLevel::Level getLevel() const { return m_level;}

    // 设置日志器的级别
    void setLevel(LogLevel::Level val) { m_level = val;}

    // 返回日志器的名称
    const std::string& getName() const { return m_name;}

    // 设置日志器的日志格式
    void setFormatter(LogFormatter::ptr val);

    // 通过字符串设置日志器的日志格式
    void setFormatter(const std::string& val);

    // 返回日志器格式
    LogFormatter::ptr getFormatter();

    // 将日志器的配置转换成yaml格式
    std::string toYamlString();
private:
    // 日志器名称
    std::string m_name;
    // 日志器级别
    LogLevel::Level m_level;
    // Mutex
    MutexType m_mutex;
    // 日志输出目标集合
    std::list<LogAppender::ptr> m_appenders;
    // 日志器的格式
    LogFormatter::ptr m_formatter;
    // 主日志器
    Logger::ptr m_root;
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    // 写日志
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 将日志输出目标的配置转换成yaml
    std::string toYamlString() override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    // 构造函数，设置文件的名称
    FileLogAppender(const std::string& filename);

    // 输出器打印日志
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 将输出器的配置转换成yaml
    std::string toYamlString() override;

    // 重新打开文件
    bool reopen();
private:
    // 文件路径
    std::string m_filename;
    // 文件流
    std::ofstream m_filestream;
    // 上次重新打开时间
    uint64_t m_lastTime = 0;
};

// 日志器管理类
class LoggerManager {
public:
    typedef Spinlock MutexType;

    // 构造函数
    LoggerManager();

    // 通过日志器名称获得日志器
    Logger::ptr getLogger(const std::string& name);

    // 初始化
    void init();

    // 返回主日志器
    Logger::ptr getRoot() const { return m_root;}

    // 将日志管理器中的所有日志器配置转换成yaml
    std::string toYamlString();
private:
    // Mutex
    MutexType m_mutex;
    // 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    // 主日志器
    Logger::ptr m_root;
};

// 日志器管理类单例模式
typedef atpdxy::Singleton<LoggerManager> LoggerMgr;

}