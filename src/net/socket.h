//
// Created by Administrator on 2020/8/10.
//

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cstdint>
#include <cstdarg>
#include <cerrno>
#include <memory>
#include <vector>

#if defined(__MSVCRT__) || defined(_MSC_VER) || defined(WIN32)
#endif

#ifdef linux

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

#ifdef apple
#endif


namespace net {

#define NET_OK 0
#define NET_ERR -1
#define ERR_MSG_LEN 256

    static void NetSetError(char *err, const char *fmt, ...) {
        va_list ap;

        if (!err) return;
        va_start(ap, fmt);
        vsnprintf(err, ERR_MSG_LEN, fmt, ap);
        va_end(ap);
    }

    class Socket {
    public:
        explicit Socket(int af, int sock_type, int protocol);

        virtual ~Socket();

        virtual int Init(char *err_msg){};

        virtual int Read(char *buf, size_t buff_len){};

        virtual int Write(char *buf, size_t count){};

        virtual int SetNonBlock(char *err_msg){};

        virtual int SetBlock(char *err_msg){};

        virtual int SetReuseAddr(char *err_msg);

        int m_fd{};
    };

    using SocketPTR = std::shared_ptr<Socket>;

    class Server {
    public:
        Server() = default;

        virtual int Listen(SocketPTR, char *err_msg) = 0;

        virtual int Accept(char *err_msg) = 0;
    };

    class Client {
        Client();

        int Connect();
    };


    class TCPServer  {

    public:
        TCPServer(int port, const char *bind_addr);

        int Listen(const struct addrinfo *addr,char *err_msg) ;

        int Accept(char *err_msg) ;

    private:
        int m_port;

        SocketPTR m_socket;
    };

    class TCPClient : public Client {
        TCPClient();
    };
}


#endif //NET_SOCKET_H
