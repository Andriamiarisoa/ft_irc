#include "../includes/QuitCommand.hpp"

QuitCommand::QuitCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute() {
}
