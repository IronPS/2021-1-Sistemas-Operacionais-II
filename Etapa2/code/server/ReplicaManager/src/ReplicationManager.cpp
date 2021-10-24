
#include <ReplicationManager.hpp>

ReplicationManager::ReplicationManager(const cxxopts::ParseResult& input)
: _id(input["id"].as<unsigned short>()), _sem(1)
{
    _ids = input["ids"].as<std::vector<unsigned short>>();
    _dead = std::vector<bool>(_ids.size(), false);
}

ReplicationManager::~ReplicationManager() {

}

ReplicationManager::ReplicationState ReplicationManager::getMessageState(uint64_t id) {
    auto it = _messages.find(id);

    if (it != _messages.end()) {
        return it->second.state;
    }

    return ReplicationState::CANCELED;
}

void ReplicationManager::processReceivedPacket(PacketData::packet_t packet, unsigned short otherID) {
    _sem.wait();
    switch (packet.rtype) {
        case PacketData::ReplicationType::R_NEWMESSAGE:            
        case PacketData::ReplicationType::R_DELMESSAGE:            
        case PacketData::ReplicationType::R_SESSION:       
        case PacketData::ReplicationType::R_FOLLOWER:       
            receivedToReplicate(packet);
            break;
            
        case PacketData::ReplicationType::R_CONFIRM:
            receivedConfirm(otherID, packet.timestamp);
            break;
            
        case PacketData::ReplicationType::R_NONE:
            break;

    }
    _sem.notify();
}

bool ReplicationManager::newReplication(PacketData::packet_t packet, uint64_t& id) {
    _sem.wait();

    bool success = false;
    ReplicationData toReplicate = {
        packet,
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
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
                it->second.state = ReplicationState::COMMITTED;
            }
        }

    } else {
        // Not found
    }
}

void ReplicationManager::checkSent() {
    for (auto it = _messages.begin(); it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::SENT) {
            it->second.state = ReplicationState::COMMITTED;
        }
    }
}

bool ReplicationManager::getNextToSend(ReplicationData** res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::SEND) {
            *res = &it->second;
            (void)res; // Stop warning "set but not used"
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
        data.sentTo[_id] = true;
    }

}

void ReplicationManager::receivedToReplicate(PacketData::packet_t packet) {
    if (packet.seqn < _epoch) return;

    ReplicationData toReplicate = {
        packet,
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
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

bool ReplicationManager::getNextToConfirm(ReplicationData** res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::CONFIRM || it->second.state == ReplicationState::CONFIRMDUP) {
            *res = &it->second;
            (void)res; // Stop warning "set but not used"
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

bool ReplicationManager::getNextToCommit(ReplicationData** res, bool firstIt) {
    static std::map<uint64_t,ReplicationData>::iterator it = _messages.begin();
    bool hasNext = false;

    if (firstIt) {
        it = _messages.begin();
    }

    for (; it != _messages.end(); it++) {
        if (it->second.state == ReplicationState::COMMIT) {
            it->second.state = ReplicationState::COMMITTED;
            *res = &it->second;
            (void)res; // Stop warning "set but not used"
            hasNext = true;
            it++;
            break;
        }
    }

    return hasNext;

}

void ReplicationManager::checkTimeouts() {
    uint64_t t_now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    for (auto it = _messages.begin(); it != _messages.end(); /* Nothing */) {
        bool remove = false;

        switch (it->second.state) {
            case SEND:
            case SENT:
                if ((t_now - it->second.receivedAt) > _packetTimeToLive) {
                    it->second.numFailures += 1;
                    if (it->second.numFailures > _maxFailures) {
                        it->second.state = ReplicationState::CANCELED;

                    } else {
                        it->second.receivedAt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        it->second.state = ReplicationState::SEND;

                    }

                }

                break;
            case CONFIRM:
                if (difftime(t_now, it->second.receivedAt) > _packetTimeToLive) {
                    it->second.numFailures += 1;
                    if (it->second.numFailures > _maxFailures) {
                        it->second.state = ReplicationState::CANCELED;

                    } else {
                        it->second.receivedAt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                    }                
                }

                break;
            case CONFIRMDUP:
                if (difftime(t_now, it->second.receivedAt) > _packetTimeToLive) {
                    it->second.numFailures += 1;
                    if (it->second.numFailures > _maxFailures) {
                        it->second.state = ReplicationState::CANCELED;

                    } else {
                        it->second.receivedAt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                    }                
                }

                break;
            case COMMIT:
                // Do nothing

                break;
            case COMMITTED:
                if ((t_now - it->second.receivedAt) > _packetTimeToLive) {
                    remove = true;
                }

                break;
            case CANCELED:
                remove = true;
                break;
        }

        if (remove) {
            _messages.erase(it++);
        } else {
            it++;
        }
    }
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

bool ReplicationManager::_allDead() {
    std::vector<bool> resVec = _dead;
    resVec[_id] = true;

    return std::all_of(resVec.begin(), resVec.end(), [](bool v) { return v; });
}

void ReplicationManager::clear() {
    for (auto it = _messages.begin(); it != _messages.end(); /* Nothing */) {
        if (it->second.packet.seqn < _epoch && !(it->second.state == ReplicationState::COMMIT)) {
            _messages.erase(it++);
        } else {
            it++;
        }
    }

}
