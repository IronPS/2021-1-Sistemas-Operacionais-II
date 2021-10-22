
#pragma once

#include <mutex>
#include <condition_variable>

#include <limits.h>

class Semaphore {
 public:
    Semaphore (int maxCount, int start_count = INT_MIN);
    ~Semaphore();

    void wait();
    void notify();

 protected:
    std::mutex _mutex;
    std::condition_variable _condition;
    int _maxCount;
    int _count = 0;
};
