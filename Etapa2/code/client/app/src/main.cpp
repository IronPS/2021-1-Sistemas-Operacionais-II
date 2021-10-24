
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>

#include <parser.hpp>
#include <Stoppable.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <PacketTypes.hpp>
#include <CommandExecutor.hpp>
#include <Semaphore.hpp>

static sig_atomic_t closed = false;
static sig_atomic_t is_over = false;
static sig_atomic_t server_lost = false;
static sig_atomic_t print_user = true;
static unsigned int timeout = 10;
static Semaphore _sem(1);

bool messageExists(uint64_t timestamp) {
    static const size_t vsize = 200;
    static std::vector<uint64_t> timestamps(vsize);
    static size_t nextIdx = 0;

    if(std::find(timestamps.begin(), timestamps.end(), timestamp) != timestamps.end()) { // Already delivered
        return true;
    } else { // Prepare for delivery
        nextIdx = (nextIdx + 1) % vsize;
        timestamps[nextIdx] = timestamp;

        return false;
    }
}

void printUser(std::string username) {
    static Semaphore sem(1);

    sem.wait();
        if (print_user) {
            std::cout << username << "> " << std::flush;
            print_user = false;
        }
    sem.notify();
}

bool login(std::string user, ClientConnectionManager& cm) {
    bool timedOut = false;
    time_t timer;
    time_t t_now;
    bool accepted = false;

    time(&timer);
    while (signaling::_continue && !cm.openConnection(true, false, false)) {
        time(&t_now);
        if (difftime(t_now, timer) > timeout) {
            timedOut = true;
        }
        if (timedOut) break;
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

    if (timedOut) {
        std::cout << "Login connection timed-out" << std::endl;
        return accepted;
    }

    if (!signaling::_continue) return false;

    PacketData::packet_t packet;
    packet.type = PacketData::PacketType::NOTHING;
    PacketData::packet_t loginPacket = PacketBuilder::login(user, cm.getListenerPort());

    time(&timer);
    
    ssize_t bytes = -1;
    while (signaling::_continue && ((bytes = cm.dataSend(loginPacket)) == 0 || bytes == -1)) {
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

    if (!signaling::_continue) return false;

    time(&timer);
    while (signaling::_continue && ((bytes = cm.dataReceive(packet)) == 0 || bytes == -1)) {
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

    if (bytes > 0 && packet.type == PacketData::PacketType::SUCCESS) {
        accepted = true;

    }

    std::cout << packet.payload << std::endl;

    return accepted;
}

bool reconnect(std::string user, ClientConnectionManager& cm) {
    bool accepted = false;
    bool timedOut = false;
    time_t timer;
    time_t t_now;
    PacketData::packet_t reconPacket = PacketBuilder::login(user, cm.getListenerPort());
    reconPacket.type = PacketData::PacketType::RECONNECT;

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
    packet.type = PacketData::PacketType::NOTHING;

    ssize_t bytes = -1;
    
    // Waiting packet reception
    time(&timer);
    while(signaling::_continue && ((bytes = ClientConnectionManager::dataReceive(serverSFD, packet)) == -1 || bytes == -1)) {
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

    if (!signaling::_continue) {
        cm.dataSend(PacketBuilder::close());
        return false;
    }

    // Leader sent new info
    cm.closeConnection();
    ClientConnectionManager::closeConnection(serverSFD);

    // Connecting to Leader
    cm.setAddress(std::string(packet.extra));
    cm.setPort(std::string(packet.payload));

    std::cout << "Preparing reconnection..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (cm.openConnection(true, false)) {  
        time(&timer);
        while (signaling::_continue && ((bytes = cm.dataSend(reconPacket)) == 0 || bytes == -1)) {
            time(&t_now);
            if (difftime(t_now, timer) > timeout) {
                timedOut = true;
            }
            if (timedOut) break;
        }

        if (timedOut) {
            std::cout << "Reconnection timed-out" << std::endl;
            cm.dataSend(PacketBuilder::close());
            return accepted;
        }

        time(&timer);
        while (signaling::_continue && ((bytes = cm.dataReceive(packet)) == 0 || bytes == -1)) {
            time(&t_now);
            if (difftime(t_now, timer) > timeout) {
                timedOut = true;
            }
            if (timedOut) break;
        }

        if (timedOut) {
            std::cout << "Reconnection timed-out" << std::endl;
            cm.dataSend(PacketBuilder::close());
            return accepted;
        }

        if (!signaling::_continue) {
            cm.dataSend(PacketBuilder::close());
            return accepted;
        }

        if ((bytes > 0) && (bytes != -1) && packet.type == PacketData::PacketType::SUCCESS) {
            accepted = true;
        }

        std::cout << packet.payload << std::endl;
    
    }

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
                print_user = true;
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
    
    std::string command;
    printUser(user);

    do {
        if (std::cin) {
            std::cin.clear();
            if (getline_async(command)) {
                recognized = ce.execute(command);

                if (!recognized) {
                    _sem.wait();
                    std::cout << "Command '" << command << "' could not be recognized. Available commands: "
                              << "SEND, FOLLOW, CLOSE"
                              << std::endl;
                    _sem.notify();
                }

                if(!is_over && signaling::_continue) { // Conditional to avoid creating a new prompt when user wants to close
                    printUser(user);
                }
            
            }

        } else {
            ce.execute("close");
            is_over = true;
            closed = true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        packet.type = PacketData::PacketType::NOTHING;
        cm.dataReceive(packet);

        if (packet.type == PacketData::PacketType::NOTHING) {
            time_t t_now;
            time(&t_now);
            if (difftime(t_now, lastHeartbeat) > hbTimeout) {
                is_over = true;
                server_lost =  true;

                cm.closeConnection();

                _sem.wait();
                    std::cout << "Server Lost" << std::endl;
                    print_user = true;
                _sem.notify();

                return;
            }

            continue;
        }

        if (signaling::_continue) {
            time(&lastHeartbeat); // Every message counts as a heartbeat

            if (packet.type == PacketData::PacketType::HEARTBEAT) {

            } else {
                _sem.wait();
                    std::cout << '\n';  // Get out of user prompt
                _sem.notify();

                if (packet.type == PacketData::PacketType::CLOSE) {
                    _sem.wait();
                        std::cout   << "\e[1;31m"   // Color RED for Server
                                    << "SERVER: " 
                                    << "\e[0m"  // Restore normal color
                                    << "Closed by the server" 
                                    << std::endl;
                    _sem.notify();
                    is_over = true;
                    closed = true;
                    // No need to recreate user prompt here
                } else if (packet.type == PacketData::PacketType::MESSAGE) {
                    if (messageExists(packet.timestamp)) continue;

                    _sem.wait();
                        std::cout   << "\e[1;36m"   // Color BLUE (or at least is should be) for normal Creators
                                    << "@" << packet.extra << ": "  // Creator identification
                                    << "\e[0m"  // Restore normal color
                                    << packet.payload << std::endl;
                        
                        print_user = true;
                    _sem.notify();
                    printUser(user);

                } else {
                    _sem.wait();
                        std::cout << packet.payload << std::endl;
                        
                        print_user = true;
                    _sem.notify();
                    printUser(user);
                }
            
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

    ClientConnectionManager cm(results);

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
