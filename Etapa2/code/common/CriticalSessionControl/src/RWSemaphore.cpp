
#include <RWSemaphore.hpp>

RWSemaphore::RWSemaphore() {

}

RWSemaphore::~RWSemaphore() {

}

void RWSemaphore::beginRead() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    _gMutex.lock();
        while (_writerActive) {
            _condition.wait(gLock);
        }
        _nReaders++;

    _gMutex.unlock();
}

void RWSemaphore::endRead() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    _gMutex.lock();
        _nReaders--;
        if (_nReaders == 0)
            _condition.notify_all();
    _rMutex.unlock();
}


void RWSemaphore::beginWrite() {
    std::unique_lock<std::mutex> gLock(_gMutex);

    _gMutex.lock();
        while (_nReaders > 0) {
            _condition.wait(gLock);
        }
        _writerActive = true;

    _gMutex.unlock();
}

void RWSemaphore::endWrite() {
    _gMutex.lock();
        _writerActive = false;
        _condition.notify_all();

    _gMutex.unlock();
}


