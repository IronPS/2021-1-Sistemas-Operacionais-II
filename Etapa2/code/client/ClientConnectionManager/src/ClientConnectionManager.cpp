
#include <ClientConnectionManager.hpp>

ClientConnectionManager::ClientConnectionManager(const cxxopts::ParseResult& input) {
    _server = input["server"].as<std::string>();
    _port = std::to_string(input["port"].as<unsigned short>());
    _listenerPort = std::to_string(input["listener"].as<unsigned short>());

    _openConnection(true, false);
    _createListener();

}

ClientConnectionManager::ClientConnectionManager(std::string serverAddress, unsigned short serverPort) {
    _server = serverAddress;
    _port = std::to_string(serverPort);
}

ClientConnectionManager::~ClientConnectionManager() {
    if (_clientSFD != -1) {
        close(_clientSFD);
    }
}

bool ClientConnectionManager::openConnection(bool exitOnFail, bool nonBlocking, bool ignoreErrorMessage) {
    return _openConnection(exitOnFail, nonBlocking, ignoreErrorMessage);
}

void ClientConnectionManager::closeConnection() {
    if (_clientSFD != -1) {
        close(_clientSFD);
        _clientSFD = -1;
    }
}

void ClientConnectionManager::closeConnection(int& sfd) {
    if (sfd != -1) {
        close(sfd);
        sfd = -1;
    }
}

void ClientConnectionManager::_createListener() {
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
        _listenerSFD = socket(addrptr->ai_family, addrptr->ai_socktype, addrptr->ai_protocol);

        if (_listenerSFD == -1) {
            err = errno;
            break;
        }

        if (bind(_listenerSFD, addrptr->ai_addr, addrptr->ai_addrlen) == 0)
            break;

        err = errno;

        close(_listenerSFD);
    }

    if (_listenerSFD == -1) {
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

        close(_listenerSFD);
        freeaddrinfo(addrs);
        
        exit(1);
    }

    freeaddrinfo(addrs);

    err = listen(_listenerSFD, _backlog);
    if (err == -1) {
        std::cerr << "Error listening to socket "
        << strerror(errno)
        << std::endl;
        close(_listenerSFD);

        exit(1);
    }

    fcntl(_listenerSFD, F_SETFL, fcntl(_listenerSFD, F_GETFL, 0) | O_NONBLOCK);
}

int ClientConnectionManager::getConnection() {
    sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // Socket File Descriptor
    int serverSFD = accept(_listenerSFD, (sockaddr*) &client_addr, &client_addr_size);

    if (serverSFD != -1) {
        struct timeval tv;
        tv.tv_sec = 1; // 1 seconds
        tv.tv_usec = 0;
        setsockopt(serverSFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    }

    return serverSFD;
}

bool ClientConnectionManager::setAddress(std::string address) {
    if (_clientSFD != -1) {
        return false;
    }

    _server = address;

    return true;
}

bool ClientConnectionManager::setPort(std::string port) {
    if (_clientSFD != -1) {
        return false;
    }

    _port = port;

    return true;
}

bool ClientConnectionManager::_openConnection(bool exitOnFail, bool nonBlocking, bool ignoreErrorMessage) {
    if (_clientSFD != -1) return false;

    struct addrinfo hints, *addrs = NULL;
    int err = 0;
    
    // Get address
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;
    err = getaddrinfo (
        _server.c_str(),
        _port.c_str(),
        &hints,
        &addrs
    );

    if (err != 0) {
        if (!ignoreErrorMessage) {
            std::cerr << "Error at getaddrinfo: "
            << gai_strerror(err)
            << std::endl;
        }
        if (exitOnFail) exit(1);
        return false;
    }

    for (struct addrinfo* addrpt = addrs; addrpt != NULL; addrpt = addrpt->ai_next) {
        _clientSFD = socket(addrpt->ai_family, addrpt->ai_socktype, addrpt->ai_protocol);

        if (_clientSFD == -1) {
            err = errno;
            break;
        }

        if (connect(_clientSFD, addrpt->ai_addr, addrpt->ai_addrlen) == 0)
            break;

        err = errno;

        close(_clientSFD);
        _clientSFD = -1;
    }

    freeaddrinfo(addrs);

    if (_clientSFD == -1) {
        if (!ignoreErrorMessage) {
            std::cerr << "Connection error (s1): "
            << strerror(err)
            << std::endl;
        }
        if (exitOnFail) exit(1);
        return false;
    }

    if (err != 0) {
        if (!ignoreErrorMessage) {
            std::cerr << "Connection error (e1): "
            << strerror(err)
            << std::endl;
        }
        if (exitOnFail) exit(1);
        return false;
    }

    // Set timeout
    struct timeval tv;
    tv.tv_sec = 1; // 1 seconds
    tv.tv_usec = 0;
    setsockopt(_clientSFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if (nonBlocking) fcntl(_clientSFD, F_SETFL, fcntl(_clientSFD, F_GETFL, 0) | O_NONBLOCK);
    
    return true;
}

// PURELY FOR DEBUG, delete this once client<->server communication is working
void ClientConnectionManager::_print_packet(PacketData::packet_t packet) {
    std::cout   << "Packet:\n"
                << "Payload: " << packet.payload << "\n"
                << "Extra: " << packet.extra << "\n"
                << "Type: " << packet.type << "\n"
                << "Timestamp: " << packet.timestamp << "\n"
                << std::endl; 
}


ssize_t ClientConnectionManager::dataSend(PacketData::packet_t packet) {
    // To go back to communicating with server:
    // Delete this,
    //std::cout << "Sending to socket " << _clientSFD << "..." << "\n" << packet.payload << std::endl;

    // Restore this,
    // std::cout << "Sending packet\n";
    auto bytes_sent = send(_clientSFD, (void*) &packet, sizeof(PacketData::packet_t), 0);

    // And delete this, from here:
    // print_packet(packet);
    // std::cout << "Bytes sent: " << bytes_sent << std::endl;
    // ssize_t bytes_sent = 99;
    // : to here

    return bytes_sent;
}

ssize_t ClientConnectionManager::dataReceive(PacketData::packet_t& packet) {
    auto bytes_received = read(_clientSFD, &packet, sizeof(PacketData::packet_t));

    return bytes_received;
}

ssize_t ClientConnectionManager::dataSend(int sfd, PacketData::packet_t packet) {
    auto bytes_sent = send(sfd, (void*) &packet, sizeof(PacketData::packet_t), 0);

    return bytes_sent;
}

ssize_t ClientConnectionManager::dataReceive(int sfd, PacketData::packet_t& packet) {
    auto bytes_received = read(sfd, (void*) &packet, sizeof(PacketData::packet_t));

    return bytes_received;
}