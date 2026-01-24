#include "../includes/TopicCommand.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"

TopicCommand::TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

TopicCommand::~TopicCommand() {
}

void TopicCommand::execute() {
    if (!this->client->isRegistered()) {
        this->client->sendMessage(ERR_NOTREGISTERED(this->client->getNickname()));
        return;
    }
    if (((this->params.size() != 1) && (this->params.size() != 2)) ||
    (this->params.size() == 2 && this->params[1][0] != ':')) {
        this->client->sendMessage(ERR_NEEDMOREPARAMS(this->client->getNickname(), "TOPIC"));
        return;
    }
    Channel *channel = this->server->getChannel(this->params[0]);
    if (channel == NULL) {
        this->client->sendMessage(ERR_NOSUCHCHANNEL(this->client->getNickname(), this->params[0]));
        return;
    }
    if (!channel->isMember(this->client)) {
        this->client->sendMessage(ERR_NOTONCHANNEL(this->client->getNickname(), this->params[0]));
        return;
    }
    if (this->params.size() == 1) {
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            this->client->sendMessage(RPL_NOTOPIC(this->client->getNickname(), this->params[0]));
            return;
        }
        this->client->sendMessage(RPL_TOPIC(this->client->getNickname(), this->params[0], topic));
    }
    else {
        std::string topic = this->params[1].substr(1);
        if (channel->getTopic() == "+t" && !channel->isOperator(client)) {
            this->client->sendMessage(ERR_CHANOPRIVSNEEDED(this->client->getNickname(), this->params[0]));
            return;
        }
        channel->setTopic(topic, this->client);
        channel->broadcast(this->client->getPrefix() + " TOPIC " + this->params[0] + " :" + topic.substr(1) + "\r\n", this->client);
    }
}