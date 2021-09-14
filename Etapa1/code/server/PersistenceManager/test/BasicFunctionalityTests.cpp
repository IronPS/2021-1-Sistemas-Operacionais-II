#include <gtest/gtest.h>
#include <PersistenceManager.hpp>
#include <iostream>
#include <cstdio>

std::string copy_to_tmp(std::string filename) {
    std::string out_path = RESOURCES_TMP + filename;
    std::ifstream src(RESOURCES + filename, std::ios::in);
    std::ofstream out(out_path);

    out << src.rdbuf();

    src.close();
    out.close();

    return out_path;
}

std::string load_file(std::string filename) {
    std::cerr << "============ Trying to load " << RESOURCES_TMP + filename << std::endl;
    std::ifstream src(RESOURCES_TMP + filename);
    std::stringstream ss;
    ss << src.rdbuf();

    return ss.str();
}

std::string load_GT(std::string filename) {
    std::cerr << "============ Trying to load " << RESOURCES + std::string("ground_truths/") + filename << std::endl;

    std::ifstream src(RESOURCES + std::string("ground_truths/") + filename);
    std::stringstream ss;
    ss << src.rdbuf();

    return ss.str();
}


std::string toberemoved_file = RESOURCES_TMP + std::string("todelete.db");
TEST(PersistenceManager, InexistentCreationAndDestruction) {
    PersistenceManager pm(toberemoved_file);
}

TEST(PersistenceManager, ExistentCreationAndDestruction) {
    PersistenceManager* pm = new PersistenceManager(toberemoved_file);
    delete pm;
    std::remove(toberemoved_file.c_str());
}

TEST(PersistenceManager, CreateFromEmpty) {
    std::string dbname = "databases/emptydb.db";
    PersistenceManager pm(copy_to_tmp(dbname));
    User user0 = pm.loadUser("user0");

    std::string db = load_file(dbname);
    std::string gt = load_GT("CreateFromEmpty.txt");

    EXPECT_TRUE(gt.size() > 0);
    EXPECT_TRUE(db.size() > 0);
    EXPECT_TRUE(db == gt);

    EXPECT_TRUE(user0.id() == 0);
    EXPECT_TRUE(user0.name() == "user0");

}

TEST(PersistenceManager, LoadExistent) {
    std::string dbname = "databases/user0.db";
    PersistenceManager pm(copy_to_tmp(dbname));
    User user0 = pm.loadUser("user0");

    std::string db = load_file(dbname);
    std::string gt = load_GT("user0.txt");

    EXPECT_TRUE(gt.size() > 0);
    EXPECT_TRUE(db.size() > 0);
    EXPECT_TRUE(db == gt);

    EXPECT_TRUE(user0.id() == 0);
    EXPECT_TRUE(user0.name() == "user0");
    
}

TEST(PersistenceManager, LoadExistentMiddle) {
    std::string dbname = "databases/threeUsers.db";
    PersistenceManager pm(copy_to_tmp(dbname));
    User user = pm.loadUser("user1");

    std::string db = load_file(dbname);
    std::string gt = load_GT("threeUsers.txt");

    EXPECT_TRUE(gt.size() > 0);
    EXPECT_TRUE(db.size() > 0);
    EXPECT_TRUE(db == gt);

    EXPECT_TRUE(user.id() == 1);
    EXPECT_TRUE(user.name() == "user1");
    
}

TEST(PersistenceManager, ThreeUsersCreateOne) {
    std::string dbname = "databases/threeUsers.db";
    PersistenceManager pm(copy_to_tmp(dbname));
    User user = pm.loadUser("user3");

    std::string db = load_file(dbname);
    std::string gt = load_GT("threeUsersPlusOne.txt");

    EXPECT_TRUE(gt.size() > 0);
    EXPECT_TRUE(db.size() > 0);
    EXPECT_TRUE(db == gt);

    EXPECT_TRUE(user.id() == 3);
    EXPECT_TRUE(user.name() == "user3");
    
}

