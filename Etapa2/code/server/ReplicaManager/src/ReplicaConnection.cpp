
#include <ReplicaConnection.hpp>

ReplicaConnection::ReplicaConnection(ElectionManager& em, ReplicationManager& rm,
                                     unsigned short thisID, std::string thisAddr, unsigned int thisPort, 
                                     unsigned short otherID, std::string otherAddr, unsigned int otherPort)
: _em(em), _rm(rm)
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

    time(&_lastSentHeartbeat);

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
    if (!_connected && _first_connection) {
        _connect();
    }

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
        if (_sfd != -1) {
            _connected = true;
            _first_connection = false;

            struct timeval tv;
            tv.tv_sec = 0; // 0 seconds
            tv.tv_usec = 25 * 1000; // 25 milliseconds
            setsockopt(_sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        }

    } else {
        _connected = _cm->openConnection(false, false, true);
        if (_connected) {
            _sfd = _cm->getSFD();
            _first_connection = false;

            struct timeval tv;
            tv.tv_sec = 0; // 0 seconds
            tv.tv_usec = 25 * 1000; // 25 milliseconds
            setsockopt(_sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        }

    }

    if (_connected) {
        time(&_lastReceivedHeartbeat);
        std::cout << "Connected to " << _otherID << "\n";
        _attemptingConnectionMessage = true;
    }

}

void ReplicaConnection::_receivePacket(bool ignoreTimeout) {
    PacketData::packet_t packet;
    packet.type = PacketData::PacketType::NOTHING;

    auto bytesReceived = ServerConnectionManager::dataReceive(_sfd, packet);

    if (bytesReceived > 0 && bytesReceived != -1) {
        time(&_lastReceivedHeartbeat); // Every packet can behave as a heartbeat

        switch(packet.type) {
            case PacketData::PacketType::HEARTBEAT:
                // std::cout << "Received HB from " << _otherID << "\n";
                break;

            case PacketData::PacketType::ELECTION:
                // std::cout << "Received ELECTION from " << _otherID << "\n";
                ServerConnectionManager::dataSend(_sfd, PacketBuilder::serverSignal(_thisID, PacketData::PacketType::ANSWER, _em.epoch()));
                if (_em.unlockedIsLeader()) {
                    ServerConnectionManager::dataSend(_sfd, PacketBuilder::serverSignal(_thisID, PacketData::PacketType::COORDINATOR, _em.epoch()));

                } else {
                    _em.receivedElection();
                }
                break;

            case PacketData::PacketType::ANSWER:
                _em.receivedAnswer(packet.seqn);
                // if (packet.seqn >= _em.epoch()) std::cout << "Received answer from " << _otherID << "\n";
                break;

            case PacketData::PacketType::COORDINATOR:
                // std::cout << "Received COORDINATOR from " << _otherID << "\n";
                _em.receivedCoordinator(_otherID, packet.seqn);
                break;
            
            case PacketData::PacketType::REPLICATE:
                // std::cout << "Received REPLICATE from " << _otherID << "\n";
                _rm.processReceivedPacket(packet, _otherID);
                break;

            default:
                ;
        }
    } else if (!ignoreTimeout) {
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
        // std::cout << "Sending HB to " << _otherID << std::endl;
        auto bytesSent = ServerConnectionManager::dataSend(_sfd, PacketBuilder::heartbeat(_thisID));

        if (bytesSent > 0 && bytesSent != -1) {
            time(&_lastSentHeartbeat);
        }
    }
}

void ReplicaConnection::_lostConnection() {
    if (!_connected) return;

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

void ReplicaConnection::electionState(ElectionManager::Action action) {
    switch (action) {
        case ElectionManager::Action::SendElection:
            if (_thisID > _otherID) {
                ServerConnectionManager::dataSend(_sfd, PacketBuilder::serverSignal(_thisID, PacketData::PacketType::ELECTION, _em.epoch()));
            }
            break;

        case ElectionManager::Action::SendCoordinator:
            ServerConnectionManager::dataSend(_sfd, PacketBuilder::serverSignal(_thisID, PacketData::PacketType::COORDINATOR, _em.epoch()));

            break;
        case ElectionManager::Action::WaitAnswer:
        case ElectionManager::Action::WaitElection:
            _receivePacket(true);
            break;

        case ElectionManager::Action::None:
            break;
    }
}

bool ReplicaConnection::sendReplication(PacketData::packet_t packet, bool ignoreConnected) {
    bool success = false;

    if (!_connected) {
        return ignoreConnected;
    }

    auto bytesSent = ServerConnectionManager::dataSend(_sfd, packet);

    if (bytesSent > 0 && bytesSent != -1) {
        success = true;
    }

    return success;
}
