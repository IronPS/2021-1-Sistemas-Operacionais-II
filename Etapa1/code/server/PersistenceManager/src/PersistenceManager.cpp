
#include <PersistenceManager.hpp>

PersistenceManager::PersistenceManager(std::string databasePath) {
    std::ifstream infile(databasePath);
    if(infile.good()) { // File exists
        infile.close();

        _dbFile.open(databasePath, std::fstream::in | std::fstream::out);
        
    } else { // File doesn't exist
        infile.close();

        _dbFile.open(databasePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
    }

    if (!_dbFile.is_open()) {
        std::cerr << "Error: failed to open Database '" << databasePath << "'" << std::endl;
        exit(1);
    }
}

PersistenceManager::~PersistenceManager() {
    if (_dbFile.is_open()) {
        _dbFile.close();
    }

}

User PersistenceManager::loadUser(std::string username) {

    // lock

    std::string search = username + ",";

    _dbFile.seekg(std::ios::beg);
    std::string line;
    unsigned int lineCursor = 0;
    if (!_dbFile.eof()) {
        for (lineCursor = 0; std::getline(_dbFile, line); lineCursor++) {
            if (line.find(search) != std::string::npos) {
                break;
            }
        }
    }

    _dbFile.clear(); // Clear errors (?)

    std::string uname = username;
    std::set<std::string> followers;
    unsigned int ID = lineCursor;
    bool mustCreate = line.empty();
    if (!line.empty()) { // Found
        std::istringstream userData(line);

        // Get username - redundant, but either way we need to set the file cursor
        std::getline(userData, uname, ',');
        
        std::string follower;
        // Get followers
        while(std::getline(userData, follower, ';')) {
            followers.insert(follower);
        }

    }

    User user(uname, ID);
    _setFollowers(user, followers);

    if (mustCreate) {
        _privateSaveUser(user);
    }

    _dbFile.clear(); // Clear errors (?)

    // unlock

    return user;

}

void PersistenceManager::saveUser(User& user) {
    // lock

    _privateSaveUser(user);


    // unlock
}

void PersistenceManager::_privateSaveUser(User& user) {
    // We have to rewrite the whole file :/
    _dbFile.clear(); // Clear errors (?)

    _dbFile.seekg(std::ios::beg);

    std::stringstream out;
    std::string line;
    for(unsigned int i = 0; i < user.id(); ++i){
        std::getline(_dbFile, line);
        out << line << "\n";
    }

    out << user << "\n";

    if (!_dbFile.eof()) {
        std::getline(_dbFile, line); // Ignore line
        while (std::getline(_dbFile, line)) {
            out << line << "\n";
        }
    }

    _dbFile.clear(); // Clear errors (?)

    _dbFile.seekg(std::ios::beg);

    _dbFile << out.str();

    _dbFile.flush();

    _dbFile.seekg(std::ios::beg);

}

void PersistenceManager::_setFollowers(User& user, std::set<std::string> followers) {
    user._followers = followers;
}
