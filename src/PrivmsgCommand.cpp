#include "../includes/PrivmsgCommand.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"

PrivmsgCommand::PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PrivmsgCommand::~PrivmsgCommand() {
}

void PrivmsgCommand::execute() {
    if (!this->client->isRegistered()) {
        this->client->sendMessage(ERR_NOTREGISTERED(this->client->getNickname()));
        return;
    }
    if (this->params.size() != 2) {
        this->client->sendMessage(ERR_NEEDMOREPARAMS(this->client->getNickname(), "PRIVMSG"));
        return;
    }
    if (this->params[0][0] == '#' || this->params[0][0] == '&') {
        if (!this->server->channelExistOrNot(this->params[0])) {
            this->client->sendMessage(ERR_NOSUCHCHANNEL(this->client->getNickname(), this->params[0]));
            return;
        }
        Channel* channel = this->server->getOrCreateChannel(this->params[0]);
        if (!channel->isMember(this->client)) {
            this->client->sendMessage(ERR_CANNOTSENDTOCHAN(this->client->getNickname(), this->params[0]));
            return;
        }
        channel->broadcast(this->client->getPrefix() + " PRIVMSG " + this->params[0] + " " + this->params[1] + "\r\n", this->client);
    }
    else
    {
        Client *targetClient = this->server->getClientByNick(this->params[0]);
        if (targetClient == NULL) {
            this->client->sendMessage(ERR_NOSUCHNICK(this->client->getNickname(), this->params[0]));
            return;
        }
        targetClient->sendMessage(this->client->getPrefix() + " PRIVMSG " + this->params[0] + " " + this->params[1] + "\r\n");
    }
}
