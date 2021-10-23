
#include <SessionMonitor.hpp>

SessionMonitor::SessionMonitor(PersistenceManager& pm, ReplicaManager& rm) 
: _pm(pm), _rm(rm), _mm(rm), _mapSem(1)
{
}

SessionMonitor::~SessionMonitor() {
    for (auto sess : _sessions) {
        delete sess.second;
    }
}

SessionController* SessionMonitor::createSession(std::string username, std::string listenerPort, int csfd, bool& success) {
    if (!_rm.waitCommit(PacketBuilder::replicateSession(username, "LOGIN"))) {
        std::cerr << "Failed to replicate session\n";

        success = false;
        return nullptr;
    }

    _mapSem.wait();

    success = false;
    if (!_sessions.count(username)) {
        SessionController* sess = new SessionController(username, _pm, _mm, _rm);
        if (!sess->isValid()) {
            delete sess;
            success = false;
            return nullptr;
        }
        _sessions[username] = sess;
    }
    success = _sessions[username]->newSession(csfd, std::atoi(listenerPort.c_str()));

    _mapSem.notify();

    return _sessions[username];
}

void SessionMonitor::closeSession(std::string username, int csfd, bool sendClose) {
    _rm.waitCommit(PacketBuilder::replicateSession(username, "CLOSE"));

    _mapSem.wait();

    _sessions[username]->closeSession(csfd, sendClose);

    if (_sessions[username]->getNumSessions() == 0) {
        delete _sessions[username];
        _sessions[username] = nullptr;
        _sessions.erase(username);
    }

    _mapSem.notify();
}

void SessionMonitor::getControl() {
    _mapSem.wait();
}

void SessionMonitor::freeControl() {
    _mapSem.notify();
}

SessionController* SessionMonitor::getSession(std::string username) {
    if (_sessions.count(username)) return _sessions[username];
    else return nullptr;
}
