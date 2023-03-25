
#ifndef SHM_OBJ_H
#define SHM_OBJ_H

#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <utility>
#include <atomic>

#include "common.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/sem.h>
#include <unistd.h>
#endif

namespace IPC {

static sembuf sem_p1 = {0, -1, SEM_UNDO}, sem_v1 = {0, 1, SEM_UNDO},
              sem_v2 = {1, 1, SEM_UNDO}, sem_p2 = {1, -1, SEM_UNDO};

class Sem {
public:
    Sem(key_t key) {
        sem_id_ = semget(key, 2, IPC_CREAT | 0600);
        if (sem_id_ < 0) {
            fprintf(stderr, "Failed to create semaphore\n");
            exit(-1);
        }
        union semun sem_union;

        sem_union.val = 1;
        union semun sem_args;
        unsigned short array[2] = {0, 0}; //构建数组

        sem_args.array = array; //构建出给两个信号量一起赋值的共用体参数
        int ret = semctl(sem_id_, 0, SETALL, sem_args);
        if (ret < 0) {
            fprintf(stderr, "Failed to init semaphore\n");
            exit(-1);
        }
    }

    ~Sem() {
        union semun sem_union;
        if (semctl(sem_id_, 0, IPC_RMID, sem_union) == -1) {
            fprintf(stderr, "Failed to delete semaphore\n");
        }
    }

    void p1() {
        if (semop(sem_id_, &sem_p1, 1) < 0) {
            fprintf(stderr, "semaphore_p1 failed\n");
            exit(-1);
        }
    }

    void v1() {
        if (semop(sem_id_, &sem_v1, 1) < 0) {
            fprintf(stderr, "semaphore_v1 failed\n");
        }
    }

    void p2() {
        if (semop(sem_id_, &sem_p2, 1) < 0) {
            fprintf(stderr, "semaphore_p2 failed\n");
            exit(-1);
        }
    }

    void v2() {
        if (semop(sem_id_, &sem_v2, 1) < 0) {
            fprintf(stderr, "semaphore_v2 failed\n");
            exit(-1);
        }
    }

    int get_sem_id() {
        return sem_id_;
    }

private:
    int sem_id_;
};

class ShmObj {
public:
    ShmObj(key_t key, size_t size) {
        shmid_ = shmget(key, size, IPC_CREAT | 0660);
        if (shmid_ < 0) {
            perror("shmget");
            exit(-1);
        }
        shm_addr_ = (char *)shmat(shmid_, NULL, 0);
        memset(shm_addr_, 0, size);
    }

    ~ShmObj() {
    }

    char *get_address() {
        return shm_addr_;
    }

private:
    int shmid_;
    char *shm_addr_;
};

template <typename Msg>
class ShmWithSem {
public:
    ShmWithSem(const std::string &name, size_t size) {
        key_t key = ftok(name.c_str(), 888);
        if (key < 0) {
            perror("ftok");
            exit(-1);
        }
        shm_obj_.reset(new ShmObj(key, size));
        sem_.reset(new Sem(key));
    }

    void start_produce() {
        sem_->v1();
    }

    void send_receive_process(const Msg *msg,
                              std::function<void(const Msg *)> func) {
        sem_->p1();
        memcpy(shm_addr_, msg, msg->size);
        sem_->v2();
        sem_->p1();
        // 这时上个消息已经ready了
        func(reinterpret_cast<const Msg *>(shm_addr_));
        sem_->v1();
    }

    void receive_process_send(std::function<Msg *(const Msg *)> func) {
        sem_->p2();
        Msg *msg = reinterpret_cast<Msg *>(shm_addr_);
        Msg *new_msg = func(msg);
        memcpy(shm_addr_, new_msg, new_msg->size);
        sem_->v1();
    }

private:
    int create_new_fd(const std::string &name, mode_t mode) {
        int ret = 0;
        return ret;
    }

private:
    std::unique_ptr<Sem> sem_;
    std::unique_ptr<ShmObj> shm_obj_;
    char *shm_addr_;
};

template <typename Tp>
struct RingBuffer {
    RingBuffer() :
        write_index(0), read_index(0), check_index(0) {
    }

    void write(const Tp *msg) {
        memcpy(payload + this->write_index, msg, msg->size);
        this->write_index += msg->size;
    }

    Tp *peek() {
        while (this->read_index >= this->write_index)
            ;
        Tp *msg = reinterpret_cast<Tp *>(payload + this->read_index);
        return msg;
    }

    void pop(size_t size) {
        this->read_index += size;
    }

    Tp *read_one() {
        while (this->read_index >= this->write_index)
            ;
        Tp *msg = reinterpret_cast<Tp *>(payload + this->read_index);
        this->read_index += msg->size;
        return msg;
    }

    Tp *check() {
        // std::cout << "check:" << check_index << "read" << read_index << std::endl;
        while (check_index >= read_index)
            ;
        Tp *msg = reinterpret_cast<Tp *>(payload + this->check_index);
        this->check_index += msg->size;
        return msg;
    }

    volatile size_t write_index, read_index, check_index;
    char payload[0];
};

template <typename Msg>
class ShmWithRingBuffer {
public:
    ShmWithRingBuffer(const std::string &name, size_t size) :
        stop_(false) {
        key_t key = ftok(name.c_str(), 888);
        if (key < 0) {
            perror("ftok");
            exit(-1);
        }
        shm_obj_.reset(new ShmObj(key, size));
        ring_buffer_ = reinterpret_cast<RingBuffer<Msg> *>(shm_obj_->get_address());
    }

    void send(const Msg *msg) {
        ring_buffer_->write(msg);
    }

    void send_and_process(const Msg *msg, std::function<void(const Msg &)> func) {
        ring_buffer_->write(msg);
        Msg &new_msg = ring_buffer_->check();
        func(new_msg);
    }

    void read_and_process() {
        Msg *msg = ring_buffer_->peek();
        assert(msg->checksum == crc32(msg));
        msg->payload[0]++; // 第一个字符加一
        msg->checksum = crc32(msg);
        ring_buffer_->pop(msg->size);
    }

    Msg &read() {
        return *ring_buffer_->read_one();
    }

    Msg &check() {
        return *ring_buffer_->check();
    }

    void stop() {
        stop_.store(true);
    }

    bool is_stop() {
        return stop_ == true;
    }

private:
    std::unique_ptr<ShmObj> shm_obj_;
    RingBuffer<Msg> *ring_buffer_;
    std::atomic<bool> stop_;
};

} // namespace IPC

#endif // SHM_OBJ_H