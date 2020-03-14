//
// Created by simon on 2020/3/14.
//

#include "../include/Dispatcher.h"
SZ::Dispatcher::~Dispatcher() {

}

void SZ::Dispatcher::AddServer(TcpConnection *svr) {
    _servers.push_back(svr);
}