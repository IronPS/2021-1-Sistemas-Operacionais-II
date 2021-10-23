
#pragma once

#include <map>
#include <PersistenceManager.hpp>
#include <SessionController.hpp>
#include <Semaphore.hpp>
#include <MessageManager.hpp>
#include <ReplicaManager.hpp>

class SessionMonitor {
 public:
    SessionMonitor(PersistenceManager& pm, ReplicaManager& rm);
    ~SessionMonitor();

    SessionController* createSession(std::string username, std::string listenerPort, int csfd, bool& success);
    void closeSession(std::string username, int csfd, bool sendClose = true);

    void getControl();
    void freeControl();

    SessionController* getSession(std::string username);


 private:
    PersistenceManager& _pm;
    ReplicaManager& _rm;
    MessageManager _mm;
    std::map<std::string, SessionController*> _sessions;

    Semaphore _mapSem;

};
