#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

#include <User.hpp>
#include <PacketBuilder.hpp>
#include <Semaphore.hpp>
#include <ProducerConsumerBuffer.hpp>
#include <ReplicaManager.hpp>

class MessageManager {
 public:
    MessageManager(ReplicaManager& rm);
    ~MessageManager();

    void processIncomingMessage(User& creator, const std::string text, const uint64_t timestamp);
    PacketData::packet_t getPacket(std::string toUser);

 private:
    std::map<std::string, ProducerConsumerBuffer*> _pendingMessages;
    Semaphore _sem;

 private:
    ReplicaManager& _rm;

};