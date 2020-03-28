//
// Created by Simon on 2020/3/28.
//
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

#ifdef _WIN32
struct MyData
{
    string name;
    int age;
    MyData(string _name, int _age) : name(std::move(_name)), age(_age)
    {}
};

void writeMemory()
{
    // define shared data
    char *shared_file_name = const_cast<char*>("my_shared_memory");
    unsigned long buff_size = 4096;
    char share_buffer[] = "greetings, hello world";
    //MyData share_buffer("Tom", 18);

    // create shared memory file
    HANDLE dump_file_descriptor = CreateFile(shared_file_name,
                                             GENERIC_READ | GENERIC_WRITE,
                                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                                             NULL,
                                             OPEN_ALWAYS, // open exist or create new, overwrite file
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);

    if (dump_file_descriptor == INVALID_HANDLE_VALUE)
        cout << "create file error" << endl;

    HANDLE shared_file_handler = CreateFileMapping(
            dump_file_descriptor, // Use paging file - shared memory
            NULL,                 // Default security attributes
            PAGE_READWRITE,       // Allow read and write access
            0,                    // High-order DWORD of file mapping max size
            buff_size,            // Low-order DWORD of file mapping max size
            shared_file_name);    // Name of the file mapping object

    if (shared_file_handler)
    {
        // map memory file view, get pointer to the shared memory
        LPVOID lp_base = MapViewOfFile(
                shared_file_handler,  // Handle of the map object
                FILE_MAP_ALL_ACCESS,  // Read and write access
                0,                    // High-order DWORD of the file offset
                0,                    // Low-order DWORD of the file offset
                buff_size);           // The number of bytes to map to view



        // copy data to shared memory
        memcpy(lp_base, &share_buffer, sizeof(share_buffer));

        FlushViewOfFile(lp_base, buff_size); // can choose save to file or not

        // process wait here for other task to read data
        cout << "already write to shared memory, wait ..." << endl;
        //cout << share_buffer << endl;
        this_thread::sleep_for(chrono::seconds(10));

        // close shared memory file
        UnmapViewOfFile(lp_base);
        CloseHandle(shared_file_handler);
        CloseHandle(dump_file_descriptor);
        //unlink(shared_file_name);
        cout << "shared memory closed" << endl;
    }
    else
        cout << "create mapping file error" << endl;
}
#elif __linux

struct MyData
{
    char name[20];
    int age;
};

void writeMemory()
{
    // specify shared file path
    char *shared_file_name = "/home/user/codetest/my_shared_memory";

    // define shared data
    //    unsigned long buff_size = 4096;
    //    char share_buffer[] = "greetings, hello world";
    //    MyData share_buffer("Tom", 18);
    MyData share_buffer = { "Tom", 18 };

    // create mmap file
    int fd = open(shared_file_name, O_CREAT | O_RDWR | O_TRUNC, 00777);
    if (fd < 0)
        cout << "create file error" << endl;

    size_t write_size = sizeof(share_buffer);

    ftruncate(fd, write_size); // extend file size

    // map memory to file
    void *p = mmap(NULL, write_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // copy data to shared memory
    memcpy(p, &share_buffer, write_size);

    cout << "already write to shared memory, wait ..." << endl;
    //cout << share_buffer << endl;
    this_thread::sleep_for(chrono::seconds(10));

    // unmap and close
    munmap(p, write_size);
    close(fd);

}
#endif

int main()
{
    writeMemory();

    return 0;
}
