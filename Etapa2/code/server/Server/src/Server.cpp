
#include <Server.hpp>

Server::Server(const cxxopts::ParseResult& input, PersistenceManager& pm, SessionMonitor& sm)
: ReplicaManager(input, pm, sm), _pm(pm), _sm(sm)
{

}

Server::~Server() {

}

void Server::newClient(int csfd) {
    PacketData::packet_t packet;

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; // 500 milliseconds
    setsockopt(csfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    // TODO: all checkings for invalid packet
    ServerConnectionManager::dataReceive(csfd, packet);
    std::string username = std::string(packet.extra);

    if (!isLeader()) {
        std::cout << "User '" << username << "' tried to log-in\n";
        ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: trying to connect to replica"));
        ServerConnectionManager::closeConnection(csfd);
        
        return;
    }
    
    bool connected = false;
    if (packet.type == PacketData::LOGIN || packet.type == PacketData::RECONNECT) {
        if (waitCommit(PacketBuilder::replicateSession(username, "LOGIN," + std::to_string(csfd) + "," + packet.payload))) {

            SessionController* session = _sm.createSession(packet.extra, packet.payload, csfd, connected);

            if (connected) {
                if (packet.type == PacketData::LOGIN) {
                    ServerConnectionManager::dataSend(csfd, PacketBuilder::success(std::string("Welcome to Incredible Tvitter!\nSuccessfuly logged in as: ") + username));
                    _handleUser(username, csfd, session);
                } else if (packet.type == PacketData::RECONNECT) {
                    ServerConnectionManager::dataSend(csfd, PacketBuilder::success(std::string("Reconnection successful. Logged-in as ") + username));
                    _handleUser(username, csfd, session);
                }

            } else {
                auto bytes = ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect"));

                std::cout << "User " << packet.extra << " could not connect." << std::endl
                << "\tSent " << bytes << " bytes" << std::endl;
            }

            // Must not close user session on exit
            if (csfd != -1) {
                if (session != nullptr) {
                    waitCommit(PacketBuilder::replicateSession(username, "CLOSE," + std::to_string(csfd)));

                    _sm.closeSession(username, csfd, false);
                    std::cout << "Closed user " << username << " session socket without \"CLOSE\"\n" << std::endl;
                } else {
                    ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Error: Failed to connect -- Replication error"));
                    ServerConnectionManager::closeConnection(csfd);
                }
            }

        }
    } else {
        // TODO invalid command received error
        ServerConnectionManager::closeConnection(csfd);
    }
}

void Server::_handleUser(std::string username, int& csfd, SessionController* session) {
    PacketData::packet_t packet;
    packet.type = PacketData::PacketType::NOTHING;

    // Time in seconds for heartbeat repetition
    unsigned int hbTime = 2;

    std::cout << "User " << username << " successfuly logged in\n";

    ServerConnectionManager::dataSend(csfd, PacketBuilder::heartbeat(0));
    time_t lastHeartbeat;
    time(&lastHeartbeat);
    bool is_over = false;
    while(signaling::_continue && !is_over) {
        session->deliverMessages(*this);
                
        auto bytes_received = ServerConnectionManager::dataReceive(csfd, packet);
        if (bytes_received > 0) {
            if (packet.type == PacketData::PacketType::CLOSE) {
                is_over = true;
                waitCommit(PacketBuilder::replicateSession(username, "CLOSE," + std::to_string(csfd)));

                _sm.closeSession(username, csfd);
                csfd = -1;
                std::cout << "Closed user " << username << " session\n" << std::endl;

            } else if (packet.type == PacketData::PacketType::FOLLOW) {
                std::cout << "Received FOLLOW from user '" 
                << username << "' to user '"
                << packet.extra 
                << "'" << std::endl;

                bool success = true;
                if (username != packet.extra) {
                    _sm.getControl();
                        
                    if (waitCommit(PacketBuilder::replicateFollower(packet.extra, username))) {
                        SessionController* followee = _sm.getSession(packet.extra);
                        if (followee != nullptr) {
                            followee->addFollower(username);
                        } else { // No session open
                            User user = _pm.loadUser(packet.extra, false);

                            if (user.name() == packet.extra) {
                                user.addFollower(username);
                                _pm.saveUser(user);
                            } else {
                                success = false;
                            }
                        }
                    }
                            
                    _sm.freeControl();

                    if (!success) {
                        ServerConnectionManager::dataSend(csfd, PacketBuilder::error("Follow failed: username does not exist"));
                    } else {
                        std::cout << "User " << username << " now following " << packet.extra << std::endl;
                    }
                }


            } else if (packet.type == PacketData::PacketType::MESSAGE) {
                std::string messageContent = packet.payload;

                std::cout << "Received MESSAGE from user '"
                << username << "' with content '"
                << messageContent
                << "'" << std::endl;

                if (waitCommit(PacketBuilder::replicateMessage(username, messageContent))) {
                    session->sendMessage(messageContent);

                }

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

