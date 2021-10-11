
#pragma once

#include <iostream>

#include <signal.h>
#include <cstddef>
#include <unistd.h>
#include <sys/prctl.h>

namespace signaling {
    extern volatile sig_atomic_t _continue;
    extern volatile sig_atomic_t _heartbeat;
};

class Stoppable {
 public:
    Stoppable();
    ~Stoppable() {}

 protected:
    bool _continue() { return signaling::_continue; };

    void registerSighup();
};
