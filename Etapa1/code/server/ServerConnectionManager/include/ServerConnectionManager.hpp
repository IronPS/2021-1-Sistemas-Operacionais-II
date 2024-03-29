#pragma once


#include <string>

#include <cxxopts/cxxopts.hpp>
#include <PacketTypes.hpp>

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

    /*
     * Returns a socket file descriptor
     */
    int getConnection();

    static void closeConnection(int sfd);

    static ssize_t dataSend(int sfd, PacketData::packet_t packet);
    static ssize_t dataReceive(int sfd, PacketData::packet_t& packet);

 private:
    std::string _port;
    const unsigned int _backlog = 8; // Allows 8 connections on the incoming queue
    int _socketFileDesc = -1;

    void _bindListeningSocket();
};
