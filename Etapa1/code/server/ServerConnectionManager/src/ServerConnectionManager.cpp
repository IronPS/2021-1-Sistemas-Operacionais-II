
#include <ServerConnectionManager.hpp>

ServerConnectionManager::ServerConnectionManager(const cxxopts::ParseResult& input) {
    port = std::to_string(input["port"].as<unsigned short>());
    
    _bindListeningSocket();
    _listen();
}

ServerConnectionManager::~ServerConnectionManager() {
    if (listeningSocketDesc != -1) {
        close(listeningSocketDesc);
    }
}

void ServerConnectionManager::_bindListeningSocket() {
    struct addrinfo hints, *addrs = NULL;
    int err = 0;
    
    // Get address
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    err = getaddrinfo (
        NULL,
        port.c_str(),
        &hints,
        &addrs
    );

    if (err != 0) {
        std::cerr << "Error at getaddrinfo: "
        << gai_strerror(err)
        << std::endl;
        exit(1);
    }

    for (struct addrinfo* addrptr = addrs; addrptr != NULL; addrptr = addrptr->ai_next) {
        listeningSocketDesc = socket(addrptr->ai_family, addrptr->ai_socktype, addrptr->ai_protocol);

        if (listeningSocketDesc == -1) {
            err = errno;
            break;
        }

        if (bind(listeningSocketDesc, addrptr->ai_addr, addrptr->ai_addrlen) == 0)
            break;

        err = errno;

        close(listeningSocketDesc);
    }

    if (listeningSocketDesc == -1) {
        std::cerr << "Connection error: "
        << strerror(err)
        << std::endl;
        
        freeaddrinfo(addrs);

        exit(1);
    }

    if (err != 0) {
        std::cerr << "Error binding socket "
        << strerror(err)
        << std::endl;

        close(listeningSocketDesc);
        freeaddrinfo(addrs);
        
        exit(1);
    }

    freeaddrinfo(addrs);
}

void ServerConnectionManager::_listen() {
    int err = listen(listeningSocketDesc, backlog);
    if (err == -1) {
        std::cerr << "Error listening to socket "
        << strerror(errno)
        << std::endl;
        close(listeningSocketDesc);

        exit(1);
    }

    sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // TODO fork
    do {
        // Socket File Descriptor
        int newSFD = accept(listeningSocketDesc, (sockaddr*) &client_addr, &client_addr_size);
        if (newSFD == -1) {
            std::cerr << "Failed to accept incoming connection" << std::endl;
            continue;
        }

        // TODO everything down here is a placeholder
        const std::string response = "Hello World";
        auto bytes_sent = send(newSFD, response.c_str(), response.length()+1, 0);

        std::cout << "Sent " << bytes_sent << " bytes" << std::endl;
        close(newSFD);
        // TODO fork
        // if (pidorsomething == somevalue) break;

    } while (false); // TODO Change to true in the future

}

// TODO
void ServerConnectionManager::dataSend() {

}

// TODO
void ServerConnectionManager::dataReceive() {

}