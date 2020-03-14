//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_POLLER_H
#define SAMPLE_ECHO_SERVER_POLLER_H

#include <poll.h>
#include <map>
#include <vector>
#include <algorithm>
#include "Dispatcher.h"

namespace SZ {


    class Poller : public Dispatcher {
    public:
        Poller();

        ~Poller() = default;

        void DoEvent() override;

    protected:
        void CreateEvent(int fd, uint8_t socket_event) override;

    private:
        std::vector<struct pollfd> _poolfds;
    };
}

#endif //SAMPLE_ECHO_SERVER_POLLER_H
