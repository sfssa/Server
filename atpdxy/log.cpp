#include "log.h"
#include "config.h"

namespace atpdxy
{
const char* LogLevel::ToString(LogLevel::Level level)
{
    switch(level)
    {
    #define XX(name) \
        case LogLevel::name: \
            return #name; \
            break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
    
    #undef XX
    default:
        return "UNKNOW";
    }
}

LogLevel::Level LogLevel::FromString(const std::string& str)
{
    #define XX(level, v) \
    { \
        if(str == #v) \
        { \
            return LogLevel::level; \
        } \
    }

    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
    #undef XX
}

LogEventWrap::LogEventWrap(LogEvent::ptr e)
    :m_event(e)
{

}


LogEventWrap::~LogEventWrap()
{
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}


std::stringstream& LogEventWrap::getSS()
{
    return m_event->getSS();
}

// 消息体item
class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getContent();
    }
};

// 日志级别item
class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    LevelFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << LogLevel::ToString(level);
    }
};

// 字符串item
class StringFormatItem : public LogFormatter::FormatItem
{
public:
    StringFormatItem(const std::string& str)
        :m_string(str) 
    {

    }

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << m_string;
    }
private:
    std::string m_string;
};

// 启动时间item
class ElapseFormatItem : public LogFormatter::FormatItem
{
public:
    ElapseFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getElapse();
    }
};

// 日志器名称
class NameFormatItem : public LogFormatter::FormatItem
{
public:
    NameFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getLogger()->getName();
    }
};

// 线程名item
class ThreadNameFormatItem : public LogFormatter::FormatItem 
{
public:
    ThreadNameFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override 
    {
        os << event->getThreadName();
    }
};

// 线程id
class ThreadIdFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadIdFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getThreadId();
    }
};

// 协程id
class FiberIdFormatItem : public LogFormatter::FormatItem
{
public:
    FiberIdFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getFiberId();
    }
};

// 时间item
class DateTimeFormatItem : public LogFormatter::FormatItem
{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format)
    {
        // 这里的赋值代码不可省略，如果用户传递的参数为空，那么空字符串会覆盖默认的格式
        if(m_format.empty())
        {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    // 时间格式
    std::string m_format;
};

// 文件名item
class FileNameFormatItem : public LogFormatter::FormatItem
{
public:
    FileNameFormatItem(const std::string& str = "") {}
    
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getFile();
    }
};

// 行号item
class LineFormatItem : public LogFormatter::FormatItem
{
public:
    LineFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->getLine();
    }
};

// 制表符item
class TabFormatItem : public LogFormatter::FormatItem 
{
public:
    TabFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override 
    {
        os << "\t";
    }
private:
    std::string m_string;
};


// 新行号item
class NewLineFormatItem : public LogFormatter::FormatItem
{
public:
    NewLineFormatItem(const std::string& str = "") {}

    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << std::endl;
    }
};

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
    const char* file, int32_t line, uint32_t elapse, uint32_t threadId, 
    uint32_t fiberId, uint64_t time, const std::string& thread_name)
    :m_file(file)
    ,m_line(line)
    ,m_elapse(elapse)
    ,m_threadId(threadId)
    ,m_fiberId(fiberId)
    ,m_time(time)
    ,m_logger(logger)
    ,m_level(level)
    ,m_threadName(thread_name)
{

}

