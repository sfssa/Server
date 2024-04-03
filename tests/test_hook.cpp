#include "../atpdxy/hook.h"
#include "../atpdxy/log.h"
#include "../atpdxy/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// 通过hook实现了阻塞线程，在该线程中，调用sleep应当共阻塞5秒，而通过让出执行权并添加定时器的方法，一共阻塞三秒即可
// 简单来说，调用sleep函数:start=>sleep(2)=>sleep(3);
// 使用hook：start=>sleep(2)
//               =>sleep(3)
atpdxy::Logger::ptr g_logger = GET_ROOT_LOGGER();

void testSleep() {
    atpdxy::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        INFO(g_logger) << "sleep 2";
    });
    iom.schedule([](){
        sleep(3);
        INFO(g_logger) << "sleep 3";
    });
}

void testSock() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "36.155.132.76", &addr.sin_addr.s_addr);

    INFO(g_logger) << "begin connect";
    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    INFO(g_logger) << "connect rt=" << rt << " errno=" << errno;

    if(rt) {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    INFO(g_logger) << "send rt=" << rt << " errno=" << errno;

    if(rt <= 0) {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    INFO(g_logger) << "recv rt=" << rt << " errno=" << errno;

    if(rt <= 0) {
        return;
    }

    buff.resize(rt);
    INFO(g_logger) << buff;
}

void testFiberNum() {
    atpdxy::IOManager iom(1);
    iom.schedule([](){
        INFO(g_logger) << "Hello World!";
    });
}

int main() {
    // testSleep();
    // testSock();
    // atpdxy::IOManager iom;
    // iom.schedule(testSock);
    testFiberNum();
    return 0;
}