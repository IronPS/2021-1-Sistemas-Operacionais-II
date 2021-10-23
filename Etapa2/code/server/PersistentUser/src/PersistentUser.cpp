
#include <PersistentUser.hpp>

PersistentUser::PersistentUser(std::string username, unsigned int ID, PersistenceManager& pm, ReplicaManager& rm) 
: User(username, ID), _pm(pm), _rm(rm), _sem(1)
{

}

PersistentUser::PersistentUser(User user, PersistenceManager& pm, ReplicaManager& rm) 
: User(user), _pm(pm), _rm(rm), _sem(1)
{

}

PersistentUser::~PersistentUser() {

}

void PersistentUser::addFollower(std::string follower) {
    _sem.wait();

    if (_rm.waitCommit(PacketBuilder::replicateUser(_name, "ADDFOLLOWER," + follower))) {
        _followers.insert(follower);
        _pm.saveUser(*this);

    }

    _sem.notify();
}
