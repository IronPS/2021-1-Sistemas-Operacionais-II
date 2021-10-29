
#include <ReplicaManager.hpp>

ReplicaManager::ReplicaManager(const cxxopts::ParseResult& input, PersistenceManager& pm, SessionMonitor& sm) 
: _id(input["id"].as<unsigned short>()), _em(input), _rm(input), _pm(pm), _sm(sm)
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    _addresses = input["addresses"].as<std::vector<std::string>>();
    std::vector<unsigned short> auxPorts = input["auxports"].as<std::vector<unsigned short>>();
    _cliPorts = input["cliports"].as<std::vector<unsigned short>>();

    assert(_addresses.size() == _ids.size());
    assert(_addresses.size() == _cliPorts.size());

    size_t numAuxPorts = 0;
    for (size_t i = 0; i < _ids.size(); i++) {
        for (size_t j = 0; j < _ids.size()-1; j++) {
            numAuxPorts += 1;
        }
    }

    assert(numAuxPorts == auxPorts.size());

    int size = (int) _ids.size();
    int count = 0;
    int mod = -1;
    for (auto i : _ids) {
        if (i == _id) {
            mod = 0;
            continue;
        }

        int myPortPos = _id * (size - 1) + count;
        int otherPortPos = i * (size - 1) + _id + mod;

        _connections.push_back(std::shared_ptr<ReplicaConnection>(
            new ReplicaConnection(_em, _rm,
                                  _id, _addresses[_id], auxPorts[myPortPos],
                                  _ids[i], _addresses[i], auxPorts[otherPortPos])
            )
        );

        count += 1;

    }

}

ReplicaManager::~ReplicaManager() {

}

void ReplicaManager::start() {
    while (signaling::_continue) {  
        _rmSem.beginWrite();
            for (auto con : _connections) {
                con->loop();
            }
        _rmSem.endWrite();


        _em.block();

            if (!_em.unlockedLeaderIsAlive()) {
                _mustUpdateClients = true;
                _em.startElection();
            }
            
            while (signaling::_continue && !_em.unlockedLeaderIsAlive()) {
                ElectionManager::Action action = _em.action();
                for (auto con : _connections) {
                    con->electionState(action);
                }
                _em.step();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

            }

            _rm.setEpoch(_em.epoch());

        _em.unblock();
        
        if (_em.leaderIsAlive()) {
            for (auto conn : _connections) {
                if (!conn->connected()) {
                    _rm.setDead(conn->otherID());
                } else {
                    _rm.setAlive(conn->otherID());
                }

            }

            if (_em.unlockedIsLeader() && _mustUpdateClients) {
                _updateClients();
            }
            _mustUpdateClients = false;
            _rm.clear();

            _rmSem.beginWrite();
                if (_em.isLeader()) _rm.checkSent();

                if (_id == _em.getLeaderID()) {
                    // Replication Manager functionality for leader
                    bool firstIt = true;
                    ReplicationManager::ReplicationData* rep = nullptr;

                    while (_rm.getNextToSend(&rep, firstIt)) {
                        firstIt = false;
                        for (auto conn : _connections) {
                            bool res = conn->sendReplication(rep->packet);
                            _rm.updateSend(*rep, conn->otherID(), res);
                        }

                    }


                } else {
                    // Replication Manager functionality for replica
                    bool firstIt = true;
                    ReplicationManager::ReplicationData* rep = nullptr;
                    auto conn = _connections[_em.getLeaderID()];

                    while (_rm.getNextToConfirm(&rep, firstIt)) {
                        firstIt = false;
                        bool res = conn->sendReplication(PacketBuilder::confirmReplication(_em.epoch(), rep->packet.timestamp));
                        _rm.updateConfirm(*rep, res);
                    }

                    // Only passive replicas commit through here
                    // Leader commits through the user thread
                    firstIt = true;
                    while (_rm.getNextToCommit(&rep, firstIt)) {
                        firstIt = false;
                        commit(rep->packet);
                    }

                }

                _rm.checkTimeouts();

            _rmSem.endWrite();

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));

    }
}

PacketData::packet_t ReplicaManager::getLeaderInfo() {
    return PacketBuilder::leaderInfo(_addresses[_em.unlockedGetLeaderID()], _cliPorts[_em.unlockedGetLeaderID()]);
}

unsigned short ReplicaManager::_sinfoPt = 0;
ServerData::server_info_t ReplicaManager::getNextServerInfo() {
    std::ifstream serverInfos("servers.data");
    
    ServerData::server_info_t sinfo;

    bool found = false;
    std::string line;
    if (serverInfos.good()) {
        serverInfos.seekg(std::ios::beg);
        for (int lineCursor = 0; std::getline(serverInfos, line); lineCursor++) {
            if (lineCursor == _sinfoPt) {
                found = true;
                break;
            }
        }
        
        serverInfos.clear(); // Clear errors

        if (!found) {
            serverInfos.seekg(std::ios::beg);
            std::getline(serverInfos, line);

            _sinfoPt = 0;

        }

        _sinfoPt += 1;

        size_t pos = line.find(' ');
        sinfo.address = line.substr(0, pos);
        sinfo.port = line.substr(pos+1);

    }

    return sinfo;
}

