
#pragma once

#include <PersistenceManager.hpp>
#include <PersistentUser.hpp>
#include <Semaphore.hpp>
#include <ServerConnectionManager.hpp>

class SessionController {
 public:
    SessionController(std::string username, PersistenceManager& pm);
    ~SessionController();

    bool newSession();
    void closeSession(int csfd);
    size_t getNumSessions();


    SessionController operator=(const SessionController&) = delete;

 private:
    std::string _username;
    PersistentUser _pUser;
    int _numSessions = 0;

    Semaphore _sem;

};
