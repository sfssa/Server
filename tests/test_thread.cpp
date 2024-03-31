#include "../atpdxy/atpdxy.h"
#include <unistd.h>

atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

int count = 0;
//atpdxy::RWMutex s_mutex;
atpdxy::Mutex s_mutex;

void fun1() {
    INFO(g_logger) << "name: " << atpdxy::Thread::GetName()
        << " this.name: " << atpdxy::Thread::GetThis()->getName()
        << " id: " << atpdxy::GetThreadId()
        << " this.id: " << atpdxy::Thread::GetThis()->getId();

    for(int i = 0; i < 100000; ++i) {
        //atpdxy::RWMutex::WriteLock lock(s_mutex);
        atpdxy::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while(true) {
        INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        INFO(g_logger) << "========================================";
    }
}

int main(int argc, char** argv) {
    INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/atpdxy/GitHub/Server/bin/conf/logs2.yml");
    atpdxy::Config::LoadFromYaml(root);

    std::vector<atpdxy::Thread::ptr> thrs;
    for(int i = 0; i < 1; ++i) {
        atpdxy::Thread::ptr thr(new atpdxy::Thread(&fun2, "name_" + std::to_string(i * 2)));
        atpdxy::Thread::ptr thr2(new atpdxy::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    INFO(g_logger) << "thread test end";
    INFO(g_logger) << "count=" << count;

    return 0;
}