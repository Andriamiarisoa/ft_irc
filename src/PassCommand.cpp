#include "../includes/PassCommand.hpp"

PassCommand::PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PassCommand::~PassCommand() {
}

void PassCommand::execute() {
    if (params.empty()) {
        sendError(462, "PASS :Not enough parameters");
        return;
    }
    // if (client->isRegistered) {

    // }
}
