
#include <ElectionManager.hpp>

ElectionManager::ElectionManager(const cxxopts::ParseResult& input) 
: _id(input["id"].as<unsigned short>())
{
    _ids = input["ids"].as<std::vector<unsigned short>>();

}

ElectionManager::~ElectionManager() {

}

void ElectionManager::startElection() {
    if (!_leaderIsAlive && _action == Action::None) 
        _startElection();
}

void ElectionManager::receivedElection() {
    if (_action == Action::None && !_leaderIsAlive) {
        _startElection();

    }
}

void ElectionManager::receivedAnswer(uint16_t epoch) {
    if (_leaderIsAlive) return;
    if (epoch < _epoch) return;

    _state = State::REPLICA;
    _action = Action::WaitElection;
    time(&_waitElectionTimer);

}

void ElectionManager::receivedCoordinator(unsigned short id, uint16_t epoch) {
    _epoch = epoch;

    if (_state == State::REPLICA) {
        _leaderID = id;
        if (id == _id) {
            _isLeader = true;

            _state = State::LEADER;
        } else {
            _state = State::REPLICA;
        }

        _leaderIsAlive = true;
        _action = Action::None;

        std::cout << "New coordinator: " << _leaderID << "\n";

    }
}

void ElectionManager::unsetLeaderIsAlive() { 
    _leaderIsAlive = false;
    _startElection();
}

void ElectionManager::step() {
    switch(_action) {
        case SendElection:
            _action = Action::WaitAnswer;
            if (!_printedWaitingAnswers) {
                std::cout << "Waiting answers\n";
                _printedWaitingAnswers = true;
            }
            break;

        case SendCoordinator:
            _action = Action::None;
            break;

        case WaitAnswer:
            break;

        case WaitElection:
            if (!_printedWaitingElection) {
                std::cout << "Waiting answers\n";
                std::cout << "Waiting election\n";
                _printedWaitingElection = true;
            }
            break;

        case None:
            break;
    }
}

ElectionManager::Action ElectionManager::action() { 
    time_t now;
    time(&now);

    if (_action == Action::WaitAnswer && difftime(now, _waitAnswerTimer) > _waitAnswerTimeout) {
        receivedCoordinator(_id, _epoch);
        _leaderIsAlive = true;
        _action = Action::SendCoordinator;

    } else if (_action == Action::WaitElection && difftime(now, _waitElectionTimer) > _waitElectionTimeout) {
        _startElection();
    }

    return _action; 
}

void ElectionManager::_startElection() {
    std::cout << "Starting election\n";
    _state = State::REPLICA;
    _isLeader = false;
    _action = Action::SendElection;
    _epoch += 1;

    _printedWaitingAnswers = false;
    _printedWaitingElection = false;

    time(&_waitAnswerTimer);
}
