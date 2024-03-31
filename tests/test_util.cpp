#include "../atpdxy/atpdxy.h"
#include <assert.h>

atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

void test_assert() {
    INFO(g_logger) << atpdxy::BacktraceToString(10);
    ASSERT_WITH_MSG(0 == 1, "abcdef xx");
}

int main(int argc, char** argv) {
    test_assert();

    // int arr[] = {1,3,5,7,9,11};

    // INFO(g_logger) << atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 0);
    // INFO(g_logger) << atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 1);
    // INFO(g_logger) << atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 4);
    // INFO(g_logger) << atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 13);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 0) == -1);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 1) == 0);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 2) == -2);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 3) == 1);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 4) == -3);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 5) == 2);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 6) == -4);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 7) == 3);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 8) == -5);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 9) == 4);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 10) == -6);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 11) == 5);
    // ASSERT(atpdxy::BinarySearch(arr, sizeof(arr) / sizeof(arr[0]), 12) == -7);
    return 0;
}