#include <parser.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <PacketTypes.hpp>
#include <CommandExecutor.hpp>

bool login(std::string user) {
    PacketData::packet_t loginPacket = PacketBuilder::login(user);

    ClientConnectionManager::dataSend(loginPacket);

    auto bytes_received = ClientConnectionManager::dataReceive(loginPacket);

    bool accepted = false;
    if (bytes_received > 0 && loginPacket.type != PacketData::packet_type::ERROR) {
        accepted = true;

    }

    std::cout << loginPacket.payload << std::endl;

    return accepted;
}

void handleUserInput(std::string user) {
    std::string input;
    bool is_over = false;
    bool recognized;
    CommandExecutor ce(user);
    PacketData::packet_t packet;

    do {
        // Display prompt to get user input
        std::cout << user << "> ";

        getline(std::cin, input);
        is_over = std::cin.eof();  // Capture Ctrl+D

        if (!is_over) {

            recognized = ce.execute(input);

            if (!recognized) {
                std::cout << "There was an error in your command, please check it and try again" << std::endl;
            }

            // TODO remove this from here and add at packet reception treatment thread
            packet.type = PacketData::packet_type::NOTHING;
            ClientConnectionManager::dataReceive(packet);

            if (packet.type == PacketData::packet_type::CLOSE) {
                std::cout << "Closed by the server" << std::endl;
                is_over = true;
            }
        }

    } while(!is_over);

}

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    std::string user = results["user"].as<std::string>();

    std::cout << "Attempting to login as '" << user << "' ... " << std::endl;

    ClientConnectionManager cm(results);

    // TODO: Move login to a better place, perhaps inside cm constructor?
    bool logged = login(user);

    if (logged) {
        // TODO: create listener thread (mocking the server for now)

        // Inform user all is ok and they can start using system
        std::cout << "\nSuccess! Now logged in as " << user << std::endl;

        // Accept and process user input
        handleUserInput(user);
    } else {
        std::cout << "Failed to login" << std::endl;
    }

    // TODO: call cleanUp

}
