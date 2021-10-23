#include <Flag.hpp>

Flag::Flag(bool flag) {
    _flag = flag;
}

Flag::~Flag() {

}

void Flag::wait() {
    std::unique_lock<std::mutex> lock(_mutex);

    while (_flag) {
        _condition.wait(lock);
    }

}

void Flag::set() {
    std::unique_lock<std::mutex> lock(_mutex);

    _flag = true;
}

void Flag::unset() {
    std::unique_lock<std::mutex> lock(_mutex);

    _flag = false;

    _condition.notify_all();

}
