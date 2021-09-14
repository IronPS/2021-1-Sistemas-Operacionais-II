
#include <parser.hpp>
#include <ServerConnectionManager.hpp>
#include <PersistenceManager.hpp>
#include <signal.h>
#include <Stoppable.hpp>

int main(int argc, char* argv[]) {
    Stoppable stop;

    auto results = parse(argc, argv);

    PersistenceManager pm("db.db");

    ServerConnectionManager cm(results);
    

    int csfd = -1;

    while (signaling::_continue) {
        csfd = cm.getConnection();

        if (csfd != -1) {
            const std::string response = "Hello World";
            auto bytes_sent = send(csfd, response.c_str(), response.length()+1, 0);

            std::cout << "Sent " << bytes_sent << " bytes" << std::endl;

            ServerConnectionManager::closeConnection(csfd);
            // TODO create thread for Client treatment
        }

    }
    

}