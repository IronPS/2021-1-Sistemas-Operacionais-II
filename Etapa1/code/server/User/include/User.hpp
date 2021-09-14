
#pragma once

#include <set>
#include <string>
#include <iostream>

class PersistenceManager;

class User {
 public:
    User(std::string username, unsigned int ID);
    ~User();

    std::string name() { return _name; }
    unsigned int id() { return _ID; }
    std::set<std::string> followers() { return _followers; }

    void addFollower(std::string follower);
    
    /*
     * Serializes User to string format compatible with the persistence
     */
    friend std::ostream& operator<<(std::ostream& os, const User& user);

 protected:
    std::string _name;
    unsigned int _ID = 0;
    std::set<std::string> _followers;

    friend PersistenceManager;

};
