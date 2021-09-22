
#include <SessionController.hpp>

SessionController::SessionController(std::string username, PersistenceManager& pm)
: _pUser(pm.loadUser(username), pm), _sem(1)
{
    _username = username;

}

SessionController::~SessionController() {

}

bool SessionController::newSession() {
    bool success = false;
    _sem.wait();

    if (_numSessions < 2) {
        success = true;
    } else {
        success = false;
    }
    _numSessions += 1; // Because it will be deleted immediately if failed

    _sem.notify();

    return success;
}

void SessionController::closeSession(int csfd) {
    _sem.wait();
    _numSessions -= 1;

    if (_numSessions < 0) _numSessions = 0;

    auto bytes_sent = ServerConnectionManager::dataSend(csfd, PacketBuilder::close());

    std::cout << "sent " << bytes_sent << std::endl;
    ServerConnectionManager::closeConnection(csfd);

    _sem.notify();
}

size_t SessionController::getNumSessions() {
    return _numSessions;
}