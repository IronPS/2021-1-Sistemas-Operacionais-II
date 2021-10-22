
#include <thread>
#include <future>
#include <chrono>

#include <parser.hpp>
#include <Stoppable.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <PacketTypes.hpp>
#include <CommandExecutor.hpp>

static bool closed = false;
static bool is_over = false;
static bool server_lost = false;

bool login(std::string user, ClientConnectionManager& cm) {
    bool timedOut = false;
    unsigned int timeout = 10;
    time_t timer;
    time_t t_now;
    bool accepted = false;

    time(&timer);
    while (!cm.openConnection(false, false, true) && signaling::_continue) {
        time(&t_now);
        if (difftime(t_now, timer) > timeout) {
            timedOut = true;
        }
        if (timedOut) break;

    }

    if (timedOut) {
        std::cout << "Login connection timed-out" << std::endl;
        return accepted;
    }

    PacketData::packet_t loginPacket = PacketBuilder::login(user, cm.getListenerPort());
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    cm.dataSend(loginPacket);

    ssize_t bytes_received = -1;
    time(&timer);
    while((bytes_received = cm.dataReceive(packet)) == -1 || bytes_received == -1) {
        time(&t_now);
        if (difftime(t_now, timer) > timeout) {
            timedOut = true;
        }
        if (timedOut) break;
    }

    if (timedOut) {
        std::cout << "Login attempt timed-out" << std::endl;
        cm.dataSend(PacketBuilder::close());
        return accepted;
    }

    if (bytes_received > 0 && packet.type == PacketData::packet_type::SUCCESS) {
        accepted = true;

    }

    std::cout << packet.payload << std::endl;

    return accepted;
}

bool reconnect(std::string user, ClientConnectionManager& cm) {
    bool accepted = false;
    bool timedOut = false;
    unsigned int timeout = 10;
    time_t timer;
    time_t t_now;
    PacketData::packet_t reconPacket = PacketBuilder::login(user, cm.getListenerPort());
    reconPacket.type = PacketData::packet_type::RECONNECT;

    ServerData::server_info_t sinfo;

    std::cout << "Waiting for leader" << std::endl;

    // Waiting leader to connect on listening port
    int serverSFD = -1;
    time(&timer);
    do {
        serverSFD = cm.getConnection();
        
        time(&t_now);
        if (difftime(t_now, timer) > timeout) {
            timedOut = true;
        }
        if (timedOut) break;

    } while (serverSFD == -1 && signaling::_continue);

    if (timedOut) {
        std::cout << "Reconnection timed-out" << std::endl;
        return accepted;
    }

    // Leader connected
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    ssize_t bytes_received = -1;
    
    // Waiting packet reception
    time(&timer);
    while((bytes_received = ClientConnectionManager::dataReceive(serverSFD, packet)) == -1 || bytes_received == -1) {
        time(&t_now);
        if (difftime(t_now, timer) > timeout) {
            timedOut = true;
        }
        if (timedOut) break;
    }

    if (timedOut) {
        std::cout << "Reconnection timed-out" << std::endl;
        ClientConnectionManager::dataSend(serverSFD, PacketBuilder::close());
        return accepted;
    }

    // Leader sent new info
    cm.closeConnection();
    ClientConnectionManager::closeConnection(serverSFD);

    // Connecting to Leader
    cm.setAddress(std::string(packet.extra));
    cm.setPort(std::string(packet.payload));

    cm.openConnection(true, false);    
    
    cm.dataSend(reconPacket);

    if ((bytes_received > 0) && (bytes_received != -1) && packet.type == PacketData::packet_type::SUCCESS) {
        accepted = true;
    }

    std::cout << packet.payload << std::endl;

    return accepted;
}

bool getline_async(std::string& str, char delim = '\n') {
    static std::string lineSoFar;

    char inChar;
    int charsRead = 0;
    bool lineRead = false;
    str = "";

    std::ios_base::sync_with_stdio(false);
    do {
        charsRead = std::cin.readsome(&inChar, 1);
        if (charsRead == 1) {
            // if the delimiter is read then return the string so far
            if (inChar == delim) {
                str = lineSoFar;
                lineSoFar = "";
                lineRead = true;
            } else {  // otherwise add it to the string so far
                lineSoFar.append(1, inChar);
            }
        }
    } while (charsRead != 0 && !lineRead && !is_over && signaling::_continue);

    return lineRead;
}

