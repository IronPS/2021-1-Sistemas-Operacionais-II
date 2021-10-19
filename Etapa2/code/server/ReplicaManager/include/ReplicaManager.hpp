
#pragma once

#include <fstream>

#include <ReplicaConnection.hpp>
#include <PacketBuilder.hpp>
#include <ElectionManager.hpp>

#include <assert.h>

class ReplicaManager {
 public:
    typedef struct s_serverinfo {
        std::string address;
        std::string port;
    } server_info_t;

 public:
    ReplicaManager(const cxxopts::ParseResult& input);
    ~ReplicaManager();

    void start();

    bool isLeader() { /*TODO*/ return _id == 0; }
    bool waitingElection() { return true; }
    PacketData::packet_t getLeaderInfo();

    static server_info_t getNextServerInfo();


 private:
    unsigned short _id;
    std::vector<unsigned short> _ids;
    std::vector<std::string> _addresses;
    std::vector<unsigned short> _cliPorts;

    std::vector<std::shared_ptr<ReplicaConnection>> _connections;

    unsigned short _leaderID = 0;

    static unsigned short _sinfoPt;

    ElectionManager _em;

};