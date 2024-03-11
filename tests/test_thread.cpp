#include "../atpdxy/atpdxy.h"
#include <vector>

atpdxy::Logger::ptr g_logger = LOG_ROOT();

void fun1() 
{
    LOG_INFO(g_logger) << "name: " << atpdxy::Thread::GetName()
        << " this.name: " << atpdxy::Thread::GetThis()->getName()
        << " id: " << atpdxy::GetThreadId()
        << " this.id: " << atpdxy::Thread::GetThis()->getId();
    pthread_t tid = pthread_self();
    LOG_INFO(g_logger) << "pthread_self: " << tid;
    pid_t pid = getpid();
    LOG_INFO(g_logger) << "滴滴滴滴滴滴滴滴 current pid is: " << pid;
    sleep(60);
}

void fun2() 
{

}

int main() {
    LOG_INFO(g_logger) << "thread test begin";

    pid_t pid = getpid();
    LOG_INFO(g_logger) << "current pid is: " << pid;
    std::vector<atpdxy::Thread::ptr> thrs;
    for (int i = 0; i < 5; ++i) {
        atpdxy::Thread::ptr thr(new atpdxy::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    
    for(int i = 0; i < 5; ++i)
    {
        thrs[i]->join();
    }
    LOG_INFO(g_logger) << "thread test end";
    return 0;
}
