#include "../includes/PongCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

PongCommand::PongCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PongCommand::~PongCommand() {
}

void PongCommand::execute() {
    // PONG is a response to PING from server
    // When client sends PONG, we just acknowledge it (no response needed)
    // This resets the client's idle timer if implemented
    (void)params;  // Unused for now
}
