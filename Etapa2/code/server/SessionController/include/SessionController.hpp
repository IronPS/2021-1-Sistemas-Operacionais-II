
#pragma once

#include <set>

#include <PersistenceManager.hpp>
#include <PersistentUser.hpp>
#include <Semaphore.hpp>
#include <ServerConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <MessageManager.hpp>

class SessionController {
 public:
    SessionController(std::string username, PersistenceManager& pm, MessageManager& mm);
    ~SessionController();

    bool newSession(int csfd);
    void closeSession(int csfd);
    size_t getNumSessions();

    void addFollower(std::string follower);
    void sendMessage(std::string message);

    void deliverMessages();

    SessionController operator=(const SessionController&) = delete;

 private:
    std::string _username;
    PersistentUser _pUser;
    int _numSessions = 0;

    std::set<int> _sessionSFD; // Socket file descriptors

    MessageManager& _mm;

    bool _success = false;

    Semaphore _sem;

};
