#include "../includes/JoinCommand.hpp"

JoinCommand::JoinCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

JoinCommand::~JoinCommand() {
}

void JoinCommand::execute() {
}
