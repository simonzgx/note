//
// Created by simon on 2020/4/30.
//

#ifndef TEST_SEMAPHORE_H
#define TEST_SEMAPHORE_H

#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mutex>
#include <cstdlib>
#include <atomic>
#include <cstdio>
#include <map>
#include <sys/sem.h>
#include <sys/mman.h>
#include <cassert>

using std::cout;
using std::endl;

namespace util {


    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *arry;
    };

    static int sem_id = 0;

    static int set_semvalue();

    static void del_semvalue();

    static int semaphore_p();

    static int semaphore_v();

    int main(int argc, char *argv[]) {
        int fd;
        struct stat fs{};
        char *mapped_mem, buf[1024];

        if ((fd = open(argv[1], O_RDWR)) < 0) {
            perror("open");
        }
        if (fstat(fd, &fs) == -1) {
            perror("fstat error");
        }

        if ((mapped_mem = static_cast<char *>(mmap(nullptr, fs.st_size,
                                                   PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0))) == (void *) -1) {
            perror("mmap");
        }

        close(fd);

        int i = 0;

        //创建信号量
        sem_id = semget((key_t) 1234, 1, 0666 | IPC_CREAT);

        if (argc > 1) {
            //程序第一次被调用，初始化信号量
            if (!set_semvalue()) {
                fprintf(stderr, "Failed to initialize semaphore\n");
                exit(EXIT_FAILURE);
            }
        }
        while (true) {
            cout << "producer wait" << endl;
            //进入临界区
            if (!semaphore_p())
                exit(EXIT_FAILURE);

            cout << "producing..." << endl;
            sleep(rand() % 5);
            if (!semaphore_v())
                exit(EXIT_FAILURE);
            cout << "produce done\n";
            sleep(rand() % 1);
        }

        sleep(10);
        printf("\n%d - finished\n", getpid());

        if (argc > 1) {
            //如果程序是第一次被调用，则在退出前删除信号量
            sleep(3);
            del_semvalue();
        }
        exit(EXIT_SUCCESS);
    }

    static int set_semvalue() {
        //用于初始化信号量，在使用信号量前必须这样做
        union semun sem_union;

        sem_union.val = 1;
        if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
            return 0;
        return 1;
    }

    static void del_semvalue() {
        //删除信号量
        union semun sem_union;

        if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
            fprintf(stderr, "Failed to delete semaphore\n");
    }

    static int semaphore_p() {
        //对信号量做减1操作，即等待P（sv）
        struct sembuf sem_b;
        sem_b.sem_num = 0;
        sem_b.sem_op = -1;//P()
        sem_b.sem_flg = SEM_UNDO;
        if (semop(sem_id, &sem_b, 1) == -1) {
            fprintf(stderr, "semaphore_p failed\n");
            return 0;
        }
        return 1;
    }

    static int semaphore_v() {
        //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
        struct sembuf sem_b;
        sem_b.sem_num = 0;
        sem_b.sem_op = 1;//V()
        sem_b.sem_flg = SEM_UNDO;
        if (semop(sem_id, &sem_b, 1) == -1) {
            fprintf(stderr, "semaphore_v failed\n");
            return 0;
        }
        return 1;
    }

    template<typename Name>
    class OpenError : public std::exception {

    };

    class Semaphore;

    using SemaphorePTr = std::shared_ptr<Semaphore>;

    class Semaphore {
    public:

        Semaphore&operator=(const Semaphore&) = delete;

        static std::atomic<Semaphore *> instance;

        static Semaphore& getInstance(const char *file) {
            static Semaphore instance(file);
            return instance;
        }

        bool p() {
            //对信号量做减1操作，即等待P（sv）
            struct sembuf sem_b{};
            sem_b.sem_num = 0;
            sem_b.sem_op = -1;//P()
            sem_b.sem_flg = SEM_UNDO;
            if (semop(_sem_id, &sem_b, 1) == -1) {
                fprintf(stderr, "semaphore_p failed\n");
                return false;
            }
            return true;
        }

        bool v() {
            //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
            struct sembuf sem_b{};
            sem_b.sem_num = 0;
            sem_b.sem_op = 1;//V()
            sem_b.sem_flg = SEM_UNDO;
            if (semop(_sem_id, &sem_b, 1) == -1) {
                fprintf(stderr, "semaphore_v failed\n");
                return false;
            }
            return true;
        }

    private:

        explicit Semaphore(const char *file) {
            struct stat fs{};
            char *mapped_mem, buf[1024];

            if ((_fd = open(file, O_RDWR)) < 0) {
                throw std::exception();
            }
            if (fstat(_fd, &fs) == -1) {
                perror("fstat error");
            }
        }


    private:
        int _fd;
        int _sem_id{};
        std::map<key_t, int> _key_map;
        static std::mutex _lock;
    };

}

#endif //TEST_SEMAPHORE_H
