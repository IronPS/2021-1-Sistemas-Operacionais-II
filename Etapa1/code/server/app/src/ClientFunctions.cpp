
#include <ClientFunctions.hpp>

void ClientFunctions::newConnection(int csfd, SessionMonitor& sm, PersistenceManager& pm) {
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
        SessionController* session = sm.createSession(packet.extra, csfd, connected);

        if (connected) {
            strcpy(packet.payload, (std::string("Welcome to Incredible Tvitter!\nSuccessfuly logged in as: ") + username).c_str());
            ServerConnectionManager::dataSend(csfd, packet);

            std::cout << "User " << username << " successfuly logged in\n";

            bool is_over = false;
            while(signaling::_continue && !is_over) {
                session->deliverMessages();
                
                auto bytes_received = ServerConnectionManager::dataReceive(csfd, packet);
                if (bytes_received > 0) {
                    if (packet.type == PacketData::packet_type::CLOSE) {
                        is_over = true;
                    } else if (packet.type == PacketData::packet_type::FOLLOW) { // TODO
                        std::cout << "Received FOLLOW from user '" 
                        << username << "' to user '"
                        << packet.extra 
                        << "'" << std::endl;

                        bool success = true;
                        if (username != packet.extra) {
                            sm.getControl();
                            
                            SessionController* followee = sm.getSession(packet.extra);
                            if (followee != nullptr) {
                                followee->addFollower(username);
                            } else { // No session already open
                                User user = pm.loadUser(packet.extra, false);

                                if (user.name() == packet.extra) {
                                    user.addFollower(username);
                                    pm.saveUser(user);
                                } else {
                                    success = false;
                                }
                            }
                            
                            sm.freeControl();

                            if (!success) {
                                ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Follow failed: username does not exist"));
                            }
                        }


                    } else if (packet.type == PacketData::packet_type::MESSAGE) { // TODO
                        std::string messageContent = packet.payload;

                        std::cout << "Received MESSAGE from user '"
                        << username << "' with content '"
                        << messageContent
                        << "'" << std::endl;

                        session->sendMessage(messageContent);

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