void LogEvent::format(const char* fmt, ...)
{
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al)
{
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1)
    {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

std::string Logger::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOW) 
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    else 
    {
        node["level"] = "UNKNOW";
    }
    if(m_formatter)
    {

        node["formatter"] = m_formatter->getPattern();
    }
    for(auto& i : m_appenders)
    {
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

LogFormatter::ptr LogAppender::getFormatter()
{
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

void LogAppender::setFormatter(LogFormatter::ptr val)
{
    MutexType::Lock lock(m_mutex);
    m_formatter = val;
    if(m_formatter)
    {
        m_hasFormatter = true;
    }
    else
    {
        m_hasFormatter = false;
    }
}

std::string StdoutLogAppender::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOW) 
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    else
    {
        node["level"] = "UNKNOW";
    }
    if(m_hasFormatter && m_formatter)
    {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

std::string LoggerManager::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    for(auto& i : m_loggers)
    {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

std::string FileLogAppender::toYamlString()
{
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOW) 
    {
        node["level"] = LogLevel::ToString(m_level);
    }
    else
    {
        node["level"] = "UNKNOW";
    }
    if(m_hasFormatter && m_formatter)
    {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

Logger::Logger(const std::string& name)
    :m_name(name)
    ,m_level(LogLevel::DEBUG)
{
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::setFormatter(LogFormatter::ptr val)
{
    MutexType::Lock lock(m_mutex);
    m_formatter = val;
    for(auto& i : m_appenders)
    {
        MutexType::Lock ll(i->m_mutex);
        // 输出器没有自己的格式则将logger的格式赋值给输出器
        if(!i->m_hasFormatter)
        {
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val)
{
    atpdxy::LogFormatter::ptr new_val(new atpdxy::LogFormatter(val));
    if(new_val->isError())
    {
        std::cout << "Logger setFormatter name = " << m_name 
            <<" value = " << val << " invalid formatter"<< std::endl;
        return;
    }
    // m_formatter = new_val;
    setFormatter(new_val);
}

LogFormatter::ptr Logger::getFormatter()
{
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

void Logger::addAppender(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    if(!appender->getFormatter())
    {
        MutexType::Lock ll(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
    {
        if(*it == appender)
        {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders()
{
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    // 级别高于日志器本身的日志才会被输出
    if(level >= m_level)
    {
        auto self = shared_from_this();
        MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty())
        {
            for(auto& i : m_appenders)
            {
                i->log(self, level, event);
            }
        }
        else if(m_root)
        {
            m_root->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event)
{
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event)
{
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event)
{
    log(LogLevel::WARN, event);
}
    
void Logger::error(LogEvent::ptr event)
{
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event)
{
    log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string& name)
    :m_filename(name)
{
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        uint64_t now = event->getTime();
        if(now >= (m_lastTime + 3))
        {
            reopen();
            m_lastTime = now;
        }
        MutexType::Lock lock(m_mutex);
        if(!(m_filestream << m_formatter->format(logger, level, event)))
        {
            std::cout << "error" << std::endl;
        }
    }
}

bool FileLogAppender::reopen()
{
    MutexType::Lock lock(m_mutex);
    if(m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios_base::app);
    // !!的作用是将非0值转换成1；而0还是0
    return !!m_filestream;
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        MutexType::Lock lock(m_mutex);
        std::cout << m_formatter->format(logger, level, event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern)
{
    init();
}
std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items)
    {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

void LogFormatter::init() {
    std::vector<std::tuple<std::string, std::string, int> > vec;
    vec.reserve(m_pattern.size() / 2);

    std::string nstr;
    size_t i = 0;
    while (i < m_pattern.size()) {
        size_t pos = m_pattern.find('%', i);
        if (pos == std::string::npos) {
            nstr.append(m_pattern.substr(i));
            break;
        }
        
        nstr.append(m_pattern.substr(i, pos - i));
        i = pos + 1;

        if (i < m_pattern.size() && m_pattern[i] == '%') {
            nstr.append(1, '%');
            ++i;
            continue;
        }

        size_t n = i;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {
                str = m_pattern.substr(i, n - i);
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(i, n - i);
                    fmt_status = 1;
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()) {
                    str = m_pattern.substr(i);
                }
            }
        }

        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.emplace_back(nstr, std::string(), 0);
                nstr.clear();
            }
            vec.emplace_back(str, fmt, 1);
            i = n;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.emplace_back("<<pattern_error>>", fmt, 0);
        }
    }

    if (!nstr.empty()) {
        vec.emplace_back(nstr, "", 0);
    }

    static std::unordered_map<std::string, std::function<FormatItem::ptr(const std::string&)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FileNameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem),
        XX(F, FiberIdFormatItem),
        XX(N, ThreadNameFormatItem),
#undef XX
    };

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_items.emplace_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.emplace_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.emplace_back(it->second(std::get<1>(i)));
            }
        }
    }
}

Logger::ptr LoggerManager::getLogger(const std::string& name)
{
    MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end())
    {
        return it->second;
    }
    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

LoggerManager::LoggerManager()
{
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    m_loggers[m_root->m_name] = m_root;
    init();
}

struct LogAppenderDefine
{
    // 1-file;2-std
    int type = 0;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const
    {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine
{
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const
    {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const
    {
        return name < oth.name;
    }
};

template<>
class LexicalCast<std::string, LogDefine> 
{
public:
    LogDefine operator()(const std::string& v) 
    {
        YAML::Node n = YAML::Load(v);
        LogDefine ld;
        if(!n["name"].IsDefined()) 
        {
            std::cout << "log config error: name is null, " << n << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = n["name"].as<std::string>();
        ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
        if(n["formatter"].IsDefined()) 
        {
            ld.formatter = n["formatter"].as<std::string>();
        }

        if(n["appenders"].IsDefined()) 
        {
            for(size_t x = 0; x < n["appenders"].size(); ++x) 
            {
                auto a = n["appenders"][x];
                if(!a["type"].IsDefined()) 
                {
                    std::cout << "log config error: appender type is null, " << a << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();
                LogAppenderDefine lad;
                if(type == "FileLogAppender") 
                {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) 
                    {
                        std::cout << "log config error: fileappender file is null, " << a << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                    if(a["formatter"].IsDefined()) 
                    {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } 
                else if(type == "StdoutLogAppender") 
                {
                    lad.type = 2;
                    if(a["formatter"].IsDefined()) 
                    {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } 
                else 
                {
                    std::cout << "log config error: appender type is invalid, " << a << std::endl;
                    continue;
                }

                ld.appenders.push_back(lad);
            }
        }
        return ld;
    }
};

template<>
class LexicalCast<LogDefine, std::string> 
{
public:
    std::string operator()(const LogDefine& i) 
    {
        YAML::Node n;
        n["name"] = i.name;
        if(i.level != LogLevel::UNKNOW) 
        {
            n["level"] = LogLevel::ToString(i.level);
        }
        if(!i.formatter.empty()) 
        {
            n["formatter"] = i.formatter;
        }

        for(auto& a : i.appenders) 
        {
            YAML::Node na;
            if(a.type == 1) 
            {
                na["type"] = "FileLogAppender";
                na["file"] = a.file;
            } 
            else if(a.type == 2) 
            {
                na["type"] = "StdoutLogAppender";
            }
            if(a.level != LogLevel::UNKNOW) 
            {
                na["level"] = LogLevel::ToString(a.level);
            }

            if(!a.formatter.empty()) 
            {
                na["formatter"] = a.formatter;
            }

            n["appenders"].push_back(na);
        }
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
};

atpdxy::Config<std::set<LogDefine>>::ptr g_log_defines =
    atpdxy::ConfigManager::Lookup("logs", std::set<LogDefine>(), "logs config");

// 根据配置文件创建对应的日志器
struct LogIniter 
{
    LogIniter() 
    {
        g_log_defines->addListener([](const std::set<LogDefine>& old_value,
                    const std::set<LogDefine>& new_value){
            LOG_INFO(LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_value) 
            {
                auto it = old_value.find(i);
                atpdxy::Logger::ptr logger;
                if(it == old_value.end()) 
                {
                    //新增logger
                    logger = LOG_NAME(i.name);
                } else 
                {
                    if(!(i == *it)) 
                    {
                        //修改的logger
                        logger = LOG_NAME(i.name);
                    } 
                    else 
                    {
                        continue;
                    }
                }
                logger->setLevel(i.level);
                if(!i.formatter.empty()) 
                {
                    logger->setFormatter(i.formatter);
                }

                logger->clearAppenders();
                for(auto& a : i.appenders) 
                {
                    atpdxy::LogAppender::ptr ap;
                    if(a.type == 1) 
                    {
                        ap.reset(new FileLogAppender(a.file));
                    } 
                    else if(a.type == 2) 
                    {
                        ap.reset(new StdoutLogAppender);
                    }
                    ap->setLevel(a.level);
                    if(!a.formatter.empty()) 
                    {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->isError()) 
                        {
                            ap->setFormatter(fmt);
                        } 
                        else 
                        {
                            std::cout << "log.name = " << i.name << " appender type = " << a.type
                                      << " formatter = " << a.formatter << " is invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);
                }
            }

            for(auto& i : old_value) 
            {
                auto it = new_value.find(i);
                if(it == new_value.end()) 
                {
                    //删除logger
                    auto logger = LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)0);
                    logger->clearAppenders();
                }
            }
        });
    }
};

// 在main函数前初始化
static LogIniter __log_init;

void LoggerManager::init()
{
    
}
}