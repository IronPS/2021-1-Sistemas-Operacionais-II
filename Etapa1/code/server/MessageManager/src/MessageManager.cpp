#include<MessageManager.hpp>
#include<iostream>

MessageManager::MessageManager() {
    _pendingMessages = {};
}

void MessageManager::processIncomingMessage(std::string creator, std::string text, uint64_t timestamp) {
    message incoming = {
        id: timestamp,          // used timestamp just to test, as I did not know the best way to generate UUIDs in C++
        timestamp: timestamp,   // using timestamp from client for now, we can change later perhaps
        creator: creator,
        text: text,
        length: text.size(),
        usersPendingDelivery: 0
    };

    _pendingMessages.insert({ incoming.id, incoming});

    std::cout << "Now there are " << _pendingMessages.size() << " messages received by the server." << std::endl;
}