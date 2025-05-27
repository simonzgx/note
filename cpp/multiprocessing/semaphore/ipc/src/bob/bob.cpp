#include "../common/common.h"
#include "../common/shm_obj.h"
#include "../common/thread_pool.h"

#include <cassert>
#include <memory>
#include <functional>

void send(const Message *message) {
    static int fifo = 0;
    if (fifo == 0) {
        const char *filename = "bob_to_alice";
        if (access(filename, F_OK))
            mkfifo(filename, 0666);
        fifo = open(filename, O_WRONLY);
        assert(fifo != 0);
    }
    assert(write(fifo, message, message->size) == message->size);
}

const Message *recv() {
    static int fifo = 0;
    if (fifo == 0) {
        const char *filename = "alice_to_bob";
        if (access(filename, F_OK))
            mkfifo(filename, 0666);
        fifo = open(filename, O_RDONLY);
        assert(fifo != 0);
    }
    static Message *m = (Message *)malloc(MESSAGE_SIZES[4]);
    assert(read(fifo, m, sizeof(Message)) == sizeof(Message));
    assert(read(fifo, m->payload, m->payload_size()) == m->payload_size());
    return m;
}

int main() {
    // shm with sema
    // Message *m2 = (Message *)malloc(MESSAGE_SIZES[4]);
    // auto shm =
    //     std::make_shared<IPC::ShmObj<Message>>("test.txt", MESSAGE_SIZES[4]);
    // auto process = [=](const Message *msg) -> Message * {
    //   assert(msg->checksum == crc32(msg));
    //   memcpy(m2, msg, msg->size); // 拷贝m1至m2
    //   m2->payload[0]++;           // 第一个字符加一
    //   m2->checksum = crc32(m2);   // 更新校验和
    //   return m2;
    // };
    // while (true) {
    //   shm->receive_process_send(process);
    // }

    // shm with ring buffer

    ThreadPool pool(std::thread::hardware_concurrency() / 2);
    auto shm =
        std::make_shared<IPC::ShmWithRingBuffer<Message>>("test.txt", MESSAGE_SIZES[4] * 300);
    auto process = [](Message &msg) -> void {
        msg.payload[0]++; // 第一个字符加一
        msg.checksum = crc32(&msg);
    };
    while (true) {
        auto &msg = shm->read();
        pool.enqueue(std::bind(process, msg));
        // shm->read_and_process();
    }
    return 0;
}