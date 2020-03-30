//
// Created by simon on 2020/3/29.
//

#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include "rwlcok.h"

using namespace std;

int num;
RWLock rwLock;

void read_num(int threadID, int sleepTime) {

    cout << "read thread:" << threadID << "trying to get read lock...\n";
    unique_readguard<RWLock> lk(rwLock);
    cout << "read thread:" << threadID << "get the read lock!" << " num: " << num << endl;
}

void write_num(int threadID, int sleepTime) {
    cout << "write thread:" << threadID << " trying to get lock..." << endl;
    unique_writeguard<RWLock> lk(rwLock);
    cout << "write thread:" << threadID << " get the lock!" << endl;
    num++;
}

int main() {

    num = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([=](int x, int y) {
            read_num(x, y);
            cout << "read thread:" << x << "release the read lock!\n";
            std::this_thread::sleep_for(std::chrono::seconds(x));
        }, i, i);
    }

    for (int i = 3; i < 6; ++i) {
        threads.emplace_back([=](int x, int y) {
            write_num(x, y);
            cout << "write thread:" << x << " release the lock!" << endl;
            std::this_thread::sleep_for(std::chrono::seconds(x));
        }, i, i);
    }

    for (auto &t: threads) {
        t.join();
    }
}