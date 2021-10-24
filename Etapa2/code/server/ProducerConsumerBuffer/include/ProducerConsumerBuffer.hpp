
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>

#include <Semaphore.hpp>
#include <Flag.hpp>
#include <PacketBuilder.hpp>

typedef struct s_message {
    uint64_t timestamp;
    PacketData::packet_t packet;
    bool delivered;
} message_t; 

class ProducerConsumerBuffer {
 public:
    ProducerConsumerBuffer();
    ~ProducerConsumerBuffer();

    void enqueue(message_t message);
    message_t dequeue();

    message_t peek();
    void markDelivered(uint64_t messageID);

    size_t size() { return _numProduced; }

 private:
    std::vector<message_t> _buffer;

    static const size_t _maxBufferSize = 100;

    size_t _capacity = _maxBufferSize;
    Semaphore _capacitySem;

    size_t _numProduced = 0;
    Semaphore _producedSem;

    size_t _writeIndex = 0;
    Semaphore _writeSem;
    size_t _readIndex = 0;
    Semaphore _readSem;

    Flag _modifyingFlag;    

};
