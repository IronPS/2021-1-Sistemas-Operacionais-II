
#include <parser.hpp>
#include <ServerConnectionManager.hpp>

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    ServerConnectionManager cm(results);
}