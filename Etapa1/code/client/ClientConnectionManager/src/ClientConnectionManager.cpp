
#include <ClientConnectionManager.hpp>

ClientConnectionManager::ClientConnectionManager(const cxxopts::ParseResult& input) {
    user = input["user"].as<std::string>();
    server = input["server"].as<std::string>();
    port = std::to_string(input["port"].as<unsigned short>());
    
    _openConnection();

}


ClientConnectionManager::~ClientConnectionManager() {
    if (socketDesc != -1) {
        close(socketDesc);
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
        server.c_str(),
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

    for (struct addrinfo* addrpt = addrs; addrpt != NULL; addrpt = addrpt->ai_next) {
        socketDesc = socket(addrpt->ai_family, addrpt->ai_socktype, addrpt->ai_protocol);

        if (socketDesc == -1) {
            err = errno;
            break;
        }

        if (connect(socketDesc, addrpt->ai_addr, addrpt->ai_addrlen) == 0)
            break;

        err = errno;

        close(socketDesc);
    }

    freeaddrinfo(addrs);

    if (socketDesc == -1) {
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

// TODO
void ClientConnectionManager::dataSend() {
    // Placeholder 
}

// TODO
void ClientConnectionManager::dataReceive() {
    // Placeholder 
    int n = 0;
    char recvBuff[1024];

    memset(recvBuff, '0', sizeof(recvBuff));

    while ( (n = read(socketDesc, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        std::cout<< "recvBuff=" << recvBuff << std::endl;
    }
}