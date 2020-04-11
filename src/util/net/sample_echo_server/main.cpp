#include <iostream>
#include <string>
#include "include/Poller.h"
#include "include/Epoller.h"
#include "include/EchoServer.h"

class EchoSession : public SZ::TcpSession {
public:
    EchoSession(int fd, const char *ip, int port, SZ::TcpConnection *pServer)
            : SZ::TcpSession(fd, ip, port, pServer) {

    }

    void OnRecv(const char *buf, int len) override {
        std::cout << "recive from client:" << buf;
        std::string s = buf;
        Send(("ACK:" + s).c_str(), len);
    }
};

int main() {
//    SZ::Dispatcher *poller = new SZ::Poller();
    SZ::Dispatcher *poller = new SZ::Epoller();
    SZ::EchoServer echo_server(poller);
    echo_server.start<EchoSession>("echo server", 8080);

    while (1) {
        poller->DoEvent();
    }

    return 0;
}
