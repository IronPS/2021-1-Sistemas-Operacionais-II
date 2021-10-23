
#include <ProducerConsumerBuffer.hpp>

ProducerConsumerBuffer::ProducerConsumerBuffer() 
: _capacitySem(1), _producedSem(1), _writeSem(1), _readSem(1)
{
    message_t message;
    message.delivered = true;
    _buffer.resize(_maxBufferSize, message);

}

ProducerConsumerBuffer::~ProducerConsumerBuffer() {

}

void ProducerConsumerBuffer::enqueue(message_t message) {
    _capacitySem.wait();
        if (_capacity > 0) {
            _capacity -= 1;
        _capacitySem.notify();

        size_t index = 0;
        _writeSem.wait();
            index = _writeIndex;
            _writeIndex = (_writeIndex + 1) % _maxBufferSize;
        _writeSem.notify();

        if (!_buffer[index].delivered) {
            std::cerr << "Error at ProducerConsumerBuffer enqueue: overwriting non-delivered message!" << std::endl;
        }

        _buffer[index] = message;

        _producedSem.wait();
            _numProduced += 1;
        _producedSem.notify();
    } else {
        _capacitySem.notify();
    }

}

message_t ProducerConsumerBuffer::dequeue() {
    message_t message;
    message.packet.type = PacketData::PacketType::NOTHING;

    int produced = 0;
    _producedSem.wait();

    produced = _numProduced;

    if (produced > 0) {
        _numProduced -= 1;
        _producedSem.notify();

        size_t index = 0;
        _readSem.wait();
            index = _readIndex;
            _readIndex = (_readIndex + 1) % _maxBufferSize;
        _readSem.notify();

        if (_buffer[index].delivered) {
            std::cerr << "Error at ProducerConsumerBuffer dequeue: delivering already delivered message!" << std::endl;
        }

        message = _buffer[index];
        _buffer[index].delivered = true;

        _capacitySem.wait();
            _capacity += 1;
        _capacitySem.notify();

    } else {
        _producedSem.notify();

    }

    return message;
}
