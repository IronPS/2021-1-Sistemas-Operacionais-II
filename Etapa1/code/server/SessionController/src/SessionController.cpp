
#include <SessionController.hpp>

SessionController::SessionController(std::string username, PersistenceManager& pm)
: _pUser(pm.loadUser(username), pm) 
{
    _username = username;

}

SessionController::~SessionController() {

}

bool SessionController::newSession() {
    bool res = true;
    _sem.wait();

    if (_numSessions < 2) {
        _numSessions += 1;
    } else {
        res = false;
    }

    _sem.notify();

    return res;
}

void SessionController::closeSession(int csfd) {
    _sem.wait();
    _numSessions -= 1;

    if (_numSessions < 0) _numSessions = 0;

    ServerConnectionManager::closeConnection(csfd);

    _sem.notify();
}

size_t SessionController::getNumSessions() {
    return _numSessions;
}