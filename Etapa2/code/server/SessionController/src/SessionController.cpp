
#include <SessionController.hpp>

SessionController::SessionController(std::string username, PersistenceManager& pm, MessageManager& mm)
: _pUser(pm.loadUser(username), pm), _mm(mm), _sem(1)
{
    _username = username;

}

SessionController::~SessionController() {

}

bool SessionController::newSession(int csfd) {
    bool success = false;
    _sem.wait();

    if (_numSessions < 2) {
        success = true;
    } else {
        success = false;
    }
    _numSessions += 1; // Because it will be deleted immediately if failed

    _sessionSFD.insert(csfd);

    _sem.notify();

    return success;
}

void SessionController::closeSession(int csfd) {
    _sem.wait();
    _numSessions -= 1;

    if (_numSessions < 0) _numSessions = 0;

    auto bytes_sent = ServerConnectionManager::dataSend(csfd, PacketBuilder::close());

    ServerConnectionManager::closeConnection(csfd);

    _sessionSFD.erase(csfd);

    _sem.notify();
}

void SessionController::addFollower(std::string follower) {
    _sem.wait();
    _pUser.addFollower(follower);
    _sem.notify();
}


size_t SessionController::getNumSessions() {
    return _numSessions;
}

void SessionController::sendMessage(std::string message) {
    _mm.processIncomingMessage(_pUser, message, static_cast<uint64_t>(time(0)));
}

void SessionController::deliverMessages() {
    _sem.wait();
    PacketData::packet_t packet = _mm.getPacket(_pUser.name());

    while (packet.type != PacketData::packet_type::NOTHING) {      
        for (auto sfd : _sessionSFD) {
            ServerConnectionManager::dataSend(sfd, packet);
        }

        packet = _mm.getPacket(_pUser.name());
    }

    _sem.notify();

}
