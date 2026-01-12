#include "../includes/InviteCommand.hpp"

InviteCommand::InviteCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

InviteCommand::~InviteCommand() {
}

void InviteCommand::execute() {
}