bool ReplicaManager::waitCommit(PacketData::packet_t commandPacket) {
    bool success = false;

    uint64_t replicationID = 0;
    
    ReplicationManager::ReplicationState state = ReplicationManager::ReplicationState::SEND;
    commandPacket.seqn = _em.epoch();

    _rmSem.beginRead();
    if (_rm.newReplication(commandPacket, replicationID)) {
        _rmSem.endRead();
        do {
            _rmSem.beginRead();
                state = _rm.getMessageState(replicationID);
            _rmSem.endRead();

            std::this_thread::sleep_for(std::chrono::milliseconds(25));

        } while (
            signaling::_continue
            && state != ReplicationManager::ReplicationState::COMMITTED 
            && state != ReplicationManager::ReplicationState::CANCELED
        );

    } else {
        _rmSem.endRead();

    }

    if (state == ReplicationManager::ReplicationState::COMMITTED) {
        success = true;
    }

    return success;
}

void ReplicaManager::commit(const PacketData::packet_t packet) {
    size_t strPos1 = 0;
    size_t strPos2 = 0;
    int csfd = -1;
    std::string tmpStr;
    std::string cmdStr;
    std::string substr;
    std::string listenerPort;
    SessionController* session = nullptr;
    bool success;

    switch (packet.rtype) {
        case PacketData::ReplicationType::R_NEWMESSAGE:
            session = _sm.getSession(packet.extra);
            if (session != nullptr) {
                std::cout << "Creating message '" << packet.timestamp << "'\n";
                session->sendMessage(packet.payload, packet.timestamp);
            }

            break;
          
        case PacketData::ReplicationType::R_DELMESSAGE:
            std::cout << "Marking message '" << packet.payload << "' as delivered\n";
            _sm.markDeliveredMessage(packet.extra, std::stol(packet.payload));
            break;
         
        case PacketData::ReplicationType::R_SESSION:
            tmpStr = packet.payload;
            strPos1 = tmpStr.find(",");
            
            if (strPos1 == std::string::npos) return;

            cmdStr = tmpStr.substr(0, strPos1);

            substr = tmpStr.substr(strPos1+1);
            if (cmdStr == "LOGIN") {
                strPos2 = substr.find(",");

                if (strPos2 == std::string::npos) return;

                csfd = std::stol(tmpStr.substr(strPos1+1, strPos2));
                listenerPort = substr.substr(strPos2+1);
            
                _sm.createSession(packet.extra, listenerPort, csfd, success);
                if (success) {
                    std::cout << "Created replicated session to user " << packet.extra << "\n";
                } else {
                    std::cout << "Failed to create replicated session to user " << packet.extra << "\n";

                }

            } else if (cmdStr == "CLOSE") {
                csfd = std::stol(substr);
                _sm.closeSession(packet.extra, csfd, false, false);
                
                std::cout << "Closed replicated session of user " << packet.extra << "\n";
            }
        
            break;

        case PacketData::ReplicationType::R_FOLLOWER:
            success = true;
            session = _sm.getSession(packet.extra);
            if (session != nullptr) {
                session->addFollower(packet.payload);
            } else { // No session open
                User user = _pm.loadUser(packet.extra, false);

                if (user.name() == packet.extra) {
                    user.addFollower(packet.payload);
                    _pm.saveUser(user);
                } else {
                    success = false;
                }
            }

            if (success) {
                std::cout << "User " << packet.payload << " now following " << packet.extra << " on replica\n";
            }

            break;
            
        case PacketData::ReplicationType::R_CONFIRM:
            break;
            
        case PacketData::ReplicationType::R_NONE:
            break;

    }
}

void ReplicaManager::_updateClients() {
    time_t timer;
    time_t t_now;
    bool timedOut = false;
    double timeout = 0.15; // 150 milliseconds
    
    _sm.getControl();

        auto sessions = _sm.getSessions();

        for (auto it = sessions->begin(); it != sessions->end(); /* Nothing */) {
            for (auto sess : it->second->getSessions()) {

                bool res = false;
                time(&timer);
                while (!res) {
                    res = true;
                    PacketData::packet_t packet = PacketBuilder::replicateSession(it->second->username(), "CLOSE," + std::to_string(sess.first));
                    packet.seqn = _em.epoch();
                    for (auto conn : _connections) {
                        res &= conn->sendReplication(packet, true);
                    }

                    time(&t_now);
                    if (difftime(t_now, timer) > timeout) {
                        timedOut = true;
                    }
                    if (timedOut) break;

                    std::this_thread::sleep_for(std::chrono::milliseconds(5));

                }

                ClientConnectionManager cm("127.0.0.1", sess.second);

                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 1000; // 10 ms
                setsockopt(cm.getSFD(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
                
                time(&timer);
                while (signaling::_continue && !cm.openConnection(false, false, true)) {
                    time(&t_now);
                    if (difftime(t_now, timer) > timeout) {
                        timedOut = true;
                    }
                    if (timedOut) break;
                }
                
                ssize_t bytes_sent = 0;
                time(&timer);
                while(signaling::_continue && ((bytes_sent = cm.dataSend(getLeaderInfo())) == 0 || bytes_sent == -1)) {
                    time(&t_now);
                    if (difftime(t_now, timer) > timeout) {
                        timedOut = true;
                    }
                    if (timedOut) break;
                }

            }

            delete it->second;
            sessions->erase(it++);

        }


    _sm.freeControl();
}
