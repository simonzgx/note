#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <utility>
#include <cstdlib>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <functional>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <cstring>
#include <string>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace IPC {

class RingBuffer {
};

static sembuf sem_p1 = {0, -1, SEM_UNDO},
              sem_v1 = {0, 1, SEM_UNDO},
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

template <typename Msg>
class ShmObj {
public:
    ShmObj(const std::string &name, size_t size) :
        file_name_(name) {
        key_t key = ftok(name.c_str(), 888);
        if (key < 0) {
            perror("ftok");
            exit(-1);
        }
        shmid_ = shmget(key, size, IPC_CREAT | 0660);
        if (shmid_ < 0) {
            perror("shmget");
            exit(-1);
        }
        sem_.reset(new Sem(key));
        shm_addr_ = (char *)shmat(shmid_, NULL, 0);
        memset(shm_addr_, 0, size);
    }

    void start_produce() {
        sem_->v1();
    }

    void send_receive_process(const Msg *msg, std::function<void(const Msg *)> func) {
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
    std::string file_name_;
    std::unique_ptr<Sem> sem_;
    int shmid_;
    char *shm_addr_;
};
} // namespace IPC