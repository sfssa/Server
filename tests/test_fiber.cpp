#include "../atpdxy/fiber.h"
#include "../atpdxy/atpdxy.h"

atpdxy::Logger::ptr g_logger = LOG_ROOT();

void run_in_fiber()
{
    LOG_INFO(g_logger) << "run in fiber begin";
    atpdxy::Fiber::YieldToHold();
    LOG_INFO(g_logger) << "run in fiber end";
    atpdxy::Fiber::YieldToHold();
}

int main()
{
    LOG_INFO(g_logger) << "main begin -1";
    {
        atpdxy::Fiber::GetThis();
        LOG_INFO(g_logger) << "main begin";
        atpdxy::Fiber::ptr fiber(new atpdxy::Fiber(run_in_fiber));
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    LOG_INFO(g_logger) << "main after end2";
    return 0;
}