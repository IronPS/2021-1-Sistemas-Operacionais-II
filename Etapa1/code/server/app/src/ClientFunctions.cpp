
#include <ClientFunctions.hpp>

void ClientFunctions::newConnection(int csfd, SessionMonitor& sm) {
    PacketData::packet_t packet;

    // TODO: all checkings for invalid packet
    ServerConnectionManager::dataReceive(csfd, packet);
    std::string username = std::string(packet.extra);
    
    bool connected = false;
    if (packet.type == PacketData::LOGIN) {
        SessionController* session = sm.createSession(packet.extra, connected);

        if (connected) {
            strcpy(packet.payload, "Hello World 2!");
            ServerConnectionManager::dataSend(csfd, packet);

            while(signaling::_continue) {
                if (sm.newData()) {
                    // TODO get new packet
                    ServerConnectionManager::dataSend(csfd, packet);
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));

                }
            }

        } else {
            auto bytes = ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect"));

            std::cout << "User " << packet.extra << " could not connect." << std::endl
            << "\tSent " << bytes << " bytes" << std::endl;
        }

        sm.closeSession(username, csfd);
        std::cout << "Closed session\n" << std::endl;

    } else {
        // TODO invalid command received error
        ServerConnectionManager::closeConnection(csfd);
    }
    
}
