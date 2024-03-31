#include <iostream>
#include "../atpdxy/log.h"
#include "../atpdxy/util.h"

int main(int argc, char** argv) {
    atpdxy::Logger::ptr logger(new atpdxy::Logger);
    logger->addAppender(atpdxy::LogAppender::ptr(new atpdxy::StdoutLogAppender));

    atpdxy::FileLogAppender::ptr file_appender(new atpdxy::FileLogAppender("./log.txt"));
    atpdxy::LogFormatter::ptr fmt(new atpdxy::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(atpdxy::LogLevel::ERROR);

    logger->addAppender(file_appender);

    std::cout << "hello atpdxy log" << std::endl;

    INFO(logger) << "test macro";
    ERROR(logger) << "test macro error";

    FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = atpdxy::LoggerMgr::GetInstance()->getLogger("xx");
    INFO(l) << "xxx";
    return 0;
}