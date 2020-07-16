//
// Created by simon on 2020/7/16.
//

#include "util.h"

static void util::SetError(char *err, const char *fmt, ...)
{
    va_list ap;

    if (!err) return;
    va_start(ap, fmt);
    vsnprintf(err, NET_ERR_LEN, fmt, ap);
    va_end(ap);
}

static int util::SetReuseAddr(char *err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        util::SetError(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return NET_ERR;
    }
    return NET_OK;
}

static int util::IPV6Only(char *err, int s) {
    int yes = 1;
    if (setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,&yes,sizeof(yes)) == -1) {
        util::SetError(err, "setsockopt: %s", strerror(errno));
        close(s);
        return NET_ERR;
    }
    return NET_OK;
}

static int util::Listen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog) {
    if (bind(s,sa,len) == -1) {
        util::SetError(err, "bind: %s", strerror(errno));
        close(s);
        return NET_ERR;
    }

    if (listen(s, backlog) == -1) {
        util::SetError(err, "listen: %s", strerror(errno));
        close(s);
        return NET_ERR;
    }
    return NET_OK;
}

static int util::TcpServer(char *err, int port, char *bindaddr, int af, int backlog)
{
    int s = -1, rv;
    char _port[6];  /* strlen("65535") */
    struct addrinfo hints{}, *servinfo, *p;

    snprintf(_port,6,"%d",port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */

    if ((rv = getaddrinfo(bindaddr,_port,&hints,&servinfo)) != 0) {
        util::SetError(err, "%s", gai_strerror(rv));
        return NET_ERR;
    }
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && IPV6Only(err,s) == NET_ERR) goto error;
        if (SetReuseAddr(err,s) == NET_ERR) goto error;
        if (util::Listen(err,s,p->ai_addr,p->ai_addrlen,backlog) == NET_ERR) s = NET_ERR;
        goto end;
    }
    util::SetError(err, "unable to bind socket, errno: %d", errno);

    error:
    if (s != -1) close(s);
    s = NET_ERR;
    end:
    freeaddrinfo(servinfo);
    return s;
}
