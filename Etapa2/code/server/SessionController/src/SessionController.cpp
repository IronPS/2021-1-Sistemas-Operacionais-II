
#include <SessionController.hpp>
#include <ReplicaManager.hpp>

SessionController::SessionController(std::string username, PersistenceManager& pm, MessageManager& mm)
: _pUser(pm.loadUser(username), pm), _mm(mm), _sem(1)
{
    _username = _pUser.name();

}

SessionController::~SessionController() {

}

bool SessionController::newSession(int csfd, unsigned short listenerPort) {
    bool success = false;
    _sem.wait();

    if (_numSessions < 2) {
        success = true;
    } else {
        success = false;
    }
    _numSessions += 1; // Because it will be deleted immediately if failed

    _sessionsSFD.insert({csfd, listenerPort});

    _sem.notify();

    return success;
}

void SessionController::closeSession(int csfd, bool sendClose, bool closeConnection) {
    _sem.wait();
    _numSessions -= 1;

    if (_numSessions < 0) _numSessions = 0;

    if (sendClose) ServerConnectionManager::dataSend(csfd, PacketBuilder::close());

    if (closeConnection) ServerConnectionManager::closeConnection(csfd);

    _sessionsSFD.erase(csfd);

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

void SessionController::deliverMessages(ReplicaManager& rm) {
    _sem.wait();
    PacketData::packet_t packet = _mm.getPacket(_pUser.name(), true);

    while (packet.type != PacketData::PacketType::NOTHING) {
        std::cout << packet.timestamp << "\n";
        if (!rm.waitCommit(PacketBuilder::deliveredMessage(_username, packet.timestamp))) {
            break;
        }

        _mm.getPacket(_pUser.name(), false); // Dequeue
        for (auto sfd : _sessionsSFD) {
            ServerConnectionManager::dataSend(sfd.first, packet);
        }

        packet = _mm.getPacket(_pUser.name(), true);
        
    }

    _sem.notify();

}
