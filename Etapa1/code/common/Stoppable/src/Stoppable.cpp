
#include <Stoppable.hpp>

bool signaling::_continue = true;
static void _kill(int) { signaling::_continue = false; }
static void _handler(int) { std::cout << "Heartbeat" << std::endl; }

static struct sigaction sact;

Stoppable::Stoppable() {
    signal(SIGINT, &_kill);

    sigemptyset(&sact.sa_mask);
    sact.sa_flags = 0;
    sact.sa_handler = _handler;
    sigaction(SIGALRM, &sact, NULL);
}

void Stoppable::registerSighup() {
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    signal(SIGHUP, &_kill);
}
