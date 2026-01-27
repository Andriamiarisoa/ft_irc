#include "includes/Server.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    char* endptr;
    long portLong = std::strtol(argv[1], &endptr, 10);
    
    if (*endptr != '\0' || endptr == argv[1]) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }
    if (portLong < 1 || portLong > 65535) {
        std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
        return 1;
    }
    
    int port = static_cast<int>(portLong);
    std::string password = argv[2];

    try {
        Server server(port, password);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
