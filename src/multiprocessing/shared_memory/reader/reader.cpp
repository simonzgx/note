//
// Created by simon on 2020/3/29.
//

#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <cstring>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#endif

using namespace std;

#ifdef _WIN32
struct MyData
{
    string name;
    int age;
    MyData(string _name, int _age) : name(_name), age(_age)
    {}
};

void readMemory()
{
    char *shared_file_name = "my_shared_memory";

    // open shared memory file
    HANDLE shared_file_handler = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        NULL,
        shared_file_name);

    if (shared_file_handler)
    {
        LPVOID lp_base = MapViewOfFile(
            shared_file_handler,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0);

        // copy shared data from memory
        cout << "read shared data: " << endl;
        const unsigned long buff_size = 4096;
        //char share_buffer[buff_size] = { 0 };
        //strcpy(share_buffer, (char *)lp_base);
        char *share_buffer = (char *)lp_base;

        cout << share_buffer << endl;

        /*MyData *my_data = (MyData *)lp_base;
        cout << my_data->name << " " << my_data->age << endl;*/

        // close share memory file
        UnmapViewOfFile(lp_base);
        CloseHandle(shared_file_handler);
    }
    else
        cout << "open mapping file error" << endl;
}
#elif __linux
struct MyData
{
    char name[20];
    int age;
};

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static int sem_id = 0;
static inline int set_sem();
static inline void del_sem();
static inline int sem_p();
static inline int sem_v();

void readMemory()
{
    sem_id = semget((key_t)333, 1, 0666);
    // specify shared file path
    char *shared_file_name = const_cast<char*>("/home/simon/my_shared_memory.txt");

    // open mmap file
    int fd = open(shared_file_name, O_RDONLY, 00777);
    if (fd < 0)
        cout << "open file error" << endl;

    char share_buffer[BUFSIZ+1];
    size_t read_size = sizeof(share_buffer);
    memset(&share_buffer, 0, read_size);
    // map file to memory
    void *p = mmap(NULL, read_size, PROT_READ, MAP_SHARED, fd, 0);

    while(true){
        cout<<"wait reading\n";
        if(!sem_p()){
            perror("sem_p error");
            exit(1);
        }
        memcpy(share_buffer, p, read_size);
        cout << "read shared data: " << share_buffer;
        if (!sem_v()){
            perror("sem_v error");
            exit(1);
        }
        cout<<"read done\n";
        if(strncmp(share_buffer, "end",3)==0){
            printf("exit");
            break;
        }
    }

//    auto *share_buffer = (MyData *)p;
//    cout << share_buffer->name << " " << share_buffer->age << endl;

    // unmap and close
    munmap(p, read_size);
    close(fd);
}
#endif

int main()
{
    readMemory();

    return 0;
}


static inline int set_sem(){
    union semun sem_union{};
    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}

static inline void del_sem(){
    union semun sem_union{};
    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        perror("del sem id error");
}

static inline int sem_p(){
    struct sembuf sem_buff{};
    sem_buff.sem_num = 0;
    sem_buff.sem_op = -1;
    sem_buff.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_buff, 1) == -1){
        perror("set op error");
        return 0;
    }
    return 1;
}

static inline int sem_v(){
    struct sembuf sem_buff{};
    sem_buff.sem_flg = SEM_UNDO;
    sem_buff.sem_op = 1;
    sem_buff.sem_num = 0;
    if(semop(sem_id, &sem_buff, 1)==-1){
        perror("sem_v error");
        return 0;
    }
    return 1;
}