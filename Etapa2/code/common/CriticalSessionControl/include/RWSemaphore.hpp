
#pragma once

#include <Semaphore.hpp>

class RWSemaphore {
 public:
    RWSemaphore();
    ~RWSemaphore();

    void beginRead();
    void endRead();
    void beginWrite();
    void endWrite();

 private:
    std::mutex _gMutex;
    std::condition_variable _condition;

    int _nReaders = 0;
    bool _writerActive = false;
};
