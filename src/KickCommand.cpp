#include "../includes/KickCommand.hpp"

KickCommand::KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

KickCommand::~KickCommand() {
}

void KickCommand::execute() {
}
