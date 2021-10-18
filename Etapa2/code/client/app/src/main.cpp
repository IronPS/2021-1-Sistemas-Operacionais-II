
#include <thread>
#include <future>
#include <chrono>

#include <parser.hpp>
#include <Stoppable.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <PacketTypes.hpp>
#include <CommandExecutor.hpp>

static bool is_over = false;

bool login(std::string user, ClientConnectionManager& cm) {
    bool timedOut = false;
    unsigned int timeout = 20;
    time_t timer;
    PacketData::packet_t loginPacket = PacketBuilder::login(user);
    PacketData::packet_t packet;
    packet.type = PacketData::packet_type::NOTHING;

    cm.dataSend(loginPacket);

    auto bytes_received = cm.dataReceive(packet);

    bool accepted = false;
    if (bytes_received > 0 && packet.type == PacketData::packet_type::SUCCESS) {
        accepted = true;

    } else if (packet.type == PacketData::packet_type::RECONNECT) {
        time(&timer);

        while (signaling::_continue) {
            cm.closeConnection();

            time_t t_now;
            time(&t_now);
            if (difftime(t_now, timer) > timeout) {
                timedOut = true;
                break;
            }
            
            cm.setAddress(std::string(packet.extra));
            cm.setPort(std::string(packet.payload));

            cm.openConnection(true, false);
            cm.dataSend(loginPacket);

            packet.type = PacketData::packet_type::NOTHING;
            auto bytes_received = cm.dataReceive(packet);

            if (packet.type == PacketData::packet_type::SUCCESS) {
                accepted = true;
                break;

            } else if (packet.type != PacketData::packet_type::WAIT) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                time(&timer);
            
            } else if (packet.type != PacketData::packet_type::RECONNECT) {
                break;

            }

        }
    }

    if (!timedOut) std::cout << packet.payload << std::endl;
    else std::cout << "Reconnection timed-out" << std::endl;

    return accepted;
}

std::string readInput() {
    std::string input;
    std::getline(std::cin, input);
    return input;    
}

void handleUserInput(std::string user, ClientConnectionManager& cm) {
    bool recognized;
    CommandExecutor ce(user, cm);
    std::chrono::seconds timeout(3);
    std::future<std::string> future = std::async(readInput); 

    std::string command;
    std::cout << user << "> ";
    do {
        if (std::cin) {
            std::cin.clear();
            if (future.wait_for(timeout) == std::future_status::ready) {
                command = future.get();

                recognized = ce.execute(command);

                if (!recognized && std::cin) {
                    std::cout << "Command not recognized\n";
                }
            
                if(!is_over && signaling::_continue) { // Conditional to avoid creating a new prompt when user wants to close
                    std::cout << user << "> ";  
                    future = std::async(readInput);
                }
            
            }

        } else {
            ce.execute("close");
            is_over = true;
        }

    } while(!is_over && signaling::_continue);

    if (!signaling::_continue) {
        ce.execute("close");
    }

    exit(0); // Quick and dirty way
}

void handleServerInput(std::string user, ClientConnectionManager& cm) {
    PacketData::packet_t packet;

    unsigned int hbTimeout = 20;
    time_t lastHeartbeat;
    time(&lastHeartbeat);
    while (!is_over && signaling::_continue) {
        packet.type = PacketData::packet_type::NOTHING;
        cm.dataReceive(packet);

        time_t t_now;
        time(&t_now);
        if (difftime(t_now, lastHeartbeat) > hbTimeout) {
            std::cout << "Server Lost" << std::endl;
            cm.closeConnection();
            // TODO reconnect
            signaling::_continue = false;
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

int main(int argc, char* argv[]) {
    Stoppable stop;
    
    auto results = parse(argc, argv);

    std::string user = results["user"].as<std::string>();

    std::cout << "Attempting to login as '" << user << "' ... " << std::endl;

    ClientConnectionManager cm(results);

    bool logged = login(user, cm);

    std::vector<std::thread> threads;

    if (logged) {
        std::thread t = std::thread(handleUserInput, user, std::ref(cm));
        threads.push_back(std::move(t));

        t = std::thread(handleServerInput, user, std::ref(cm));
        threads.push_back(std::move(t));

    } else {
        std::cout << "Failed to login" << std::endl;
    }

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }

    exit(0);

}
