#include <Semaphore.hpp>

Semaphore::Semaphore(int count) {
    _count = count;
}

Semaphore::~Semaphore() {

}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(_mutex);

    while(_count == 0) _condition.wait(lock);

    _count--;
}

void Semaphore::notify() {
    std::unique_lock<std::mutex> lock(_mutex);

    _count++;

    _condition.notify_one();
}
