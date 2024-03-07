#include <iostream>
#include <cstdlib>
#include "Server.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    Server ircServer(port, password);
    ircServer.run();

    return EXIT_SUCCESS;
}
