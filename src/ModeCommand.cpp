#include "../includes/ModeCommand.hpp"

ModeCommand::ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

ModeCommand::~ModeCommand() {
}

void ModeCommand::execute() {
}
