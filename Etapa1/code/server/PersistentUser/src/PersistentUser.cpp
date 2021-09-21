
#include <PersistentUser.hpp>

PersistentUser::PersistentUser(std::string username, unsigned int ID, PersistenceManager& pm) 
: User(username, ID), _pm(pm)
{

}

PersistentUser::PersistentUser(User user, PersistenceManager& pm) 
: User(user), _pm(pm) 
{

}

PersistentUser::~PersistentUser() {

}

void PersistentUser::addFollower(std::string follower) {
    _sem.wait();

    _followers.insert(follower);
    _pm.saveUser(*this);

    _sem.notify();
}
