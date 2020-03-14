#include <iostream>
#include "include/Poller.h"
#include "include/EchoServer.h"

class EchoSession : public SZ::TcpSession {
public:
    EchoSession(int fd, const char *ip, int port, SZ::TcpConnection *pServer)
    :SZ::TcpSession(fd, ip, port, pServer)
    {

    }

    void OnRecv(const char *buf, int len) override {
        std::cout<<("[%s] recv %s\n", _remote_ip.c_str(), buf);
        Send(buf, len);
    }
};

int main() {
    SZ::Dispatcher *poll = new SZ::Poller();

    SZ::EchoServer echo_server(poll);
    echo_server.start<EchoSession>("echo server", 8080);

    while (1)
    {
        poll->DoEvent();
    }

    return 0;
}
