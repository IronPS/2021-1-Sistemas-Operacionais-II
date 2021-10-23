#include <Semaphore.hpp>

Semaphore::Semaphore(int maxCount, int startCount) {
    _maxCount = maxCount;

    if (startCount == INT_MIN) {
        _count = maxCount;
    } else {
        _count = startCount;
    }

}

Semaphore::~Semaphore() {

}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(_mutex);

    while(_count == 0) _condition.wait(lock);

    _count--;
   
}

void Semaphore::notify() {
    _count++;
    if (_count > _maxCount) _count = _maxCount;

    _condition.notify_one();

}
