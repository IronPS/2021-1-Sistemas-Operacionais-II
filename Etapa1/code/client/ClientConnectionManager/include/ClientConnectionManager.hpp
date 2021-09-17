#pragma once

#include <string>

#include <cxxopts/cxxopts.hpp>
#include <PacketTypes.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

class ClientConnectionManager {
 public:
    /*
     * Establishes a connection
     * 
     * Exits with error if it fails
    */
    ClientConnectionManager(const cxxopts::ParseResult& input);
    ~ClientConnectionManager();

    ssize_t dataSend(PacketData::packet_t packet);
    ssize_t dataReceive(PacketData::packet_t& packet);

 private:
    std::string _user;
    std::string _server;
    std::string _port;

    int _socketDesc = -1;

    void _openConnection();
};
