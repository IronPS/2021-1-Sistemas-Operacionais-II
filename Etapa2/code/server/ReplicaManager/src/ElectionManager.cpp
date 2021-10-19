
#include <ElectionManager.hpp>

ElectionManager::ElectionManager(const cxxopts::ParseResult& input) 
: _id(input["id"].as<unsigned short>())
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    
}

ElectionManager::~ElectionManager() {

}

void ElectionManager::startElection() {
    _state = State::ELECTION;
}

void ElectionManager::receivedWait() {
    if (_state == State::ELECTION) {
        // TODO
    }

}

void ElectionManager::receivedCoordinator(unsigned short id) {
    if (_state == State::ELECTION) {
        _leaderID = id;
        if (id == _id) {
            _isLeader = true;

            _state = State::LEADER;
        } else {
            _state = State::REPLICA;
        }

        _leaderIsAlive = true;

    }
}
