#include "thread.h"

namespace atpdxy
{
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";
Thread* Thread::GetThis()
{
    return t_thread;
}

const std::string& Thread::GetName()
{
    return t_thread_name;
}

void Thread::SetName(const std::string& name)
{
    if(t_thread)
    {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
{
    if(name.empty())
    {
        
    }
}

Thread::~Thread()
{

}

void Thread::join()
{

}

}