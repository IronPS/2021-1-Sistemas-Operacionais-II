
#pragma once

#include <thread>
#include <chrono>

#include <ElectionManager.hpp>
#include <ReplicationManager.hpp>
#include <ServerConnectionManager.hpp>
#include <ClientConnectionManager.hpp>
#include <Stoppable.hpp>
#include <assert.h>

class ReplicaConnection {
 public:
    ReplicaConnection(ElectionManager& em, ReplicationManager& rm,
                      unsigned short thisID, std::string thisAddr, unsigned int thisPort, 
                      unsigned short otherID, std::string otherAddr, unsigned int otherPort);
    ~ReplicaConnection();
    
    bool connected() const { return _connected; }
    unsigned short thisID() const { return _thisID; }
    unsigned short otherID() const { return _otherID; }
 public:
    void loop();
    
 public:
    void electionState(ElectionManager::Action);

 public:
    bool sendReplication(PacketData::packet_t packet);

 private:
    unsigned short _thisID;
    unsigned short _otherID;

    int _sfd = -1;

    std::string _thisAddr;
    unsigned short _thisPort;
    std::string _otherAddr;
    unsigned short _otherPort;

    bool _isServer;
    ClientConnectionManager* _cm = nullptr;
    ServerConnectionManager* _sm = nullptr;

    bool _connected = false;
    bool _first_connection = true;
    void _connect();
    void _lostConnection();

 private:
    void _receivePacket(bool ignoreTimeout = false);

 private:
    unsigned int _timeout = 5;
    time_t _lastReceivedHeartbeat;

    void _sendHeartbeat();
    unsigned int _heartbeatInterval = 1; // 1 second
    time_t _lastSentHeartbeat;

 private:
    ElectionManager& _em;
    ReplicationManager& _rm;

 private:
    bool _attemptingConnectionMessage = true;

};
