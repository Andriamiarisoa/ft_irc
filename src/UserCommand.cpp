#include "../includes/UserCommand.hpp"
#include "../includes/Replies.hpp"

UserCommand::UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

UserCommand::~UserCommand() {
}

void UserCommand::execute() {

    if (!this->client->isAuthenticated()) {
        this->client->sendMessage(ERR_NOTREGISTERED(this->client->getNickname().empty() ? "*" : this->client->getNickname()) + "\r\n");
        return;
    }
    if (this->params.size() != 4 || this->params[3].empty() || this->params[3][0] != ':') {
        this->client->sendMessage(ERR_NEEDMOREPARAMS(this->client->getNickname().empty() ? "*" : this->client->getNickname(), "USER") + "\r\n");
        return;
    }
    if (this->client->isRegistered()) {
        this->client->sendMessage(ERR_ALREADYREGISTERED(this->client->getNickname()) + "\r\n");
        return;
    }
    this->client->setUsername(this->params[0]);
    this->client->setRealname(this->params[3].substr(1));
    if (!this->client->getNickname().empty() && !this->client->getUsername().empty()) {
        this->client->registerClient();
        this->client->sendMessage(RPL_WELCOME(this->client->getNickname(), this->client->getUsername(), this->client->getHostname()) + "\r\n");
        this->client->sendMessage(RPL_YOURHOST(this->client->getNickname()) + "\r\n");
        this->client->sendMessage(RPL_MYINFO(this->client->getNickname()) + "\r\n");
    }
}
