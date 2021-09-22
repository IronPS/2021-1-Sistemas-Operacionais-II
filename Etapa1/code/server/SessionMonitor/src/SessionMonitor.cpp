
#include <SessionMonitor.hpp>

SessionMonitor::SessionMonitor(PersistenceManager& pm) 
: _pm(pm), _mapSem(1) 
{

}

SessionMonitor::~SessionMonitor() {
    for (auto sess : _sessions) {
        delete sess.second;
    }
}

SessionController* SessionMonitor::createSession(std::string username, bool& success) {
    _mapSem.wait();

    success = false;
    if (!_sessions.count(username)) {
        SessionController* sess = new SessionController(username, _pm);
        _sessions[username] = sess;
    }
    success = _sessions[username]->newSession();

    _mapSem.notify();

    return _sessions[username];
}

void SessionMonitor::closeSession(std::string username, int csfd) {
    _mapSem.wait();

    _sessions[username]->closeSession(csfd);

    if (_sessions[username]->getNumSessions() == 0) {
        delete _sessions[username];
        _sessions[username] = nullptr;
        _sessions.erase(username);
    }

    _mapSem.notify();
}
