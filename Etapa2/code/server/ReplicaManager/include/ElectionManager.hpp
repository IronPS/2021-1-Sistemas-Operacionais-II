
#pragma once

#include <cxxopts/cxxopts.hpp>
#include <ServerConnectionManager.hpp>

class ElectionManager {
 public:
    enum State {LEADER, REPLICA, ELECTION, ELECTIONFAILED};

 public:
    ElectionManager(const cxxopts::ParseResult& input);
    ~ElectionManager();

 public:
    bool isLeader() const { return _isLeader; }
    unsigned short getLeaderID() const { return _leaderID; }
    State state() const { return _state; }
    
 public:
    bool leaderIsAlive() const { return _leaderIsAlive; }
    void unsetLeaderIsAlive() { _leaderIsAlive = false; }

 public:
    bool waitingElection() const { return _state == ELECTION; }
    void startElection();
    void receivedWait();
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

};
