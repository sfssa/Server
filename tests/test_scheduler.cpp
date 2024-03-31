#include "../atpdxy/atpdxy.h"

static atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

void test_fiber() {
    static int s_count = 5;
    INFO(g_logger) << "test in fiber s_count=" << s_count;

    sleep(1);
    if(--s_count >= 0) {
        atpdxy::Scheduler::GetThis()->schedule(&test_fiber, atpdxy::GetThreadId());
    }
}

int main(int argc, char** argv) {
    INFO(g_logger) << "main";
    atpdxy::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    INFO(g_logger) << "over";
    return 0;
}