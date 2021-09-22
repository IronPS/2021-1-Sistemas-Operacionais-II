
#include <User.hpp>

User::User(std::string username, unsigned int ID) {
    _name = username;
    _ID = ID;
}

User::User(const User& user) {
    _name = user._name;
    _ID = user._ID;
    _followers = user._followers;
}

User::~User() {

}

void User::addFollower(std::string follower) {
    _followers.insert(follower);
}

std::ostream& operator<<(std::ostream& os, const User& user) {
    os << user._name << ",";
    for (auto follower : user._followers) {
        os << follower << ";";
    }

    return os;
}