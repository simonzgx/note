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

#if defined(__linux__)

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#endif

#ifdef apple
#endif

#define NET_OK 0
#define NET_ERR -1
#define ERR_MSG_LEN 256
#define LOCAL_HOST "127.0.0.1"

#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

namespace net {

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

        static Socket* New(int domain, char *err_msg);

        virtual ~Socket();

        int SetNonBlock(char *err_msg);

        int SetBlock(char *err_msg, int non_block = 0);

        int SetReuseAddr(char *err_msg);

    private:
        explicit Socket(int domain) : m_sfd(domain) {};

    public:
        int m_sfd{};
    };

    using SocketPTR = std::shared_ptr<Socket>;


    class TCPServer {

    public:
        TCPServer(int port, const char *bind_addr=LOCAL_HOST);

        int SetKeepAlive(int fd, int interval, char *err_msg);

        int Listen(const struct addrinfo *addr, char *err_msg);

        int Accept(char *err_msg);

    private:
        int m_port;

        SocketPTR m_socket;
    };

    int GenericResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len,
                       int flags);

    int Resolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);

    int ResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);

    int Read(int fd, char *buf, size_t count);

    int Write(int fd, const char *buf, size_t count);
}


#endif //NET_SOCKET_H
