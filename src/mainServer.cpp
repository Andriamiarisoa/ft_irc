#include "../includes/Server.hpp"

int     main() {
    Server Nero(6667, "testpassword");

    Nero.start();
    return (0);
}