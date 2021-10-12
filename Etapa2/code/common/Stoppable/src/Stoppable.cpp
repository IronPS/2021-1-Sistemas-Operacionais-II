
#include <Stoppable.hpp>

volatile sig_atomic_t signaling::_continue = true;
volatile sig_atomic_t signaling::_heartbeat = true;

static void _kill(int) {
    signaling::_continue = 0;
}

static void _alarmHandler(int) { 
    signaling::_heartbeat = true;
    alarm(5);
}

static struct sigaction sact;


Stoppable::Stoppable() {
    signaling::_heartbeat = true;

    signal(SIGINT, &_kill);
    signal(SIGPIPE, SIG_IGN);

    sigemptyset(&sact.sa_mask);
    sact.sa_flags = 0;
    sact.sa_handler = _alarmHandler;
    sigaction(SIGALRM, &sact, NULL);
    
    alarm(5);
}

void Stoppable::registerSighup() {
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    signal(SIGHUP, &_kill);
}
