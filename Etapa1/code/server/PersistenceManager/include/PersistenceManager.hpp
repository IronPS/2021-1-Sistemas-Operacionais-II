
#pragma once

#include <User.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

class PersistenceManager {
 public:
    PersistenceManager(std::string databasePath);
    ~PersistenceManager();

    User loadUser(std::string username);
    void saveUser(User& user);

 private:
    std::fstream _dbFile;

    void _privateSaveUser(User& user);
    void _setFollowers(User& user, std::set<std::string> followers);

};
