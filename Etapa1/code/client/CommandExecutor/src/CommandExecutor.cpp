#include<CommandExecutor.hpp>
#include<iostream>

CommandExecutor::CommandExecutor(std::string user) : _user(user) {

}

void CommandExecutor::_sendMessage(std::string user, std::string message) {
    PacketData::packet_t messagePacket = PacketBuilder::message(message, user);

    ClientConnectionManager::dataSend(messagePacket);
}

void CommandExecutor::_requestFollow(std::string followee) {
    PacketData::packet_t followPacket = PacketBuilder::follow(followee);

    ClientConnectionManager::dataSend(followPacket);
}

void CommandExecutor::_sendClose() {
    PacketData::packet_t closePacket = PacketBuilder::close();

    ClientConnectionManager::dataSend(closePacket);

    signaling::_continue = false;

}

std::string CommandExecutor::_parseCommand(std::string rawInput) {
    std::size_t spacePos = rawInput.find(' ');
    return rawInput.substr(0, spacePos);
}

std::string CommandExecutor::_parseArgument(std::string rawInput) {
    std::size_t spacePos = rawInput.find(" ");
    return rawInput.substr(spacePos+1);
}


bool CommandExecutor::execute(std::string fullCommand) {

    bool foundSpace = true;
    if (fullCommand.find(' ') == std::string::npos) {   // does not contain "COMMAND <argument>" structure
        foundSpace = false;
    }

    std::string command = _parseCommand(fullCommand);
    std::transform(command.begin(), command.end(), command.begin(),
        [](unsigned char c){ return std::tolower(c); });

    std::string argument;
    
    if (foundSpace) {
        argument = _parseArgument(fullCommand);
    }

    if (command.size() == 0) return false;
    if (foundSpace && argument.size() == 0) return false; // does not contain "COMMAND <argument>" structure

    bool recognized = false;

    if (command == "send") {    // we allow for case-insensitive commands
        if (!foundSpace) return false;

        _sendMessage(_user, fullCommand.substr(5));
        recognized = true;

    } else if (command == "follow") {
        if (!foundSpace) return false;

        _requestFollow(fullCommand.substr(7));
        recognized = true;

    } else if (command == "close") {
        _sendClose();
        recognized = true;

    } else {
        std::cout << "Command '" << command << "' could not be recognized. Available commands: "
                << "SEND, FOLLOW, CLOSE"
                << std::endl;
    }

    return recognized;
}