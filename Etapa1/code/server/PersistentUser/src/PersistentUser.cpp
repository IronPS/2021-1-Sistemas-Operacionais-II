
#include <PersistentUser.hpp>

PersistentUser::PersistentUser(std::string username, unsigned int ID, PersistenceManager& pm) 
: User(username, ID), _pm(pm) {

}

PersistentUser::~PersistentUser() {

}

void PersistentUser::addFollower(std::string follower) {
    // TODO lock

    _followers.insert(follower);
    _pm.saveUser(*this);

    // TODO unlock
}
