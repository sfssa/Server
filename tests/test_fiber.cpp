#include "../atpdxy/atpdxy.h"

atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

void run_in_fiber() {
    INFO(g_logger) << "run_in_fiber begin";
    atpdxy::Fiber::YieldToHold();
    INFO(g_logger) << "run_in_fiber end";
    atpdxy::Fiber::YieldToHold();
}

void test_fiber() {
    INFO(g_logger) << "main begin -1";
    {
        atpdxy::Fiber::GetThis();
        INFO(g_logger) << "main begin";
        atpdxy::Fiber::ptr fiber(new atpdxy::Fiber(run_in_fiber));
        fiber->swapIn();
        INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    INFO(g_logger) << "main after end2";
}

int main(int argc, char** argv) {
    atpdxy::Thread::SetName("main");

    std::vector<atpdxy::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i) {
        thrs.push_back(atpdxy::Thread::ptr(
            new atpdxy::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }
    return 0;
}