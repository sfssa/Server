#include "../atpdxy/atpdxy.h"
#include <assert.h>

atpdxy::Logger::ptr g_logger = LOG_ROOT();

void test()
{
    LOG_INFO(g_logger) << atpdxy::BacktraceToString(10, 2, "    ");
    // ATPDXY_ASSERT(false);
    ATPDXY_ASSERT2(false, "Hello World!");
}

int main()
{
    test();
    return 0;
}