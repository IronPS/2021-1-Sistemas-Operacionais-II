
#include <PacketBuilder.hpp>

using namespace PacketData;

packet_t PacketBuilder::login(std::string username, std::string listenerPort) {
    packet_t packet;

    packet.type = LOGIN;
    packet.rtype = R_NONE;
    packet.seqn = 0;                // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, listenerPort.c_str());
    strcpy(packet.extra, username.c_str());

    return packet;
}

packet_t PacketBuilder::message(std::string message, std::string sender) {
    packet_t packet;

    if (message.length()+1 >= PT_PAYLOAD_SIZE) {
        message = message.substr(0, PT_PAYLOAD_SIZE-1);
    }

    packet.type = MESSAGE;
    packet.rtype = R_NONE;
    packet.seqn = 0;            // TODO Ignored
    packet.length = message.length()+1;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, message.c_str());
    strcpy(packet.extra, sender.c_str());

    return packet;
}


packet_t PacketBuilder::follow(std::string followWho) {
    packet_t packet;

    bool error = false;

    if (followWho.length() < _min_username_len || followWho.length()+1 >= PT_EXTRA_SIZE) {
        error = true;
    }

    packet.payload[0] = '\0';
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.rtype = R_NONE;

    if (!error) {
        packet.type = FOLLOW;
        packet.timestamp = static_cast<uint64_t>(time(0));
        strcpy(packet.extra, followWho.c_str());
    } else {
        packet.type = ERROR;
        packet.timestamp = 0;
        packet.extra[0] = '\0';
    }

    return packet;
}

packet_t PacketBuilder::close() {
    packet_t packet;

    packet.type = CLOSE;
    packet.rtype = R_NONE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    packet.payload[0] = '\0';
    packet.extra[0] = '\0';

    return packet;
}

packet_t PacketBuilder::success(std::string reason) {
    packet_t packet;

    if (reason.length()+1 >= PT_PAYLOAD_SIZE) {
        reason = reason.substr(0, PT_PAYLOAD_SIZE-1);
    }

    packet.type = SUCCESS;
    packet.rtype = R_NONE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, reason.c_str());
    packet.extra[0] = '\0';

    return packet;
}

packet_t PacketBuilder::error(std::string reason) {
    packet_t packet;

    if (reason.length()+1 >= PT_PAYLOAD_SIZE) {
        reason = reason.substr(0, PT_PAYLOAD_SIZE-1);
    }

    packet.type = ERROR;
    packet.rtype = R_NONE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, reason.c_str());
    packet.extra[0] = '\0';

    return packet;
}

packet_t PacketBuilder::heartbeat(unsigned short id) {
    packet_t packet;

    packet.type = HEARTBEAT;
    packet.rtype = R_NONE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    packet.payload[0] = '\0';
    strcpy(packet.extra, std::to_string(id).c_str());

    return packet;
}

PacketData::packet_t PacketBuilder::leaderInfo(std::string address, unsigned short clientPort) {
    packet_t packet;

    packet.type = RECONNECT;
    packet.rtype = R_NONE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, std::to_string(clientPort).c_str());
    strcpy(packet.extra, address.c_str());

    return packet;
}

PacketData::packet_t PacketBuilder::serverSignal(unsigned short id, PacketData::packet_type type, uint16_t epoch) {
    packet_t packet;

    packet.type = type;
    packet.rtype = R_NONE;
    packet.seqn = epoch;
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    packet.payload[0] = '\0';
    strcpy(packet.extra, std::to_string(id).c_str());

    return packet;
}

packet_t PacketBuilder::replicateMessage(std::string userFrom, std::string userTo, std::string message) {
    packet_t packet;

    packet.type = REPLICATE;
    packet.rtype = R_MESSAGE;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcmp(packet.payload, message.c_str());
    strcmp(packet.extra, (userFrom + ',' + userTo).c_str());

    return packet;
}

packet_t PacketBuilder::replicateSession(std::string username, std::string command) {
    packet_t packet;

    packet.type = REPLICATE;
    packet.rtype = R_SESSION;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcmp(packet.payload, command.c_str()); // "LOGIN" or "CLOSE"
    strcmp(packet.extra, username.c_str());

    return packet;
}

packet_t PacketBuilder::replicateUser(std::string username, std::string command) {
    packet_t packet;

    packet.type = REPLICATE;
    packet.rtype = R_USER;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcmp(packet.payload, command.c_str()); // "CREATE" or "ADDFOLLOWER,follower_username"
    strcmp(packet.extra, username.c_str());

    return packet;
}

packet_t PacketBuilder::confirmReplication(uint64_t timestamp) {
    packet_t packet;

    packet.type = REPLICATE;
    packet.rtype = R_CONFIRM;
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = timestamp;
    packet.payload[0] = '\0';
    packet.extra[0] = '\0';

    return packet;
}
