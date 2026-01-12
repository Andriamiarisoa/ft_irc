#include "../includes/PartCommand.hpp"

PartCommand::PartCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PartCommand::~PartCommand() {
}

void PartCommand::execute() {
}
