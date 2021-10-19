
#include <ReplicaManager.hpp>

ReplicaConnection::ReplicaConnection(ElectionManager& em,
                                     unsigned short thisID, std::string thisAddr, unsigned int thisPort, 
                                     unsigned short otherID, std::string otherAddr, unsigned int otherPort)
: _em(em)
{
    _thisID = thisID;
    _otherID = otherID;

    _thisAddr = thisAddr;
    _thisPort = thisPort;
    _otherAddr = otherAddr;
    _otherPort = otherPort;

    if (_thisID < _otherID) {
        _isServer = true;
        _sm = new ServerConnectionManager(_thisPort);

        assert(_sm);
    } else {
        _isServer = false;
        _cm = new ClientConnectionManager(_otherAddr, _otherPort);

        assert(_cm);
    }

}

ReplicaConnection::~ReplicaConnection() {
    if (_isServer) {
        if (_sm != nullptr) {
            delete _sm;
        }
        _sm = nullptr;

    } else {
        if (_cm != nullptr) {
            delete _cm;
        }
        _cm = nullptr;

    }

    ServerConnectionManager::closeConnection(_sfd);

}

void ReplicaConnection::loop() {
    if (!_connected)
        _connect();

    if (_connected) {
        _receivePacket();
        _sendHeartbeat();
    }

}

void ReplicaConnection::_connect() {
    if (_attemptingConnectionMessage) {
        std::cout << "Attempting connection with " << _otherID << std::endl;
        _attemptingConnectionMessage = false;
    }

    if (_isServer) {
        _sfd = _sm->getConnection();
        if (_sfd != -1) _connected = true;

    } else {
        _connected = _cm->openConnection(false, false);
        if (_connected) _sfd = _cm->getSFD();

    }

    if (_connected) {
        time(&_lastReceivedHeartbeat);
        std::cout << "Connected to " << _otherID << "\n";
        _attemptingConnectionMessage = true;
    }

}

void ReplicaConnection::_receivePacket() {
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    auto bytesReceived = ServerConnectionManager::dataReceive(_sfd, packet);

    if (bytesReceived > 0 && bytesReceived != -1) {
        time(&_lastReceivedHeartbeat); // Every packet can behave as a heartbeat

        switch(packet.type) {
            case PacketData::packet_type::HEARTBEAT:
                break;

            default:
                ;
        }
    } else {
        time_t now;
        time(&now);

        if (difftime(now, _lastReceivedHeartbeat) > _timeout) {
            _lostConnection();

        }
    }
}

void ReplicaConnection::_sendHeartbeat() {
    time_t now;
    time(&now);

    if (difftime(now, _lastSentHeartbeat) > _heartbeatInterval) {
        auto bytesSent = ServerConnectionManager::dataSend(_sfd, PacketBuilder::heartbeat(_thisID));

        if (bytesSent > 0 && bytesSent != -1) {
            time(&_lastSentHeartbeat);
        }
    }
}

void ReplicaConnection::_lostConnection() {
    _connected = false;
    if (_isServer) {
        ServerConnectionManager::closeConnection(_sfd);

    } else {
        _cm->closeConnection();
    }

    _sfd = -1;

    std::cout << "Lost server " << _otherID << std::endl;

    if (_otherID == _em.getLeaderID()) {
        _em.unsetLeaderIsAlive();
    }
}