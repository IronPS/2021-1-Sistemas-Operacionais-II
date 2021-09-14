
#pragma once

#include <iostream>

#include <signal.h>
#include <cstddef>
#include <unistd.h>
#include <sys/prctl.h>

namespace signaling {
    extern bool _continue;
};

class Stoppable {
 public:
    Stoppable();
    ~Stoppable() {}

 protected:
    bool _continue() { return signaling::_continue; };

    void registerSighup();
};
