#include "../includes/UserCommand.hpp"

UserCommand::UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

UserCommand::~UserCommand() {
}

void UserCommand::execute() {
}
