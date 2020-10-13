#include <iostream>
#include <unistd.h>
#include <thread>
#include <string>
#include <cstring>

#include "Channel.h"
#include "ChannelBus.h"

using namespace std;
using namespace gochan;

int main() {

    Channel chan(ChannelBus::getInstance());
    ReadCallback cb = [](Channel *c) -> void {
        cout << "ReadCallback" << endl;
        void *tmp = c->pop();
        if (tmp == CLOSE_SIGNAL) {
            cout << "receive close signal!" << endl;
            return;
        } else {
            cout << "read:" << *static_cast<int *>(tmp) << endl;
            free(tmp);
        }
    };
    chan.registerMsgCallback(cb);
    thread th([&]() -> void {
        for (int i = 0; i < 3;) {
            int *tmp = new int(i);
            chan.push(tmp);
            EVENT_SIGNAL val;
            size_t n = write(chan.fd(), &val, sizeof val);
            if (n > 0) {
                ++i;
            } else {
                cout << "errno:" << errno << " errmsg:" << std::strerror(errno) << endl;
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
        chan.close();
    });

    ChannelBus::getInstance()->run();
    th.join();


    return 0;
}