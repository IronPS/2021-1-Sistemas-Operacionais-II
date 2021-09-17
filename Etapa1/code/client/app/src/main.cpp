
#include <parser.hpp>
#include <ClientConnectionManager.hpp>

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    ClientConnectionManager cm(results);
    PacketData::packet_t packet;
    auto bytes = cm.dataReceive(packet);
    std::cout << "Received " << bytes
    << "Payload:\nBEGIN\n" << packet.payload << "\nEND\n";
}