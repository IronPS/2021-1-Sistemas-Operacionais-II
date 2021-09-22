#pragma once

#include <string>

#include <ClientConnectionManager.hpp>
#include <PacketTypes.hpp>
#include <PacketBuilder.hpp>

class CommandExecutor {
    public:
        CommandExecutor(std::string user);

        bool execute(std::string fullCommand);
    private:
        std::string _user;
        void _sendMessage(std::string user, std::string message);
        void _requestFollow(std::string followee);
        std::string _parseCommand(std::string rawInput);
        std::string _parseArgument(std::string rawInput);
};