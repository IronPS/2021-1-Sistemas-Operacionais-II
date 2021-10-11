
#include <ReplicaManager.hpp>

ReplicaManager::ReplicaManager(const cxxopts::ParseResult& input) {
    _myPort = std::to_string(input["port"].as<unsigned short>());
    _addresses = input["addresses"].as<std::vector<std::string>>();
    _ports = input["ports"].as<std::vector<unsigned short>>();

    assert(_addresses.size() == _ports.size());

    for (auto i : _ports) {
        _socketFileDescriptors[i] = -1;
    }

}

ReplicaManager::~ReplicaManager() {
    for (auto& kv : _socketFileDescriptors) {
        if (kv.second != -1) close(kv.second);
    }
}

void ReplicaManager::handleReplicas() {
    std::vector<std::thread> threads;
    for (size_t i = 0; i < _addresses.size(); i++) {
        std::thread t = std::thread(&ReplicaManager::_connectionHandler, *this, i);
        threads.push_back(std::move(t));
    }

    while (signaling::_continue) {
        // TODO receive heartbeats, detect server is down and update SFD
        
        if (signaling::_heartbeat) {
            // TODO send heartbeat
            signaling::_heartbeat = false;
            alarm(1);
        }
    }

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }
}

void ReplicaManager::_connectionHandler(size_t i) {
    auto address = _addresses[i];
    auto port = std::to_string(_ports[i]);

    // TODO try to connect to address[i] port[i] as client
    // If does not work, starts listening on the port waiting for this replica
    while (signaling::_continue) {
        // TODO when replica that was being waited for by the thread connects, updates SFD
    }
}