#include <iostream>
#include <pthread.h>
#include <queue>
#include <strings.h>
#include <sstream>
#include <unistd.h>

using QueuePair = std::pair<unsigned int, unsigned char*>;
using CmdQueue = std::pair<volatile bool, QueuePair>;

class NonLockQueue
{
public:
    NonLockQueue():read(0), write(0), msg(NULL), msgLen(0)
    {}

    ~NonLockQueue()
    {

    }
    QueuePair *front()
    {
        if(cmdQueue[read].first)
            return &cmdQueue[read].second;
        return NULL;
    }

    void pop()
    {
        cmdQueue[read].first = false;
        read = (++read) % 1024;
    }

    bool push(const void *msg, const unsigned int len)
    {
        unsigned char* buf = new unsigned char[len];
        if(buf)
        {
            bcopy(msg, buf, len);
            if(!dump() && !cmdQueue[write].first)
            {
                cmdQueue[write].second.first = len;
                cmdQueue[write].second.second = buf;
                cmdQueue[write].first = true;
                write = (++write) % 1024;
                return true;
            }
            else
            {
                //如果队列满了，则写入缓冲
                cacheQueue.push(std::make_pair(len, buf));
            }
            return true;
        }
        return false;
    }

private:

    bool dump()
    {
        //缓冲中还有数据
        while(!cacheQueue.empty())
        {
            if(cmdQueue[write].first)
            {
                return true;
            }
            // 优先将缓冲中的数据写入到队列
            cmdQueue[write].second = cacheQueue.front();
            cmdQueue[write].first = true;
            write = ++write % 1024;
            cacheQueue.pop();
        }
        return false;
    }

    CmdQueue cmdQueue[1024];
    std::queue<QueuePair> cacheQueue;
    unsigned int read;
    unsigned int write;
    unsigned char *msg;
    unsigned int msgLen;
};

static void *funWrite(void *arg)
{
    NonLockQueue* pQue = (NonLockQueue*)arg;

    for(int i = 0; i < 1000; ++i)
    {
        std::stringstream ss;
        ss << i;
        pQue->push(ss.str().c_str(), ss.str().size() + 1);
    }
    return NULL;
}

static void* funRead(void *arg)
{
    NonLockQueue* pQue = (NonLockQueue*)arg;
    while(true)
    {
        QueuePair* pPair = pQue->front();
        if(!pPair)
        {
            continue;
        }

        std::cout << pPair->first << "----" << pPair->second << std::endl;
        pQue->pop();
    }
    return NULL;
}

int main()
{
    NonLockQueue que;
    pthread_t t1, t2;
    pthread_create(&t1, NULL, funWrite, &que);
    pthread_create(&t2, NULL, funRead, &que);

    while(true)
    {

    }
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}