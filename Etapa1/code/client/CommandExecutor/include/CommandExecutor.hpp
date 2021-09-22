#pragma once

#include <string>

#include <ClientConnectionManager.hpp>
#include <PacketTypes.hpp>
#include <PacketBuilder.hpp>

class CommandExecutor {
    public:
        CommandExecutor(const ClientConnectionManager& cm, std::string user);

        bool execute(std::string fullCommand);
    private:
        ClientConnectionManager _cm;
        std::string _user;
        void _sendMessage(ClientConnectionManager cm, std::string user, std::string message);
        void _requestFollow(ClientConnectionManager cm, std::string followee);
        std::string _parseCommand(std::string rawInput);
        std::string _parseArgument(std::string rawInput);
};