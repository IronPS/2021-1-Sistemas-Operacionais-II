
#include <thread>
#include <chrono>

#include <parser.hpp>
#include <ServerConnectionManager.hpp>
#include <ReplicaManager.hpp>
#include <PersistenceManager.hpp>
#include <SessionMonitor.hpp>
#include <ClientFunctions.hpp>
#include <Stoppable.hpp>

int main(int argc, char* argv[]) {
    Stoppable stop;

    auto results = parse(argc, argv);

    ServerConnectionManager cm(results);

    PersistenceManager pm(std::to_string(results["port"].as<unsigned short>()) + "-db.db");
    SessionMonitor sm(pm);
    ReplicaManager rm(results, pm, sm);
    
    std::vector<std::thread> threads;
    int csfd = -1;

    std::thread t = std::thread(&ReplicaManager::start, &rm);
    threads.push_back(std::move(t));

    while (signaling::_continue) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        csfd = cm.getConnection();

        if (csfd != -1) {
            t = std::thread(ClientFunctions::newConnection, csfd, std::ref(sm), std::ref(pm), std::ref(rm));
            threads.push_back(std::move(t));
        }

    }

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }
    

}