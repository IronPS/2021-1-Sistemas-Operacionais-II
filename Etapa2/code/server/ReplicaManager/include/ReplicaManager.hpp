
#pragma once

#include <thread>
#include <assert.h>

#include <cxxopts/cxxopts.hpp>
#include <ServerConnectionManager.hpp>
#include <ClientConnectionManager.hpp>
#include <Stoppable.hpp>

class ReplicaManager {
 public:
    ReplicaManager(const cxxopts::ParseResult& input);
    ~ReplicaManager();

    void handleReplicas();

 private:
    std::string _myPort;
    std::vector<std::string> _addresses;
    std::vector<unsigned short> _ports;
    std::map<unsigned short, int> _socketFileDescriptors;

    enum state {Replica, Leader, Election};

    unsigned short _leaderID = 0;

 private:
    void _connectionHandler(size_t i);
};
