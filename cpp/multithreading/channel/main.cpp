//
// Created by Administrator on 2020/9/27.
//
#include "channel.h"


int main() {

    BufferedChannel<size_t> input;
    BufferedChannel<size_t> output;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 2; ++i) {
        producers.emplace_back([&]{
            for (size_t i = 0; i < 1000; ++i) {
                input.send(1);
            }
        });

        consumers.emplace_back([&]{
            size_t total = 0;
            size_t next = 0;

            while (input.recv(next)) {
                total += next;
            }

            output.send(total);
        });
    }

    for (auto&& t : producers) {
        t.join();
    }

    input.drain();

    for (auto&& t : consumers) {
        t.join();
    }

    output.drain();

    size_t total = 0;
    size_t next = 0;

    while (output.recv(next)) {
        total += next;
    }

    std::cout << "total: " << total << std::endl;

    return 0;
}