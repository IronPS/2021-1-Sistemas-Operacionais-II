
#pragma once

#include <chrono>
#include <thread>

#include <ServerConnectionManager.hpp>
#include <SessionController.hpp>
#include <SessionMonitor.hpp>
#include <Stoppable.hpp>
#include <PacketBuilder.hpp>

namespace ClientFunctions {
    void newConnection(int csfd, SessionMonitor& sm);
}
