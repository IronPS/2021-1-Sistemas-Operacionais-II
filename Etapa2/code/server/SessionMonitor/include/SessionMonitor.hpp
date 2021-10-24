
#pragma once

#include <map>
#include <PersistenceManager.hpp>
#include <SessionController.hpp>
#include <Semaphore.hpp>
#include <MessageManager.hpp>

class SessionMonitor {
 public:
    SessionMonitor(PersistenceManager& pm);
    ~SessionMonitor();

    SessionController* createSession(std::string username, std::string listenerPort, int csfd, bool& success);
    void closeSession(std::string username, int csfd, bool sendClose = true, bool closeConnection = true);

    void getControl();
    void freeControl();

    SessionController* getSession(std::string username);
    std::map<std::string, SessionController*>* getSessions() { return &_sessions; }

    void markDeliveredMessage(std::string userTo, uint64_t messageID);

 private:
    PersistenceManager& _pm;
    MessageManager _mm;
    std::map<std::string, SessionController*> _sessions;

    Semaphore _mapSem;

};
