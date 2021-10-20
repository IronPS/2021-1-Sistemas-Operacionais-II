
#pragma once

#include <cxxopts/cxxopts.hpp>
#include <ServerConnectionManager.hpp>
#include <Semaphore.hpp>

class ElectionManager {
 public:
    enum State {LEADER, REPLICA};
    enum Action {SendElection, SendCoordinator, WaitAnswer, WaitElection, None};

 public:
    ElectionManager(const cxxopts::ParseResult& input);
    ~ElectionManager();

 public:
    bool isLeader() { _sem.wait(); _sem.notify(); return _isLeader; }
    unsigned short getLeaderID() { _sem.wait(); _sem.notify(); return _leaderID; }
    bool asyncIsLeader() { return _isLeader; }
    unsigned short asyncGetLeaderID() { return _leaderID; }
    Action action();

    void step();
    void block() { _sem.wait(); }
    void unblock() {_sem.notify(); }

 public:
    bool leaderIsAlive() { _sem.wait(); _sem.notify(); return _leaderIsAlive; }
    bool asyncLeaderIsAlive() { return _leaderIsAlive; }
    void unsetLeaderIsAlive();

 public:
    bool waitingElection() { _sem.wait(); _sem.notify(); return !_leaderIsAlive; }
    void startElection();
    void receivedElection();
    void receivedAnswer();
    void receivedCoordinator(unsigned short id);

 private:
    unsigned short _id;
    std::vector<unsigned short> _ids;

 private:
    unsigned short _leaderID = 0;
    bool _isLeader = false;
    State _state = State::REPLICA;

 private:
    bool _leaderIsAlive = false;
    Action _action = Action::None;

    unsigned int _waitAnswerTimeout = 3;
    unsigned int _waitElectionTimeout = 3;
    time_t _waitAnswerTimer;
    time_t _waitElectionTimer;

    void _startElection();

 private:
    Semaphore _sem;

};
