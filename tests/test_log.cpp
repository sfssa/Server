#include <iostream>
#include "../atpdxy/log.h"
#include "../atpdxy/util.h"

int main()
{
    atpdxy::Logger::ptr logger(new atpdxy::Logger("pzx"));
    logger->addAppender(atpdxy ::LogAppender::ptr(new atpdxy::StdoutLogAppender));

    atpdxy::FileLogAppender::ptr file_appender(new atpdxy::FileLogAppender("./log.txt"));
    atpdxy::LogFormatter::ptr fmt(new atpdxy::LogFormatter("%d%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(atpdxy::LogLevel::ERROR);
    logger->addAppender(file_appender);
    
    std::cout << "Hello Log!" << std::endl;
    std::cout << "-------------------------" << std::endl;
    LOG_INFO(logger) << "test info";
    LOG_ERROR(logger) << "test error";

    LOG_FMT_ERROR(logger, "test fmt error %s", "atpdxy");

    auto l = atpdxy::LoggerMgr::GetInstance()->getLogger("xx");
    LOG_INFO(l) << "xxx";
    return 0;
}