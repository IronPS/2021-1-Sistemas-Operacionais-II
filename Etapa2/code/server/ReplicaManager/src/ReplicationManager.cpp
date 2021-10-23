
#include <ReplicationManager.hpp>

ReplicationManager::ReplicationManager(const cxxopts::ParseResult& input)
: _id(input["id"].as<unsigned short>()), _sem(1)
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    _dead = std::vector<bool>(_ids.size(), false);
}

ReplicationManager::~ReplicationManager() {

}

void ReplicationManager::processReceivedPacket(PacketData::packet_t packet, bool isLeader) {
    // TODO
    
    _sem.wait();

    _sem.notify();
}

bool ReplicationManager::newReplication(PacketData::packet_t packet, uint64_t& id) {
    _sem.wait();

    bool success = false;
    ReplicationData toReplicate = {
        packet,
        static_cast<uint64_t>(time(0)),
        ReplicationState::SEND,
        std::vector<bool>(_ids.size(), 0),
        0
    };

    toReplicate.sentTo[_id] = true;

    auto res = _messages.insert({(uint64_t) toReplicate.packet.timestamp, toReplicate});
    if (!res.second) {
        // Exists
    } else {
        id = res.first->second.packet.timestamp;
        success = true;
    }

    _sem.notify();

    return success;

}

void ReplicationManager::receivedConfirm(unsigned short otherID, uint64_t packetID) {
    auto it = _messages.find(packetID);

    if (it != _messages.end()) { // Found
        assert(it->second.sentTo.size() == _ids.size());
        if (it->second.state == ReplicationState::SENT) {
            it->second.sentTo[otherID] = true;

            if (_allMarked(it->second)) {
                it->second.state = ReplicationState::COMMIT;
            }
        }

    } else {
        // Not found
    }
}

bool ReplicationManager::getNextToSend(ReplicationData& res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::SEND) {
            res = it->second;
            hasNext = true;
            it++;
            break;
        }
    }

    return hasNext;
}

void ReplicationManager::updateSend(ReplicationData& data, unsigned short sentTo, bool success) {
    assert(data.sentTo.size() == _ids.size());

    data.sentTo[sentTo] = success;

    if (_allMarked(data)) {
        data.state = ReplicationState::SENT;
        std::fill(data.sentTo.begin(), data.sentTo.end(), false);
    }

}

void ReplicationManager::receivedToReplicate(PacketData::packet_t packet) {
    ReplicationData toReplicate = {
        packet,
        static_cast<uint64_t>(time(0)),
        ReplicationState::CONFIRM,
        std::vector<bool>(),
        0
    };

    auto res = _messages.insert({(uint64_t) toReplicate.packet.timestamp, toReplicate});
    if (!res.second) {
        if (res.first->second.state != ReplicationState::CONFIRM) {
            if (res.first->second.state == ReplicationState::COMMITTED) {
                res.first->second.state = ReplicationState::CONFIRMDUP;
            } else {
                res.first->second.state = ReplicationState::CONFIRM;
            }
            res.first->second.receivedAt = toReplicate.receivedAt;
        }
    }

}

bool ReplicationManager::getNextToConfirm(ReplicationData& res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::CONFIRM || it->second.state == ReplicationState::CONFIRMDUP) {
            res = it->second;
            hasNext = true;
            it++;
            break;
        }
    }

    return hasNext;

}

void ReplicationManager::updateConfirm(ReplicationData& data, bool success) {
    if (success) {
        if (data.state == ReplicationState::CONFIRMDUP) {
            data.state = ReplicationState::COMMITTED;

        } else {
            data.state = ReplicationState::COMMIT;

        }
    }
}

bool ReplicationManager::getNextToCommit(ReplicationData& res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::COMMIT) {
            it->second.state = ReplicationState::COMMITTED;
            res = it->second;
            hasNext = true;
            it++;
            break;
        }
    }

    return hasNext;

}

void ReplicationManager::checkTimeouts() {
    // TODO
}

bool ReplicationManager::_allMarked(ReplicationData& data) {
    std::vector<bool> resVec(data.sentTo.size());
    std::transform(
        data.sentTo.begin(), data.sentTo.end(), 
        _dead.begin(), 
        resVec.begin(), 
        std::logical_or<bool>()
    );

    return std::all_of(resVec.begin(), resVec.end(), [](bool v) { return v; });

}
