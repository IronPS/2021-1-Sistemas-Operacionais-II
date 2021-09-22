#include<CommandExecutor.hpp>
#include<iostream>

CommandExecutor::CommandExecutor(const ClientConnectionManager& cm, std::string user) : _cm(cm), _user(user) {

}

void CommandExecutor::_sendMessage(ClientConnectionManager cm, std::string user, std::string message) {
    PacketData::packet_t messagePacket = PacketBuilder::message(message, user);

    cm.dataSend(messagePacket);
}

void CommandExecutor::_requestFollow(ClientConnectionManager cm, std::string followee) {
    PacketData::packet_t followPacket = PacketBuilder::follow(followee);

    cm.dataSend(followPacket);
}

std::string CommandExecutor::_parseCommand(std::string rawInput) {
    std::size_t spacePos = rawInput.find(" ");
    return rawInput.substr(0, spacePos);
}

std::string CommandExecutor::_parseArgument(std::string rawInput) {
    std::size_t spacePos = rawInput.find(" ");
    return rawInput.substr(spacePos+1);
}


bool CommandExecutor::execute(std::string fullCommand) {

    if (fullCommand.find(" ") == std::string::npos) {   // does not contain "COMMAND <argument>" structure
        return false;
    }

    std::string command = _parseCommand(fullCommand);
    std::string argument = _parseArgument(fullCommand);

    if (command.size() == 0 || argument.size() == 0) {  // does not contain "COMMAND <argument>" structure
        return false;
    }

    bool recognized = false;

    if (command == "SEND") {    // should we make commands case-insensitive?
        _sendMessage(_cm, _user, fullCommand.substr(5));
        recognized = true;
    }
    else if (command == "FOLLOW") {
        _requestFollow(_cm, fullCommand.substr(7));
        recognized = true;
    }

    return recognized;
}