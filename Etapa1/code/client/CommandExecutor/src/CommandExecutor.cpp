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

bool CommandExecutor::execute(std::string command) {

    bool recognized = true;

    if (command.rfind("SEND ", 0) == 0) {
        _sendMessage(_cm, _user, command.substr(5));
    }
    else if (command.rfind("FOLLOW ", 0) == 0) {
        _requestFollow(_cm, command.substr(7));
    }
    else {
        recognized = false;
    }

    return recognized;
}