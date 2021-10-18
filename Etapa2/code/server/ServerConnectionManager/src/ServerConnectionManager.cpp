
#include <ServerConnectionManager.hpp>

ServerConnectionManager::ServerConnectionManager(const cxxopts::ParseResult& input) {
    _port = std::to_string(input["port"].as<unsigned short>());
    _bindListeningSocket();
}

ServerConnectionManager::ServerConnectionManager(const unsigned short port) {
    _port = std::to_string(port);
    _bindListeningSocket();
}

ServerConnectionManager::~ServerConnectionManager() {
    if (_socketFileDesc != -1) {
        shutdown(_socketFileDesc, SHUT_RDWR);
    }
}

void ServerConnectionManager::closeConnection(int sfd) {
    if (sfd != -1) {
        shutdown(sfd, SHUT_RDWR);
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
        _port.c_str(),
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
        _socketFileDesc = socket(addrptr->ai_family, addrptr->ai_socktype, addrptr->ai_protocol);

        if (_socketFileDesc == -1) {
            err = errno;
            break;
        }

        if (bind(_socketFileDesc, addrptr->ai_addr, addrptr->ai_addrlen) == 0)
            break;

        err = errno;

        close(_socketFileDesc);
    }

    if (_socketFileDesc == -1) {
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

        close(_socketFileDesc);
        freeaddrinfo(addrs);
        
        exit(1);
    }

    freeaddrinfo(addrs);

    err = listen(_socketFileDesc, _backlog);
    if (err == -1) {
        std::cerr << "Error listening to socket "
        << strerror(errno)
        << std::endl;
        close(_socketFileDesc);

        exit(1);
    }

    fcntl(_socketFileDesc, F_SETFL, fcntl(_socketFileDesc, F_GETFL, 0) | O_NONBLOCK);
}

int ServerConnectionManager::getConnection() {
    sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // Socket File Descriptor
    int newSFD = accept4(_socketFileDesc, (sockaddr*) &client_addr, &client_addr_size, SOCK_NONBLOCK);

    return newSFD;

}

ssize_t ServerConnectionManager::dataSend(int sfd, PacketData::packet_t packet) {
    auto bytes_sent = send(sfd, (void*) &packet, sizeof(PacketData::packet_t), 0);

    return bytes_sent;
}

ssize_t ServerConnectionManager::dataReceive(int sfd, PacketData::packet_t& packet) {
    auto bytes_received = read(sfd, (void*) &packet, sizeof(PacketData::packet_t));

    return bytes_received;
}