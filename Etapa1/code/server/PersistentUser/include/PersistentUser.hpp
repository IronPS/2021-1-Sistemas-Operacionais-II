
#pragma once

#include <User.hpp>
#include <PersistenceManager.hpp>

class PersistentUser : User {
 public:
    PersistentUser(std::string username, unsigned int ID, PersistenceManager& pm);
    ~PersistentUser();

    void addFollower(std::string follower);

 private:
    PersistenceManager& _pm;
};

