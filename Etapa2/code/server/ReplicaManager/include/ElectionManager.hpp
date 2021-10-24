
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
    bool isLeader() { if (_locked) return false; return _isLeader; }
    unsigned short getLeaderID() { if (_locked) return 0; return _leaderID; }
    bool unlockedIsLeader() { return _isLeader; }
    unsigned short unlockedGetLeaderID() { return _leaderID; }
    Action action();
    uint16_t epoch() { return _epoch; }

    void step();
    void block() { _locked = true; }
    void unblock() { _locked = false; }

 public:
    bool leaderIsAlive() { if (_locked) return false; return _leaderIsAlive; }
    bool unlockedLeaderIsAlive() { return _leaderIsAlive; }
    void unsetLeaderIsAlive();

 public:
    bool waitingElection() { if (_locked) return true; return !_leaderIsAlive; }
    void startElection();
    void receivedElection();
    void receivedAnswer(uint16_t epoch);
    void receivedCoordinator(unsigned short id, uint16_t epoch);

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
    uint16_t _epoch = 0;

    unsigned int _waitAnswerTimeout = 3;
    unsigned int _waitElectionTimeout = 3;
    time_t _waitAnswerTimer;
    time_t _waitElectionTimer;

    bool _printedWaitingAnswers = false;
    bool _printedWaitingElection = false;

    void _startElection();

 private:
    bool _locked = false;

};
