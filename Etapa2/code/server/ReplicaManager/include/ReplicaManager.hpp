
#pragma once

#include <fstream>

#include <ReplicaConnection.hpp>
#include <PacketBuilder.hpp>
#include <ElectionManager.hpp>
#include <ReplicationManager.hpp>
#include <Flag.hpp>
#include <RWSemaphore.hpp>

#include <PersistenceManager.hpp>
#include <SessionMonitor.hpp>

#include <assert.h>

class ReplicaManager {
 public:
    ReplicaManager(const cxxopts::ParseResult& input, PersistenceManager&, SessionMonitor&);
    ~ReplicaManager();

    void start();

 public:
    bool isLeader() { return _em.isLeader(); }
    bool waitingElection() { return _em.waitingElection(); }
    PacketData::packet_t getLeaderInfo();

    static ServerData::server_info_t getNextServerInfo();

    bool waitCommit(const PacketData::packet_t commandPacket);
 
 protected:
    void commit(PacketData::packet_t commandPacket);

 protected:
    unsigned short _id;
    std::vector<unsigned short> _ids;
    std::vector<std::string> _addresses;
    std::vector<unsigned short> _cliPorts;

    std::vector<std::shared_ptr<ReplicaConnection>> _connections;

    static unsigned short _sinfoPt;

    ElectionManager _em;
    ReplicationManager _rm;
    RWSemaphore _rmSem;

 protected:
    PersistenceManager& _pm;
    SessionMonitor& _sm;
};