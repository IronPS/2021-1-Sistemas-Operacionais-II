
#pragma once

#include <ReplicaManager.hpp>
#include <PersistenceManager.hpp>
#include <SessionMonitor.hpp>

class Server : public ReplicaManager {
 public:
    Server(const cxxopts::ParseResult& input, PersistenceManager& pm, SessionMonitor& sm);
    ~Server();

 public:
    void newClient(int csfd);

 protected:
    void _handleUser(std::string username, int& csfd, SessionController* session);

 protected:
    PersistenceManager& _pm;
    SessionMonitor& _sm;

};
