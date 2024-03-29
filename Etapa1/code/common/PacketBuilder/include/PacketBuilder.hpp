
#pragma once

#include <PacketTypes.hpp>

#include <string>
#include <ctime>
#include <cstring>

class PacketBuilder {
 public:
    PacketBuilder() = delete;
    ~PacketBuilder() = delete;

    static PacketData::packet_t login(std::string username);
    static PacketData::packet_t message(std::string message, std::string sender = "");
    /*
     * If followWho is specified with less than 4 chars or more than 20, packet.type := ERROR
     */
    static PacketData::packet_t follow(std::string followWho);
    static PacketData::packet_t close();
    static PacketData::packet_t success(std::string reason);
    static PacketData::packet_t error(std::string reason);

 private:
    static const uint _min_username_len = 4;
};
