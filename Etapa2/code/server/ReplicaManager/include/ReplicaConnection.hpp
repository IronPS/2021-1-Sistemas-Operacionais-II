
#pragma once

#include <thread>
#include <chrono>

#include <ElectionManager.hpp>
#include <ServerConnectionManager.hpp>
#include <ClientConnectionManager.hpp>
#include <Stoppable.hpp>
#include <assert.h>

class ReplicaConnection {
 public:
    ReplicaConnection(ElectionManager& em,
                      unsigned short thisID, std::string thisAddr, unsigned int thisPort, 
                      unsigned short otherID, std::string otherAddr, unsigned int otherPort);
    ~ReplicaConnection();

    void loop();

    bool connected() const { return _connected; }

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
    void _connect();
    void _lostConnection();

 private:
    void _receivePacket();

 private:
    unsigned int _timeout = 20;
    time_t _lastReceivedHeartbeat;

    void _sendHeartbeat();
    unsigned int _heartbeatInterval = 1; // 1 second
    time_t _lastSentHeartbeat;

 private:
    ElectionManager& _em;

 private:
    bool _attemptingConnectionMessage = true;

};
