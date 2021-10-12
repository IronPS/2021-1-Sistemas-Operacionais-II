
#pragma once

#include <thread>
#include <chrono>
#include <random>

#include <cxxopts/cxxopts.hpp>
#include <ServerConnectionManager.hpp>
#include <ClientConnectionManager.hpp>
#include <Stoppable.hpp>
#include <Semaphore.hpp>
#include <PacketBuilder.hpp>

#include <assert.h>
#include <time.h>
#include <fcntl.h>

class ReplicaManager {
 public:
    ReplicaManager(const cxxopts::ParseResult& input);
    ~ReplicaManager();

    void handleReplicas();

 private:
    unsigned short _id;
    std::vector<unsigned short> _ids; 

 private:
    enum State {Replica, Leader, Election};
    State _state;
    unsigned short _leaderID = 0;

    Semaphore _hbSem;
    std::vector<time_t> _lastHeartbeat;
    unsigned int _timeout = 20;

    void _receiveHeartbeat(unsigned short id, int sfd);

 private:
    std::vector<std::string> _addresses;
    std::vector<unsigned short> _ports;
    
    std::vector<int> _socketFileDescriptors;
    Semaphore _sfdSem;

    ServerConnectionManager* _sm = nullptr;
    std::vector<ClientConnectionManager*> _cms;

    std::mt19937 _gen;
    std::uniform_int_distribution<> _intDistr;
    void _connectionHandler(unsigned short serverId);
    void _connectAsClient(unsigned short i);

    Semaphore _serverSem;
    void _connectAsServer(unsigned short i);

    bool _connectionCheck(unsigned short serverId);
    bool _connectionConfirm(unsigned short serverId, int sfd);
};
