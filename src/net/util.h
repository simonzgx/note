//
// Created by simon on 2020/7/16.
//

#ifndef NET_UTIL_H
#define NET_UTIL_H

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <ctime>
#include <cstdio>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cerrno>
#include <unistd.h>
#include <cstdarg>

#define NET_OK 0
#define NET_ERR -1
#define NET_ERR_LEN 256

namespace util{
    static void SetError(char *err, const char *fmt, ...);

    static int SetReuseAddr(char *err, int fd);

    static int IPV6Only(char *err, int s);

    static int Listen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog);

    static int TcpServer(char *err, int port, char *bindaddr, int af, int backlog);
}




#endif //NET_UTIL_H
