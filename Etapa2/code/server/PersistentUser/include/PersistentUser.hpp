
#pragma once

#include <Semaphore.hpp>

#include <User.hpp>
#include <PersistenceManager.hpp>
#include <ReplicaManager.hpp>

class PersistentUser : public User {
 public:
    PersistentUser(std::string username, unsigned int ID, PersistenceManager& pm, ReplicaManager& rm);
    PersistentUser(User user, PersistenceManager& pm, ReplicaManager& rm);
    ~PersistentUser();

    void addFollower(std::string follower);

    PersistentUser& operator=(const PersistentUser&) = delete;

 private:
    PersistenceManager& _pm;
    ReplicaManager& _rm;
    Semaphore _sem;
};

