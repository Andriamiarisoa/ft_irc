#include "../includes/PrivmsgCommand.hpp"
#include "../includes/Channel.hpp"

PrivmsgCommand::PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PrivmsgCommand::~PrivmsgCommand() {
}

void PrivmsgCommand::execute() {
    if (!this->client->isRegistered()) {
        this->sendError(451, ":You have not registered");
        return;
    }
    if (this->params.size() != 2) {
        this->sendError(411, "Wrong parameters");
        return;
    }
    if (this->params[0][0] == '#' || this->params[0][0] == '&') {
        if (!this->server->channelExistOrNot(this->params[0])) {
            this->sendError(403, this->params[0] + " :No such channel");
            return;
        }
        Channel* channel = this->server->getOrCreateChannel(this->params[0]);
        if (!channel->isMember(this->client)) {
            this->sendError(404, this->params[0] + " :Cannot send to channel");
            return;
        }
        channel->broadcast(":" + this->client->getNickname() + " PRIVMSG " + this->params[0] + " " + this->params[1] + "\r\n", this->client);
    }
    else
    {
        Client *targetClient = this->server->getClientByNick(this->params[0]);
        if (targetClient == NULL) {
            this->sendError(401, this->params[0] + " :No such client");
            return;
        }
        targetClient->sendMessage(":" + this->client->getNickname() + " PRIVMSG " + this->params[0] + " " + this->params[1] + "\r\n");
    }
}
