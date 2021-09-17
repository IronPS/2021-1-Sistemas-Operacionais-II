
#include <PacketBuilder.hpp>

using namespace PacketData;

packet_t PacketBuilder::login(std::string username) {
    packet_t packet;

    packet.type = LOGIN;
    packet.seqn = 0;                // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    packet.payload[0] = '\0';
    strcpy(packet.extra, username.c_str());

    return packet;
}

packet_t PacketBuilder::message(std::string message, std::string sender) {
    packet_t packet;

    if (message.length()+1 >= PT_PAYLOAD_SIZE) {
        message = message.substr(0, PT_PAYLOAD_SIZE-1);
    }

    packet.type = MESSAGE;
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
    packet.seqn = 0;                        // TODO Ignored
    packet.length = 0;
    packet.timestamp = static_cast<uint64_t>(time(0));
    strcpy(packet.payload, reason.c_str());
    packet.extra[0] = '\0';

    return packet;
}
