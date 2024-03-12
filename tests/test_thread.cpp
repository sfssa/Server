#include "../atpdxy/atpdxy.h"
#include <vector>

atpdxy::Logger::ptr g_logger = LOG_ROOT();
atpdxy::RWMutex s_mutex;
atpdxy::Mutex mutex;
// 加volatile关键字取消编译器优化
volatile int count = 0;

void fun1() 
{
    LOG_INFO(g_logger) << "name: " << atpdxy::Thread::GetName()
        << " this.name: " << atpdxy::Thread::GetThis()->getName()
        << " id: " << atpdxy::GetThreadId()
        << " this.id: " << atpdxy::Thread::GetThis()->getId();
    // pthread_t tid = pthread_self();
    // LOG_INFO(g_logger) << "pthread_self: " << tid;
    // pid_t pid = getpid();
    // LOG_INFO(g_logger) << "滴滴滴滴滴滴滴滴 current pid is: " << pid;
    // sleep(6);
    for(int i = 0; i < 10000000; ++i)
    {
        // atpdxy::RWMutex::WriteLock lock(s_mutex);
        // atpdxy::RWMutex::ReadLock lock(s_mutex);
        atpdxy::Mutex::Lock lock(mutex);
        ++count;
    }
}

void fun2() 
{
    while(1)
    {
        LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3()
{
    while(1)
    {
        LOG_INFO(g_logger) << "==================================================";
    }
}
int main() {
    LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/atpdxy/Github/Server/bin/conf/logs2.yml");
    atpdxy::ConfigManager::LoadFromYaml(root);
    int thread_num = 2;
    // pid_t pid = getpid();
    // LOG_INFO(g_logger) << "current pid is: " << pid;
    std::vector<atpdxy::Thread::ptr> thrs;
    for (int i = 0; i < thread_num; ++i) {
        atpdxy::Thread::ptr thr1(new atpdxy::Thread(&fun2, "name_" + std::to_string(i * 2)));
        atpdxy::Thread::ptr thr2(new atpdxy::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }
    
    for(int i = 0; i < thread_num; ++i)
    {
        thrs[i]->join();
    }
    LOG_INFO(g_logger) << "thread test end";

    LOG_INFO(g_logger) << "final count is: " << count;

    return 0;
}
