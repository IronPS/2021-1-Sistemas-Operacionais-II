#pragma once

#include <string>

#include <cxxopts/cxxopts.hpp>

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

    // TODO
    void dataSend();
    // TODO
    void dataReceive();

 private:
    std::string user;
    std::string server;
    std::string port;

    int socketDesc = -1;

    void _openConnection();
};
