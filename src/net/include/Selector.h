//
// Created by Administrator on 2020/9/12.
//

#ifndef NET_SELECTOR_H
#define NET_SELECTOR_H

#include "Poller.h"

#ifdef WIN32
namespace net{
    class Selector :public Poller{
        explicit Selector(EventBase* ev);
        ~Selector() override;
        Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
        void updateChannel(Channel* channel);



    };
}

#endif // WIN32





#endif //NET_SELECTOR_H
