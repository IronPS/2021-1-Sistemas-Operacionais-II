
#pragma once

#include <mutex>
#include <condition_variable>

#include <Semaphore.hpp>

#include <limits.h>

class Flag {
 public:
    Flag (bool flag);
    ~Flag();

    void wait();
    void set();
    void unset();

 protected:
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _flag = false;

};
