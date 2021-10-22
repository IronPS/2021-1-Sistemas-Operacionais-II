
#pragma once

#include <vector>
#include <algorithm>

#include <cxxopts/cxxopts.hpp>
#include <PacketBuilder.hpp>

#include <assert.h>

class ReplicationManager {
 public:
    typedef enum {
        SEND,       // Used by the leader to send a message for replication
        SENT,       // Used by the leader to wait confirmations
        CONFIRM,    // Used by the passive replicas to confirm replication
        COMMIT,     // Used by all replicas to indicate state propagation
        COMMITTED,   // Used by all replicas
        CANCELED
    } ReplicationState;

    typedef struct s_replication_data {
        PacketData::packet_t packet;
        uint64_t receivedAt;
        ReplicationState state;
        std::vector<bool> sentTo;
        unsigned short numFailures;
    } ReplicationData;
 
 public:
    ReplicationManager(const cxxopts::ParseResult& input);
    ~ReplicationManager();

    void setDead(unsigned short id) { _dead[id] = true; }
    void setAlive(unsigned short id) { _dead[id] = false; }

 public:
    bool newReplication(PacketData::packet_t, uint64_t& id);
    void receivedConfirm(unsigned short otherID, uint64_t packetID);
    bool getNextToSend(ReplicationData& res, bool firstIt);
    void updateSend(ReplicationData& data, unsigned short sentTo);

 public:
    void receivedToReplicate(PacketData::packet_t);
    bool getNextToConfirm(ReplicationData& res, bool firstIt);

 public:
    bool getNextToCommit(ReplicationData& res, bool firstIt);
    void checkTimeouts();

 private:
    unsigned short _id = 0;
    std::vector<unsigned short> _ids;
    std::vector<bool> _dead;

 private:
    std::map<uint64_t, struct s_replication_data> _messages;
    unsigned short _maxFailures = 3;

};