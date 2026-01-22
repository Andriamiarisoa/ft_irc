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
    if (this->params.size() != 2 || this->params[1][0] != ':') {
        this->sendError(411, "Wrong parameters");
        return;
    }
    if (this->params[0][0] == '#' || this->params[0][0] == '&') {
        Channel* channel = this->server->getChannel(this->params[0]);//to change after
        if (channel == NULL) {
            this->sendError(403, params[0] + " :No such channel");
            return;
        }
        if (!channel->isMember(this->client)) {
            this->sendError(404, params[0] + " :Cannot send to channel");
            return;
        }
        channel->broadcast(":" + this->client->getNickname() + " PRIVMSG " + params[0] + " " + params[1] + "\r\n", this->client);
    }
    else
    {
        Client *targetClient = this->server->getClientByNick(this->params[0]);
        if (targetClient == NULL) {
            this->sendError(401, params[0] + " :No such client");
            return;
        }
        targetClient->sendMessage(":" + this->client->getNickname() + " PRIVMSG " + params[0] + " " + params[1] + "\r\n");
    }
}
