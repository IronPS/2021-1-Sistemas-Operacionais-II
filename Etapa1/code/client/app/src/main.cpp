
#include <parser.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>
#include <CommandExecutor.hpp>

bool login(ClientConnectionManager cm, std::string user) {
    PacketData::packet_t loginPacket = PacketBuilder::login(user);

    int b = cm.dataSend(loginPacket);
    std::cout << "Sent " << b << " bytes" << std::endl;

    // TODO: logic to wait confirmation from server
    bool accepted = true;

    return accepted;
}

void handleUserCommands(ClientConnectionManager cm, std::string user) {
    std::string input;
    bool is_over = false;
    bool recognized;
    CommandExecutor ce(cm, user);

    do {
        std::cout << user << "> ";

        getline(std::cin, input);
        is_over = std::cin.eof();

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

    // TODO: Add login logic
    ClientConnectionManager cm(results);

    bool logged = login(cm, user);

    if (logged) {
        // TODO: create listener thread (mocking the server for now)

        // Accept and process user commands
        handleUserCommands(cm, user);
    }

    // TODO: call cleanUp

}