void handleUserInput(std::string user, ClientConnectionManager& cm) {
    bool recognized;
    CommandExecutor ce(user, cm);
    std::chrono::seconds timeout(3);

    std::string command;
    std::cout << user << "> ";
    do {
        if (std::cin) {
            std::cin.clear();
            if (getline_async(command)) {
                recognized = ce.execute(command);

                if (!recognized && std::cin) {
                    std::cout << "Command not recognized\n";
                }
            
                if(!is_over && signaling::_continue) { // Conditional to avoid creating a new prompt when user wants to close
                    std::cout << user << "> "; 
                }
            
            }

        } else {
            ce.execute("close");
            is_over = true;
            closed = true;
        }

    } while(!is_over && signaling::_continue);

    if (!signaling::_continue) {
        ce.execute("close");
        closed = true;
    }

}

void handleServerInput(std::string user, ClientConnectionManager& cm) {
    PacketData::packet_t packet;

    unsigned int hbTimeout = 10;
    time_t lastHeartbeat;
    time(&lastHeartbeat);
    while (!is_over && signaling::_continue) {
        packet.type = PacketData::packet_type::NOTHING;
        cm.dataReceive(packet);

        time_t t_now;
        time(&t_now);
        if (difftime(t_now, lastHeartbeat) > hbTimeout) {
            cm.closeConnection();
            std::cout << "Server Lost" << std::endl;
            is_over = true;
            server_lost =  true;
            return;
        }

        if (packet.type == PacketData::packet_type::NOTHING) continue;

        if (signaling::_continue) {
            std::cout << '\n';  // Get out of user prompt

            if (packet.type == PacketData::packet_type::HEARTBEAT) {
                time(&lastHeartbeat);

            } else if (packet.type == PacketData::packet_type::CLOSE) {
                std::cout   << "\e[1;31m"   // Color RED for Server
                            << "SERVER: " 
                            << "\e[0m"  // Restore normal color
                            << "Closed by the server" 
                            << std::endl;
                is_over = true;
                closed = true;
                // No need to recreate user prompt here
            } else if (packet.type == PacketData::packet_type::MESSAGE) {
                std::cout   << "\e[1;36m"   // Color BLUE (or at least is should be) for normal Creators
                            << "@" << packet.extra << ": "  // Creator identification
                            << "\e[0m"  // Restore normal color
                            << packet.payload << "\n" << std::flush;
                std::cout << user << "> " << std::flush;    // Recreate user prompt
            } else {
                std::cout << packet.payload << std::endl;
                std::cout << user << "> " << std::flush;    // Recreate user prompt
            }

        }

    }
}

void launchHandlers(std::string user, ClientConnectionManager& cm) {
    std::vector<std::thread> threads;

    std::thread t = std::thread(handleUserInput, user, std::ref(cm));
    threads.push_back(std::move(t));

    t = std::thread(handleServerInput, user, std::ref(cm));
    threads.push_back(std::move(t));

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }
}

int main(int argc, char* argv[]) {
    Stoppable stop;
    
    auto results = parse(argc, argv);

    std::string user = results["user"].as<std::string>();

    std::cout << "Attempting to login as '" << user << "' ... " << std::endl;

    ClientConnectionManager cm(results["server"].as<std::string>(), results["port"].as<unsigned short>());

    bool logged = login(user, cm);

    if (logged) {
        launchHandlers(user, cm);

    } else {
        std::cout << "Failed to login" << std::endl;
    }

    if (logged) {
        while (!closed && is_over && server_lost) {
            std::this_thread::sleep_for(std::chrono::seconds(2));

            is_over = false;
            server_lost = false;

            logged = reconnect(user, cm);

            if (logged) {
                launchHandlers(user, cm);
            }


        }

    }

    exit(0);

}
