#include "../atpdxy/atpdxy.h"
#include "../atpdxy/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

int sock = 0;

void test_fiber() {
    INFO(g_logger) << "test fiber sock=" << sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);
    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
        
    } else if(errno == EINPROGRESS) {
        INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        atpdxy::IOManager::GetThis()->addEvent(sock, atpdxy::IOManager::READ, [](){
            INFO(g_logger) << "read callback";
        });
        atpdxy::IOManager::GetThis()->addEvent(sock, atpdxy::IOManager::WRITE, [](){
            INFO(g_logger) << "write callback";
        });
    } else {
        INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test1() {
    std::cout << "EPOLLIN=" << EPOLLIN << " EPOLLOUT=" << EPOLLOUT << std::endl;
    atpdxy::IOManager iom(2, false);
    iom.schedule(&test_fiber);
}

atpdxy::Timer::ptr s_timer;
void testTimer() {
    atpdxy::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        INFO(g_logger) << "hello timer i = " << i;
         if(++i == 3) {
            //s_timer->reset(2000, true);
            s_timer->cancel();
         }
    }, true);
}

int main() {
    // test1();
    testTimer();
    return 0;
}