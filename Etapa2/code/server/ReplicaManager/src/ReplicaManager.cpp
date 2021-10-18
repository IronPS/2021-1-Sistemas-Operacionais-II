
#include <ReplicaManager.hpp>

ReplicaManager::ReplicaManager(const cxxopts::ParseResult& input) 
: _id(input["id"].as<unsigned short>()), _hbSem(1), _sfdSem(1), _gen(_id), _intDistr(0, 1000), _serverSem(1)
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    _addresses = input["addresses"].as<std::vector<std::string>>();
    _auxPorts = input["auxports"].as<std::vector<unsigned short>>();
    _cliPorts = input["cliports"].as<std::vector<unsigned short>>();

    assert(_addresses.size() == _auxPorts.size());
    assert(_addresses.size() == _ids.size());
    assert(_addresses.size() == _cliPorts.size());

    time_t timer;
    _socketFileDescriptors.resize(_addresses.size());
    _lastHeartbeat.resize(_addresses.size());
    _cms.resize(_addresses.size());
    for (auto i : _ids) {
        _socketFileDescriptors[i] = -1;

        if (i == _id) continue;
        time(&timer);
        _lastHeartbeat[i] = timer;
        _cms[i] = new ClientConnectionManager(_addresses[i], _auxPorts[i]);
    }

    _state = State::ELECTION;

    signaling::_heartbeat = true;

    if (_auxPorts.size() > 0) _sm = new ServerConnectionManager(_auxPorts[_id]);
}

ReplicaManager::~ReplicaManager() {
    for (auto& sfd : _socketFileDescriptors) {
        if (sfd != -1) close(sfd);
    }
    if (_sm != nullptr) delete _sm;
    _sm = nullptr;
}

void ReplicaManager::handleReplicas() {
    if (_auxPorts.size() == 0) return;

    PacketData::packet_t heartbeat;

    std::vector<std::thread> threads;
    for (auto i : _ids) {
        if (i == _id ) continue;
        std::thread t = std::thread(&ReplicaManager::_connectionHandler, this, i);
        threads.push_back(std::move(t));
    }

    while (signaling::_continue) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Receive heartbeats, detect server is down and update SFD
        for (auto i : _ids) {
            if (i == _id) continue;

            _sfdSem.wait();
                int sfd = _socketFileDescriptors[i];
            _sfdSem.notify();

            _receiveHeartbeat(i, sfd);
        }
        
        if (signaling::_heartbeat) {
            heartbeat = PacketBuilder::heartbeat(_id);
            for (auto i : _ids) {
                _sfdSem.wait();
                    int sfd = _socketFileDescriptors[i];
                _sfdSem.notify();

                if (sfd != -1) {
                    ServerConnectionManager::dataSend(sfd, heartbeat);
                    // std::cout << "Heartbeat sent to " << i << " at " << sfd << std::endl;
                }
            }
            signaling::_heartbeat = false;
        }
    }

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }
}

PacketData::packet_t ReplicaManager::getLeaderInfo() {
    return PacketBuilder::leaderInfo(_addresses[_leaderID], _cliPorts[_leaderID]);
}

void ReplicaManager::_receiveHeartbeat(unsigned short id, int sfd) {
    time_t timer;
    size_t bytesReceived = 0;
    PacketData::packet_t heartbeat;
    heartbeat.type = PacketData::NOTHING;

    time(&timer);
    if (sfd != -1) {
        bytesReceived = ServerConnectionManager::dataReceive(sfd, heartbeat);

        if (bytesReceived > 0 && heartbeat.type == PacketData::HEARTBEAT) {
            std::cout << "Heartbeat from " << id << std::endl;
            _hbSem.wait();
                _lastHeartbeat[id] = timer;
            _hbSem.notify();

        } else {
            _hbSem.wait();
                time_t interval = difftime(timer, _lastHeartbeat[id]);
            _hbSem.notify();

            if (interval > _timeout) {
                std::cout << "Server " << id << " lost with " << interval << " seconds" << std::endl;
                _sfdSem.wait();
                    _socketFileDescriptors[id] = -1;
                _sfdSem.notify();
                close(sfd);


                if (id == _leaderID) {
                    // TODO launch election
                }

            }

        }

    }

}

