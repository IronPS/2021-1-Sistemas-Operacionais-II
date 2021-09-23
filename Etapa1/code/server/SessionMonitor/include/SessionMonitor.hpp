
#pragma once

#include <map>
#include <PersistenceManager.hpp>
#include <SessionController.hpp>
#include <Semaphore.hpp>

class SessionMonitor {
 public:
    SessionMonitor(PersistenceManager& pm);
    ~SessionMonitor();

    SessionController* createSession(std::string username, bool& success);
    void closeSession(std::string username, int csfd);

    void getControl();
    void freeControl();

    SessionController* getSession(std::string username);

    bool newData() { return false; } // TODO 


 private:
    PersistenceManager& _pm;
    std::map<std::string, SessionController*> _sessions;

    Semaphore _mapSem;

};
