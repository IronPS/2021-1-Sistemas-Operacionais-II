
#pragma once

#include <string>

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

namespace ServerData {
    typedef struct s_serverinfo {
        std::string address;
        std::string port;
    } server_info_t;
};

namespace PacketData {
    const size_t PT_PAYLOAD_SIZE = 129;
    const size_t PT_EXTRA_SIZE = 21;

    typedef enum {
        LOGIN=4762,
        RECONNECT,
        WAIT,
        MESSAGE,
        FOLLOW,
        CLOSE,
        SUCCESS,
        ERROR,
        HEARTBEAT,
        ELECTION,
        ANSWER,
        COORDINATOR,
        NOTHING
    } packet_type;

    typedef struct __attribute__((packed))s_packet {
        packet_type type;
        uint16_t seqn; // Not used
        uint16_t length; // Not used
        uint64_t timestamp;
        char payload[PT_PAYLOAD_SIZE];
        char extra[PT_EXTRA_SIZE];
    } packet_t ;
};
