
#pragma once

#include <map>

#include <PersistenceManager.hpp>
#include <PersistentUser.hpp>
#include <Semaphore.hpp>
#include <ServerConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <MessageManager.hpp>

class ReplicaManager;

class SessionController {
 public:
    SessionController(std::string username, PersistenceManager& pm, MessageManager& mm);
    ~SessionController();

    bool isValid() const { return _success; }

    bool newSession(int csfd, unsigned short listenerPort);
    void closeSession(int csfd, bool sendClose = true, bool closeConnection = true);
    size_t getNumSessions();

    void addFollower(std::string follower);
    void sendMessage(std::string message);

    void deliverMessages(ReplicaManager&);

    SessionController operator=(const SessionController&) = delete;

 private:
    std::string _username;
    PersistentUser _pUser;
    int _numSessions = 0;

    std::map<int, unsigned short> _sessionSFD; // Socket file descriptors and listeners

    MessageManager& _mm;

    bool _success = false;

    Semaphore _sem;


};
