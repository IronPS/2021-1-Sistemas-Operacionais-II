
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

    SessionController* createSession(std::string username, int csfd, bool& success);
    void closeSession(std::string username, int csfd);

    void getControl();
    void freeControl();

    SessionController* getSession(std::string username);


 private:
    PersistenceManager& _pm;
    MessageManager _mm;
    std::map<std::string, SessionController*> _sessions;

    Semaphore _mapSem;

};
