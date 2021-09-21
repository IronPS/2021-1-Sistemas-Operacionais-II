
#include <ClientFunctions.hpp>

void ClientFunctions::newConnection(int csfd, SessionMonitor& sm) {
    PacketData::packet_t packet;

    // TODO: all checkings for invalid packet
    ServerConnectionManager::dataReceive(csfd, packet);
    std::string username = std::string(packet.extra);
    
    bool connected = false;
    if (packet.type == PacketData::LOGIN) {
        SessionController& session = sm.createSession(packet.extra, connected);

        if (connected) {
            while(signaling::_continue) {
                strcpy(packet.payload, "Hello World 2!");
                auto bytes = ServerConnectionManager::dataSend(csfd, packet);

                std::cout << "Sent " << bytes << " bytes" << std::endl;
            }
        } else {
            auto bytes = ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect"));

            std::cout << "Sent " << bytes << " bytes" << std::endl;
        }

        sm.closeSession(username, csfd);

    } else {
        // TODO invalid command received error
        ServerConnectionManager::closeConnection(csfd);
    }
    
}
