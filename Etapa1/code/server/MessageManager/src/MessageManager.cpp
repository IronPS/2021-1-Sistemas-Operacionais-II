
#include<MessageManager.hpp>

MessageManager::MessageManager() : _sem(1) {

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

    // std::cout << "Now there are " << _pendingMessages[creator.name()].size() << " messages received by the server." << std::endl;
}

PacketData::packet_t MessageManager::getPacket(std::string toUser) {
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    ProducerConsumerBuffer* buffer = nullptr;

    _sem.wait();
    if (_pendingMessages.count(toUser)) {
        buffer = _pendingMessages[toUser];
    }
    _sem.notify();

    if (buffer == nullptr) return packet;

    std::chrono::milliseconds timeout(500);
    std::future<message_t> future = std::async(&ProducerConsumerBuffer::dequeue, buffer); 

    message_t message;
    message.packet.type = PacketData::packet_type::NOTHING;
    if (future.wait_for(timeout) == std::future_status::ready) {
        message = future.get();
    }

    return message.packet;
}
