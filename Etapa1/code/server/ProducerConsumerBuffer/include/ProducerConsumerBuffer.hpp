
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>

#include <Semaphore.hpp>
#include <PacketBuilder.hpp>

typedef struct s_message {
    uint64_t timestamp;
    PacketData::packet_t packet;
    bool delivered;
} message_t;  // Structure mostly as suggested by professor, with 'creator' added

class ProducerConsumerBuffer {
 public:
    ProducerConsumerBuffer();
    ~ProducerConsumerBuffer();

    void enqueue(message_t message);
    message_t dequeue();

    size_t size() { return _bufferSize; }

 private:
    std::vector<message_t> _buffer;

    size_t _bufferSize = 0;

    static const size_t _maxBufferSize = 100;

    Semaphore _capacitySem;
    size_t _numProduced = 0;
    Semaphore _producedSem;

    size_t _writeIndex = 0;
    Semaphore _writeSem;
    size_t _readIndex = 0;
    Semaphore _readSem;

    

};
