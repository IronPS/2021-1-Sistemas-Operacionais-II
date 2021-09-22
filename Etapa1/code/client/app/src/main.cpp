
#include <parser.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <CommandExecutor.hpp>

bool login(ClientConnectionManager cm, std::string user) {
    PacketData::packet_t loginPacket = PacketBuilder::login(user);

    cm.dataSend(loginPacket);

    // TODO: logic to wait confirmation from server
    bool accepted = true;

    return accepted;
}

void handleUserInput(ClientConnectionManager cm, std::string user) {
    std::string input;
    bool is_over = false;
    bool recognized;
    CommandExecutor ce(cm, user);

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

        }

    } while(!is_over);

}

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    std::string user = results["user"].as<std::string>();

    std::cout << "Attempting to login as '" << user << "' ... " << std::endl;

    ClientConnectionManager cm(results);

    // TODO: Move login to a better place, perhaps inside cm constructor?
    bool logged = login(cm, user);

    if (logged) {
        // TODO: create listener thread (mocking the server for now)

        // Inform user all is ok and they can start using system
        std::cout << "\nSuccess! Now logged in as " << user << std::endl;

        // Accept and process user input
        handleUserInput(cm, user);
    }

    // TODO: call cleanUp

}