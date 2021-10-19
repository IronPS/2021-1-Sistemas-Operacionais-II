
#include <ClientFunctions.hpp>

void ClientFunctions::newConnection(int csfd, SessionMonitor& sm, PersistenceManager& pm, ReplicaManager& rm) {
    PacketData::packet_t packet;

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; // 500 milliseconds
    setsockopt(csfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    // TODO: all checkings for invalid packet
    ServerConnectionManager::dataReceive(csfd, packet);
    std::string username = std::string(packet.extra);
    
    bool connected = false;
    if (packet.type == PacketData::LOGIN || packet.type == PacketData::RECONNECT) {
        SessionController* session = sm.createSession(packet.extra, csfd, connected);

        if (connected) {
            if (packet.type == PacketData::LOGIN) {
                if (!rm.isLeader()) {
                    if(!reconnect(username, csfd, sm, rm)) return;

                } else {
                    ServerConnectionManager::dataSend(csfd, PacketBuilder::success(std::string("Welcome to Incredible Tvitter!\nSuccessfuly logged in as: ") + username));
                }
                
                handleUser(username, csfd, session, sm, pm);

            } else if (packet.type == PacketData::RECONNECT) {
                if (!rm.isLeader()) {
                    if(!reconnect(username, csfd, sm, rm)) return;
                }

                handleUser(username, csfd, session, sm, pm);
            }

        } else {
            auto bytes = ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect"));

            std::cout << "User " << packet.extra << " could not connect." << std::endl
            << "\tSent " << bytes << " bytes" << std::endl;
        }

        // Must not close user session on exit
        // sm.closeSession(username, csfd);
        // std::cout << "Closed user " << username << " session\n" << std::endl;

    } else {
        // TODO invalid command received error
        ServerConnectionManager::closeConnection(csfd);
    }
    
}

void ClientFunctions::handleUser(std::string username, int csfd, SessionController* session, SessionMonitor& sm, PersistenceManager& pm) {
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    // Time in seconds for heartbeat repetition
    unsigned int hbTime = 5;

    std::cout << "User " << username << " successfuly logged in\n";

    ServerConnectionManager::dataSend(csfd, PacketBuilder::heartbeat(0));
    time_t lastHeartbeat;
    time(&lastHeartbeat);
    bool is_over = false;
    while(signaling::_continue && !is_over) {
        session->deliverMessages();
                
        auto bytes_received = ServerConnectionManager::dataReceive(csfd, packet);
        if (bytes_received > 0) {
            if (packet.type == PacketData::packet_type::CLOSE) {
                is_over = true;
                sm.closeSession(username, csfd);
                std::cout << "Closed user " << username << " session\n" << std::endl;

            } else if (packet.type == PacketData::packet_type::FOLLOW) {
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
                    } else { // No session open
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


            } else if (packet.type == PacketData::packet_type::MESSAGE) {
                std::string messageContent = packet.payload;

                std::cout << "Received MESSAGE from user '"
                << username << "' with content '"
                << messageContent
                << "'" << std::endl;

                session->sendMessage(messageContent);

            }
        }

        time_t t_now;
        time(&t_now);
        if (difftime(t_now, lastHeartbeat) > hbTime) {
            ServerConnectionManager::dataSend(csfd, PacketBuilder::heartbeat(0));
            lastHeartbeat = t_now;
        }

    }

}

bool ClientFunctions::reconnect(std::string username, int csfd, SessionMonitor& sm, ReplicaManager& rm) {
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    std::cout << "User " << username << " tried to log in\n";
    do {
        if (rm.waitingElection()) {
            packet.type = PacketData::packet_type::WAIT;
            ServerConnectionManager::dataSend(csfd, packet);
        } else {
            ServerConnectionManager::dataSend(csfd, rm.getLeaderInfo());
            auto bytes_received = ServerConnectionManager::dataReceive(csfd, packet);
        }

    } while (packet.type == PacketData::RECONNECT && ( rm.waitingElection() || !rm.isLeader()));
    
    if (rm.isLeader()) {
        ServerConnectionManager::dataSend(csfd, PacketBuilder::success(std::string("Successfuly reconnected as: ") + username));
    
    } else {
        sm.closeSession(username, csfd);
        std::cout << "Closed user " << username << " session\n" << std::endl;
        
        return false;
    }

    return true;
}