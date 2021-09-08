
#include <parser.hpp>
#include <ClientConnectionManager.hpp>

int main(int argc, char* argv[]) {
    auto results = parse(argc, argv);

    ClientConnectionManager cm(results);
    cm.dataReceive();
}