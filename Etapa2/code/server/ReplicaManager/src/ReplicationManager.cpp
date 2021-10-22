
#include <ReplicationManager.hpp>

ReplicationManager::ReplicationManager(const cxxopts::ParseResult& input)
: _id(input["id"].as<unsigned short>())
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    _dead = std::vector<bool>(_ids.size(), false);
}

ReplicationManager::~ReplicationManager() {

}

bool ReplicationManager::newReplication(PacketData::packet_t packet, uint64_t& id) {
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
        return false;
    } else {
        id = res.first->second.packet.timestamp;
    }

    return true;

}

void ReplicationManager::receivedConfirm(unsigned short otherID, uint64_t packetID) {
    // TODO
    // Check if exists
    // If SENT vector[otherID] = true
    //  If std::all_of(vec.begin(), vec.end(), [](bool v) { return v; }) then COMMIT
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

void ReplicationManager::updateSend(ReplicationData& data, unsigned short sentTo) {
    assert(data.sentTo.size() == _ids.size());

    data.sentTo[sentTo] = true;
    std::vector<bool> resVec(data.sentTo.size());
    std::transform(
        data.sentTo.begin(), data.sentTo.end(), 
        _dead.begin(), 
        resVec.begin(), 
        std::logical_or<bool>()
    );

    if (std::all_of(resVec.begin(), resVec.end(), [](bool v) { return v; })) {
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
            res.first->second.state = ReplicationState::CONFIRM;
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
        if (it->second.state == ReplicationState::CONFIRM) {
            it->second.state = ReplicationState::COMMIT;
            res = it->second;
            hasNext = true;
            it++;
            break;
        }
    }

    return hasNext;

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
