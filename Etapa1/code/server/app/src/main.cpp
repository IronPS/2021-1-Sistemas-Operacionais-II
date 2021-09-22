
#include <thread>
#include <parser.hpp>
#include <ServerConnectionManager.hpp>
#include <PersistenceManager.hpp>
#include <SessionMonitor.hpp>
#include <Stoppable.hpp>
#include <ClientFunctions.hpp>

int main(int argc, char* argv[]) {
    Stoppable stop;

    auto results = parse(argc, argv);

    PersistenceManager pm("db.db");
    ServerConnectionManager cm(results);
    SessionMonitor sm(pm);
    
    std::vector<std::thread> threads;

    int csfd = -1;

    while (signaling::_continue) {
        csfd = cm.getConnection();

        if (csfd != -1) {
            std::thread t = std::thread(ClientFunctions::newConnection, csfd, std::ref(sm));
            threads.push_back(std::move(t));
        }

    }

    for (std::thread& th : threads) {
        if (th.joinable())
            th.join();
    }
    

}