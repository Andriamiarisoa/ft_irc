#include "../includes/NickCommand.hpp"

NickCommand::NickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

NickCommand::~NickCommand() {
}

void NickCommand::execute() {
}
