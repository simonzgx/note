//
// Created by simon on 2020/3/14.
//

#ifndef SAMPLE_ECHO_SERVER_EPOLLER_H
#define SAMPLE_ECHO_SERVER_EPOLLER_H

#include "TcpConnection.h"

namespace SZ {


    class Epoller :public Dispatcher{
    public:
        Epoller();
        ~Epoller();
        void DoEvent();

    protected:
        void CreateEvent(int fd, unsigned char socket_event);

    private:
        int _fd;
    };
}


#endif //SAMPLE_ECHO_SERVER_EPOLLER_H
