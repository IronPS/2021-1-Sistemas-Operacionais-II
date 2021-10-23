
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

            if (!_em.unlockedLeaderIsAlive()) _em.startElection();
            
            while (signaling::_continue && !_em.unlockedLeaderIsAlive()) {
                ElectionManager::Action action = _em.action();
                for (auto con : _connections) {
                    con->electionState(action);
                }
                _em.step();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

            }

        _em.unblock();

        if (_em.leaderIsAlive()) {
            for (auto conn : _connections) {
                if (!conn->connected()) {
                    _rm.setDead(conn->otherID());
                } else {
                    _rm.setAlive(conn->otherID());
                }

            }

            _rmSem.beginWrite();

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
                        bool res = conn->sendReplication(PacketBuilder::confirmReplication(rep->packet.timestamp));
                        _rm.updateConfirm(*rep, res);
                    }

                    // Only passive replicas commit through here
                    // Leader commits through the user thread
                    firstIt = true;
                    while (_rm.getNextToCommit(&rep, firstIt)) {
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
    return PacketBuilder::leaderInfo(_addresses[_em.getLeaderID()], _cliPorts[_em.getLeaderID()]);
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

    _rmSem.beginRead();
    if (_rm.newReplication(commandPacket, replicationID)) {
        _rmSem.endRead();
        do {
            _rmSem.beginRead();
                state = _rm.getMessageState(replicationID);
            _rmSem.endRead();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

void ReplicaManager::commit(PacketData::packet_t packet) {
    switch (packet.rtype) {
        case PacketData::ReplicationType::R_NEWMESSAGE:
            break;
          
        case PacketData::ReplicationType::R_DELMESSAGE:
            break;
         
        case PacketData::ReplicationType::R_SESSION:
            break;

        case PacketData::ReplicationType::R_USER:
            break;
            
        case PacketData::ReplicationType::R_CONFIRM:
            break;
            
        case PacketData::ReplicationType::R_NONE:
            break;

    }
}
