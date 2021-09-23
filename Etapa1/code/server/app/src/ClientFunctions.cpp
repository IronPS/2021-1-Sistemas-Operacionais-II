
#include <ClientFunctions.hpp>

void ClientFunctions::newConnection(int csfd, SessionMonitor& sm) {
    PacketData::packet_t packet;

    // Set timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; // 500 milliseconds
    setsockopt(csfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    // TODO: all checkings for invalid packet
    ServerConnectionManager::dataReceive(csfd, packet);
    std::string username = std::string(packet.extra);
    
    bool connected = false;
    if (packet.type == PacketData::LOGIN) {
        SessionController* session = sm.createSession(packet.extra, connected);

        if (connected) {
            strcpy(packet.payload, (std::string("Welcome to Incredible Tvitter!\nSuccessfuly logged in as: ") + username).c_str());
            ServerConnectionManager::dataSend(csfd, packet);

            std::cout << "User " << username << " successfuly logged in\n";

            bool is_over = false;
            while(signaling::_continue && !is_over) {
                if (sm.newData()) {
                    // TODO get new packet
                    ServerConnectionManager::dataSend(csfd, packet);
                } 
                
                auto bytes_received = ServerConnectionManager::dataReceive(csfd, packet);
                if (bytes_received > 0) {
                    if (packet.type == PacketData::packet_type::CLOSE) {
                        is_over = true;
                    } else if (packet.type == PacketData::packet_type::FOLLOW) { // TODO
                        std::cout << "Received FOLLOW from user '" 
                        << username << "' to user '"
                        << packet.extra 
                        << "'" << std::endl;
                    } else if (packet.type == PacketData::packet_type::MESSAGE) { // TODO
                        std::cout << "Received MESSAGE from user '"
                        << username << "' with content '"
                        << packet.payload 
                        << "'" << std::endl;
                    }
                }

            }

        } else {
            auto bytes = ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect"));

            std::cout << "User " << packet.extra << " could not connect." << std::endl
            << "\tSent " << bytes << " bytes" << std::endl;
        }

        sm.closeSession(username, csfd);
        std::cout << "Closed user " << username << " session\n" << std::endl;

    } else {
        // TODO invalid command received error
        ServerConnectionManager::closeConnection(csfd);
    }
    
}