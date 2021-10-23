
#include <RWSemaphore.hpp>

RWSemaphore::RWSemaphore() {

}

RWSemaphore::~RWSemaphore() {

}

void RWSemaphore::beginRead() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    while (_writerActive) {
        _condition.wait(gLock);
    }
    _nReaders++;

}

void RWSemaphore::endRead() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    _nReaders--;
    if (_nReaders == 0)
        _condition.notify_all();
}


void RWSemaphore::beginWrite() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    _writerActive = true;
    while (_nReaders > 0) {
        _condition.wait(gLock);
    }
}

void RWSemaphore::endWrite() {
    std::unique_lock<std::mutex> gLock(_gMutex);
    
    _writerActive = false;
    _condition.notify_all();

}