void ReplicaManager::_connectionHandler(unsigned short i) {
    // Try to connect to address[i] port[i] as client
    // If does not work, starts listening on the port waiting for this replica
    std::this_thread::sleep_for(std::chrono::milliseconds(_id * 2000 + i * 250));    

    while (signaling::_continue) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_id*500 + _intDistr(_gen)));

        _sfdSem.wait();
            int sfd = _socketFileDescriptors[i];
        _sfdSem.notify();

        if (sfd == -1) {
            _connectAsClient(i);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500 + _intDistr(_gen)));

        _sfdSem.wait();
            sfd = _socketFileDescriptors[i];
        _sfdSem.notify();

        if (sfd == -1) {
            _connectAsServer(i);
        }
    }
}

void ReplicaManager::_connectAsClient(unsigned short i) {
    time_t timer;

    bool success = _cms[i]->openConnection(false, true);

    if (success) {
        success = _connectionCheck(i);

        if (!success) return;

        std::cout << "As Client: Client replica " << _id << " connected to " << i << std::endl;

        _sfdSem.wait();
            _socketFileDescriptors[i] = _cms[i]->getSFD();
            fcntl(_socketFileDescriptors[i], F_SETFL, fcntl(_socketFileDescriptors[i], F_GETFL, 0) | O_NONBLOCK);

        _sfdSem.notify();
        
        _hbSem.wait();
            time(&timer);
            _lastHeartbeat[i] = timer;
        _hbSem.notify();

    }
}

void ReplicaManager::_connectAsServer(unsigned short i) {
    time_t timer;

    _serverSem.wait();
        int sfd = _sm->getConnection();
    _serverSem.notify();

    bool success = _connectionConfirm(i, sfd);
    
    if (success) {
        std::cout << "As Server: Client replica " << i << " connected to " << _id << std::endl;

        _sfdSem.wait();
            _socketFileDescriptors[i] = sfd;
            fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL, 0) | O_NONBLOCK);
        _sfdSem.notify();
        
        _hbSem.wait();
            time(&timer);
            _lastHeartbeat[i] = timer;
        _hbSem.notify();

    }

}

bool ReplicaManager::_connectionCheck(unsigned short i) {
    bool success = false;
    PacketData::packet_t packet;
    PacketData::packet_t packet_backup;

    int sfd = _cms[i]->getSFD();

    // Set timeout
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    packet = PacketBuilder::heartbeat(_id);
    ServerConnectionManager::dataSend(sfd, packet);

    std::this_thread::sleep_for(std::chrono::milliseconds(5000 + _intDistr(_gen)));

    packet.type = PacketData::ERROR;
    ssize_t res = 0;
    do {
        res = ServerConnectionManager::dataReceive(sfd, packet);
        packet_backup = packet;

        int id_ = std::atoi(packet_backup.extra);

        if (id_ < _id && packet_backup.type == PacketData::HEARTBEAT && id_ == i) {
            tv.tv_sec = 1; // 1 seconds
            setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

            success = true;
        }
    
    } while (res != -1 && res > 0);

    if (!success) _cms[i]->closeConnection();

    return success;
}

bool ReplicaManager::_connectionConfirm(unsigned short i, int sfd) {
    bool success = false;
    if (sfd == -1) return success;

    PacketData::packet_t packet;
    PacketData::packet_t packet_backup;

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    packet.type = PacketData::ERROR;
    ssize_t res = 0;
    do {
        res = ServerConnectionManager::dataReceive(sfd, packet);
        packet_backup = packet;

        int id_ = std::atoi(packet_backup.extra);

        if (id_ > _id && packet_backup.type == PacketData::HEARTBEAT && id_ == i) {
            packet = PacketBuilder::heartbeat(_id);
            ServerConnectionManager::dataSend(sfd, packet);

            success = true;
        }

    } while (res != -1 && res > 0);
    
    if (!success) {
        packet.type = PacketData::ERROR;
        ServerConnectionManager::dataSend(sfd, packet);
    }

    tv.tv_sec = 1;
    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    return success;

}

unsigned short ReplicaManager::_sinfoPt = 0;
ReplicaManager::server_info_t ReplicaManager::getNextServerInfo() {
    std::ifstream serverInfos("servers.data");
    
    server_info_t sinfo;

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
