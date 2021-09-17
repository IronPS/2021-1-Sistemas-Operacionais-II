
#include <ClientConnectionManager.hpp>

ClientConnectionManager::ClientConnectionManager(const cxxopts::ParseResult& input) {
    _user = input["user"].as<std::string>();
    _server = input["server"].as<std::string>();
    _port = std::to_string(input["port"].as<unsigned short>());
    
    _openConnection();

}


ClientConnectionManager::~ClientConnectionManager() {
    if (_socketDesc != -1) {
        close(_socketDesc);
    }
}

void ClientConnectionManager::_openConnection() {
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
        std::cerr << "Error at getaddrinfo: "
        << gai_strerror(err)
        << std::endl;
        exit(1);
    }

    for (struct addrinfo* addrpt = addrs; addrpt != NULL; addrpt = addrpt->ai_next) {
        _socketDesc = socket(addrpt->ai_family, addrpt->ai_socktype, addrpt->ai_protocol);

        if (_socketDesc == -1) {
            err = errno;
            break;
        }

        if (connect(_socketDesc, addrpt->ai_addr, addrpt->ai_addrlen) == 0)
            break;

        err = errno;

        close(_socketDesc);
    }

    freeaddrinfo(addrs);

    if (_socketDesc == -1) {
        std::cerr << "Connection error: "
        << strerror(err)
        << std::endl;
        exit(1);
    }

    if (err != 0) {
        std::cerr << "Connection error: "
        << strerror(err)
        << std::endl;
        exit(1);
    }


}

ssize_t ClientConnectionManager::dataSend(PacketData::packet_t packet) {
    auto bytes_sent = send(_socketDesc, (void*) &packet, sizeof(PacketData::packet_t), 0);

    return bytes_sent;
}

ssize_t ClientConnectionManager::dataReceive(PacketData::packet_t& packet) {
    auto bytes_received = read(_socketDesc, &packet, sizeof(PacketData::packet_t));

    return bytes_received;
}