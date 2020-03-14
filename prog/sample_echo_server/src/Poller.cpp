//
// Created by simon on 2020/3/14.
//

#include "../include/Poller.h"

SZ::Poller::Poller() {

}

void SZ::Poller::DoEvent() {

    int nfds = poll(_poolfds.data(), _poolfds.size(), 10000);
    if (nfds <= -1) {
        //LOG__(NET,  "select failed, error code: %d\n", GetLastError());
        return;
    }

    if (nfds == 0) {
        return;
    }

    for (pollfd &_pollfd : _poolfds) {
        if (_pollfd.revents & POLLIN) {
            auto it = std::find_if(_servers.begin(), _servers.end(), [=](auto *item) {
                return item->_sock == _pollfd.fd;
            });

            if (it != _servers.end()) {
                (*it)->OnAccept();
            } else {
                for (auto &item : _servers) {
                    item->OnRead(_pollfd.fd);
                }
            }
        }

        if (_pollfd.revents & POLLOUT) {
            for (auto &item : _servers) {
                item->OnWrite(_pollfd.fd);
            }
        }

        if (_pollfd.revents & POLLERR) {
            for (auto &item : _servers) {
                item->OnError(_pollfd.fd);
            }
        }
    }
}

void SZ::Poller::CreateEvent(int fd, uint8_t socket_event) {
    auto it = std::find_if(_poolfds.begin(), _poolfds.end(), [=](pollfd &item) {
        return fd == item.fd;
    });

    short setEvent = 0;

    if ((socket_event & SOCKET_READ) != 0) {
        setEvent |= POLLIN;
    }

    if ((socket_event & SOCKET_WRITE) != 0) {
        setEvent |= POLLOUT;
    }

    if ((socket_event & SOCKET_EXCEP) != 0) {
        setEvent |= POLLERR;
    }

    if (it != _poolfds.end()) {
        it->fd = fd;
        it->events = setEvent;
    } else {
        struct pollfd item;
        item.fd = fd;
        item.events = setEvent;
        _poolfds.push_back(item);
    }
}
