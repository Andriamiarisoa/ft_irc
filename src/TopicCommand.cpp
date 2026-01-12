#include "../includes/TopicCommand.hpp"

TopicCommand::TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

TopicCommand::~TopicCommand() {
}

void TopicCommand::execute() {
}
