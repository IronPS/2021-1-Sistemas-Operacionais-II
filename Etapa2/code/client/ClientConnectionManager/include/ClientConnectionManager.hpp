#pragma once

#include <string>

#include <cxxopts/cxxopts.hpp>
#include <PacketTypes.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

class ClientConnectionManager {
 public:
    /*
     * Establishes a connection
     * 
     * Exits with error if it fails
    */
    ClientConnectionManager(const cxxopts::ParseResult& input);
    ClientConnectionManager(std::string serverAddress, unsigned short serverPort);
    ~ClientConnectionManager();

    bool openConnection(bool exitOnFail = false, bool nonBlocking = true);
    static ssize_t dataSend(PacketData::packet_t packet);
    static ssize_t dataReceive(PacketData::packet_t& packet);

 private:
    std::string _user;
    std::string _server;
    std::string _port;

    static int _socketDesc;

    bool _openConnection(bool exitOnFail = true, bool nonBlocking = false);
    void _print_packet(PacketData::packet_t packet);
};
