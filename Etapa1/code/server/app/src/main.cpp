
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
            PacketData::packet_t packet;
            strcpy(packet.payload, "Hello World 2!");
            auto bytes = ServerConnectionManager::dataSend(csfd, packet);

            std::cout << "Sent " << bytes << " bytes" << std::endl;

            ServerConnectionManager::closeConnection(csfd);
            // TODO create thread for Client treatment
        }

    }
    

}