
#include <parser.hpp>
#include <ClientConnectionManager.hpp>
#include <PacketBuilder.hpp>

bool login(ClientConnectionManager cm, std::string user) {
    PacketData::packet_t loginPacket = PacketBuilder::login(user);

    int b = cm.dataSend(loginPacket);
    std::cout << "Sent " << b << " bytes" << std::endl;

    // TODO: logic to wait confirmation from server
    bool accepted = true;

    return accepted;
}

void sendMessage(ClientConnectionManager cm, std::string user, std::string message) {
    PacketData::packet_t messagePacket = PacketBuilder::message(message, user);

    int b = cm.dataSend(messagePacket);
    std::cout << "Sent " << b << " bytes" << std::endl;
}

void requestFollow(ClientConnectionManager cm, std::string followee) {
    PacketData::packet_t followPacket = PacketBuilder::follow(followee);

    int b = cm.dataSend(followPacket);
    std::cout << "Sent " << b << " bytes" << std::endl;
}

void handle_user_commands(ClientConnectionManager cm, std::string user) {
    // TODO: move this user-prompt logic to a separate module
    std::string input;
    bool is_over = false;

    do {
        std::cout << user << "> ";

        getline(std::cin, input);
        is_over = std::cin.eof();

        if (!is_over) {
            // TODO: parse input into command and argument

            // This is here just for debugging, for now
            // std::cout << "@" << user << " says: " << input << std::endl;

            if (input.rfind("SEND ", 0) == 0) {
                std::cout << "\nIt was a SEND message!" << std::endl;
                sendMessage(cm, user, input.substr(5));
            }
            else if (input.rfind("FOLLOW ", 0) == 0) {
                std::cout << "\nIt was a FOLLOW message!" << std::endl;
                requestFollow(cm, input.substr(7));
            }
            else {
                std::cout << "\nI did not recognize that..." << std::endl;
            }

        }

    } while(!is_over);

}

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    std::string user = results["user"].as<std::string>();

    std::cout << "Attempting to login as '" << user << "' ... " << std::endl;

    ClientConnectionManager cm(results);

    bool logged = login(cm, user);

    if (logged) {
        // TODO: create listener thread (mocking the server for now)

        // Accept and process user commands
        handle_user_commands(cm, user);
    }

}