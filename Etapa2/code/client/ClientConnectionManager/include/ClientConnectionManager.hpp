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

    bool openConnection(bool exitOnFail = false, bool nonBlocking = true, bool ignoreErrorMessage = false);
    void closeConnection();
    static void closeConnection(int& sfd);

    ssize_t dataSend(PacketData::packet_t packet);
    ssize_t dataReceive(PacketData::packet_t& packet);

    int getSFD() { return _clientSFD; }

    static ssize_t dataSend(int sfd, PacketData::packet_t packet);
    static ssize_t dataReceive(int sfd, PacketData::packet_t& packet);
    int getConnection();

    std::string getListenerPort() { return _listenerPort; }
    
    bool setAddress(std::string address);
    bool setPort(std::string port);


 private:
    std::string _user;
    std::string _server;
    std::string _port;
    std::string _listenerPort;
    int _clientSFD = -1;
    int _backlog = 1;

    bool _openConnection(bool exitOnFail = true, bool nonBlocking = false, bool ignoreErrorMessage = false);
    void _print_packet(PacketData::packet_t packet);

    void _createListener();
    int _listenerSFD = -1;

};
