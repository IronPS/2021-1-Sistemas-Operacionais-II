
#include <ReplicaManager.hpp>

ReplicaManager::ReplicaManager(const cxxopts::ParseResult& input) 
: _id(input["id"].as<unsigned short>())
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

        _connections.push_back(std::shared_ptr<ReplicaConnection>(new ReplicaConnection(_id, _addresses[_id], auxPorts[myPortPos],
                          _ids[i], _addresses[i], auxPorts[otherPortPos])));

        count += 1;

    }

}

ReplicaManager::~ReplicaManager() {

}

void ReplicaManager::start() {
    
    while (signaling::_continue) {
        for (auto con : _connections) {
            con->loop();
        }

    }
}

PacketData::packet_t ReplicaManager::getLeaderInfo() {
    return PacketBuilder::leaderInfo(_addresses[_leaderID], _cliPorts[_leaderID]);
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

