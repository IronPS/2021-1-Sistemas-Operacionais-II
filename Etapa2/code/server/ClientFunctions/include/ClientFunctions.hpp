
#pragma once

#include <chrono>
#include <thread>

#include <ServerConnectionManager.hpp>
#include <SessionController.hpp>
#include <SessionMonitor.hpp>
#include <Stoppable.hpp>
#include <PacketBuilder.hpp>
#include <PersistenceManager.hpp>
#include <ReplicaManager.hpp>

namespace ClientFunctions {
    void newConnection(int csfd, SessionMonitor& sm, PersistenceManager& pm, ReplicaManager& rm);
    void handleUser(std::string username, int& csfd, SessionController* session, SessionMonitor& sm, PersistenceManager& pm);

}
