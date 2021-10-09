
#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {
 public:
    Semaphore (int count = 0);
    ~Semaphore();

    void wait();
    void notify();

 private:
    std::mutex _mutex;
    std::condition_variable _condition;
    int _count = 0;
};
