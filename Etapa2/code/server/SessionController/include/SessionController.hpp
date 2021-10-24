
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

    std::string username() const { return _username; }

    bool newSession(int csfd, unsigned short listenerPort);
    void closeSession(int csfd, bool sendClose = true, bool closeConnection = true);
    size_t getNumSessions();

    void addFollower(std::string follower);
    void sendMessage(std::string message);

    void deliverMessages(ReplicaManager&);

    std::map<int, unsigned short> getSessions() { return _sessionsSFD; }

    SessionController operator=(const SessionController&) = delete;

 private:
    std::string _username;
    PersistentUser _pUser;
    int _numSessions = 0;

    std::map<int, unsigned short> _sessionsSFD; // Socket file descriptors and listeners

    MessageManager& _mm;

    Semaphore _sem;

};
