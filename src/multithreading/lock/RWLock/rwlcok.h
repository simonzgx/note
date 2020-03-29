//
// Created by simon on 2020/3/29.
//

#include <iostream>
#include <mutex>
#include <condition_variable>

class RWLock{
public:
    RWLock() = default;
    ~RWLock() = default;

public:
    void RLock(){
        std::unique_lock<std::mutex> lk(_lk);
        r_cond.wait(lk, [=]()->bool{return write_count==0;});
        ++read_count;
    }

    void Lock(){
        std::unique_lock<std::mutex> lk(_lk);
        ++write_count;
        w_cond.wait(lk, [=]()->bool {return read_count==0 && !isWriting;});
        isWriting = true;
    }

    void RUnlock(){
        std::unique_lock<std::mutex> lk(_lk);
        if(--read_count==0 && write_count>0){
            w_cond.notify_one();
        }
    }

    void Unlock(){
        std::unique_lock<std::mutex> lk(_lk);
        if(--write_count==0){
            r_cond.notify_all();
        }else{
            r_cond.notify_one();
        }
        isWriting = false;
    }

private:
    volatile size_t read_count{0};
    volatile size_t write_count{0};
    volatile bool isWriting{false};
    std::mutex _lk;
    std::condition_variable w_cond;
    std::condition_variable r_cond;
};

template<typename _RWLockable>
class unique_readguard{
public:
    explicit unique_readguard(_RWLockable& rwlock):_lk(rwlock){
        _lk.RLock();
    }
    ~unique_readguard(){
        _lk.RUnlock();
    }
    unique_readguard(const unique_readguard& )=delete;
    unique_readguard& operator=(const unique_readguard &)=delete;

private:
    _RWLockable &_lk;
};

template<typename _RWLockable>
class unique_writeguard{
public:
    explicit unique_writeguard(_RWLockable& rwlock):_lk(rwlock){
        _lk.Lock();
    }
    ~unique_writeguard(){
        _lk.Unlock();
    }
    unique_writeguard(const unique_writeguard& )=delete;
    unique_writeguard& operator=(const unique_writeguard &)=delete;

private:
    _RWLockable &_lk;
};