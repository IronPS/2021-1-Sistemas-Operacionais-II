#pragma once

#include <string>

#include <cxxopts/cxxopts.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

class ServerConnectionManager {
 public:
    /*
     * Creates a listening port
     * 
     * Exits with error if it fails
    */
    ServerConnectionManager(const cxxopts::ParseResult& input);
    ~ServerConnectionManager();

    // TODO
    void dataSend();
    // TODO
    void dataReceive();

 private:
    std::string port;
    const unsigned int backlog = 8; // Allows 8 connections on the incoming queue
    int listeningSocketDesc = -1;

    void _bindListeningSocket();
    void _listen();
};
