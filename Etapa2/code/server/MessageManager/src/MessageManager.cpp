
#include<MessageManager.hpp>

MessageManager::MessageManager() : _sem(1){

}

MessageManager::~MessageManager() {
    for (auto it : _pendingMessages) {
        delete it.second;
    }
}

void MessageManager::processIncomingMessage(User& creator, const std::string text, const uint64_t timestamp) {
    _sem.wait();
    for (auto follower : creator.followers()) {
        if (!_pendingMessages.count(follower)) {
            _pendingMessages[follower] = new ProducerConsumerBuffer();
        } 
    }
    _sem.notify();

    message_t incoming = {
        timestamp: timestamp,
        packet: PacketBuilder::message(text, creator.name()),
        delivered: false
    };

    std::vector<std::thread> threads;
    _sem.wait();
    for (auto follower : creator.followers()) {
        std::thread t = std::thread(&ProducerConsumerBuffer::enqueue, _pendingMessages[follower], incoming);
        threads.push_back(std::move(t));
    }
    _sem.notify();

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

}

PacketData::packet_t MessageManager::getPacket(std::string toUser) {
    PacketData::packet_t packet;
    packet.type = PacketData::PacketType::NOTHING;

    ProducerConsumerBuffer* buffer = nullptr;

    _sem.wait();
    if (_pendingMessages.count(toUser)) {
        buffer = _pendingMessages[toUser];
    }
    _sem.notify();

    if (buffer == nullptr) return packet;

    packet = buffer->dequeue().packet;

    return packet;
}
