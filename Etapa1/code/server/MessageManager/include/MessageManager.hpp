#pragma once

#include <string>
#include <map>

typedef struct __message {
    uint64_t id;
    uint64_t timestamp;
    std::string creator;
    std::string text;
    int length;
    int usersPendingDelivery;
} message;  // Structure mostly as suggested by professor, with 'creator' added

class MessageManager {
    public:
        MessageManager();

        void processIncomingMessage(std::string creator, std::string text, uint64_t timestamp);
    private:
        std::map<uint64_t, message> _pendingMessages;
};