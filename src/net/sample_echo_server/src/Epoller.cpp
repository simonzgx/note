//
// Created by simon on 2020/3/14.
//

#include <algorithm>
#include "../include/Epoller.h"

SZ::Epoller::Epoller() {
    _fd = epoll_create(1024);
    if (_fd == -1) {
        DEBUG_LOG_INFO__("epoll_create failed");
    }
}

SZ::Epoller::~Epoller() {
    close(_fd);
}

void SZ::Epoller::CreateEvent(int fd, unsigned char socket_event) {
    struct epoll_event ev{};
    ev.events = 0;
    if ((socket_event & SOCKET_READ) != 0) {
        ev.events |= EPOLLIN;
    }

    if ((socket_event & SOCKET_WRITE) != 0) {
        ev.events |= EPOLLOUT;
    }

    if ((socket_event & SOCKET_EXCEP) != 0) {
        ev.events |= EPOLLERR;
    }
    ev.data.fd = fd;
    if (epoll_ctl(_fd, EPOLL_CTL_ADD, fd, &ev) != 0) {
        DEBUG_LOG_INFO__("epoll_ctl() failed, errno=%d", errno);
    }
}

void SZ::Epoller::DoEvent() {
    struct epoll_event revs[1024];
    int n = sizeof(revs) / sizeof(revs[0]);
    int timeout = 1000;
    int num = 0;


    num = epoll_wait(_fd, revs, n, timeout);
    if (num <= -1) {
        //LOG__(NET,  "select failed, error code: %d\n", GetLastError());
        return;
    }
    if (num == 0) {
        return;
    }

    //struct epoll_event ev;
    int i = 0;
    for (; i < num; i++) {
        int fd = revs[i].data.fd;

        if (revs[i].events & EPOLLIN) {
            auto it = std::find_if(_servers.begin(), _servers.end(), [=](auto &item) {
                return item->_sock == fd;
            });

            if (it != _servers.end()) {
                (*it)->OnAccept();
            } else {
                for (auto &item : _servers) {
                    item->OnRead(fd);
                }
            }
        }

        //
        if (revs[i].events & EPOLLOUT) {
            for (auto &item : _servers) {
                item->OnWrite(fd);
            }
        }

        //
        if (revs[i].events & EPOLLERR) {
            for (auto &item : _servers) {
                item->OnError(fd);
            }
        }
    }
}
