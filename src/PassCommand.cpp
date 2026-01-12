#include "../includes/PassCommand.hpp"

PassCommand::PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PassCommand::~PassCommand() {
}

void PassCommand::execute() {
}
