
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

namespace PacketData {
    const size_t PT_PAYLOAD_SIZE = 129;
    const size_t PT_EXTRA_SIZE = 21;

    typedef enum {
        LOGIN=4762,
        RECONNECT,
        MESSAGE,
        FOLLOW,
        CLOSE,
        SUCCESS,
        ERROR,
        HEARTBEAT,
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
