#include "../includes/UserCommand.hpp"

UserCommand::UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

UserCommand::~UserCommand() {
}

void UserCommand::execute() {

    if (!this->client->isAuthenticated()) {
        sendError(464, "USER :You must send PASS first");
        return;
    }
    if (this->params.size() < 4) {
        sendError(461, "USER :Not enough parameters");
        return;
    }
    if (this->client->isRegistered()) {
        sendError(462, "USER:You may not reregister");
        return;
    }
    this->client->setUsername(this->params[0]);
    this->client->setRealname(this->params[3]);
    if (!this->client->getNickname().empty() && !this->client->getUsername().empty()) {
        this->client->registerClient();
        sendReply(001, ":Welcome to the IRC Network " + this->client->getNickname());
    }
}
