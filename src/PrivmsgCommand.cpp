#include "../includes/PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PrivmsgCommand::~PrivmsgCommand() {
}

void PrivmsgCommand::execute() {
}
