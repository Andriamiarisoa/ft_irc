#include "../includes/PrivmsgCommand.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"

PrivmsgCommand::PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PrivmsgCommand::~PrivmsgCommand() {
}

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

void    PrivmsgCommand::executeSingle(const std::string &receptor) {
    if (receptor[0] == '#' || receptor[0] == '&') {
        if (!this->server->channelExistOrNot(receptor)) {
            this->client->sendMessage(ERR_NOSUCHCHANNEL(this->client->getNickname(), receptor) + "\r\n");
            return;
        }
        Channel* channel = this->server->getOrCreateChannel(receptor);
        if (!channel->isMember(this->client)) {
            this->client->sendMessage(ERR_CANNOTSENDTOCHAN(this->client->getNickname(), receptor) + "\r\n");
            return;
        }
        channel->broadcast(this->client->getPrefix() + " PRIVMSG " + receptor + " " + this->params[1] + "\r\n", this->client);
    }
    else
    {
        Client *targetClient = this->server->getClientByNick(receptor);
        if (targetClient == NULL) {
            this->client->sendMessage(ERR_NOSUCHNICK(this->client->getNickname(), receptor) + "\r\n");
            return;
        }
        targetClient->sendMessage(this->client->getPrefix() + " PRIVMSG " + receptor + " " + this->params[1] + "\r\n");
    }
}

void PrivmsgCommand::execute() {
    if (!this->client->isRegistered()) {
        this->client->sendMessage(ERR_NOTREGISTERED(this->client->getNickname()) + "\r\n");
        return;
    }
    if (this->params.size() < 2) {
        this->client->sendMessage(ERR_NEEDMOREPARAMS(this->client->getNickname(), "PRIVMSG") + "\r\n");
        return;
    }
    std::vector<std::string> receptors = split(this->params[0], ',');
    for (std::vector<std::string>::iterator it = receptors.begin(); it != receptors.end(); ++it) {
        executeSingle(*it);
    }

}
