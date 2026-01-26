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
        this->client->sendMessage(ERR_NOTREGISTERED(this->client->getNickname()) + "\r\n");
        return;
    }
    if (this->params.empty()) {
        this->client->sendMessage(ERR_NEEDMOREPARAMS(this->client->getNickname(), "TOPIC") + "\r\n");
        return;
    }
    Channel *channel = this->server->getChannel(this->params[0]);
    if (channel == NULL) {
        this->client->sendMessage(ERR_NOSUCHCHANNEL(this->client->getNickname(), this->params[0]) + "\r\n");
        return;
    }
    if (!channel->isMember(this->client)) {
        this->client->sendMessage(ERR_NOTONCHANNEL(this->client->getNickname(), this->params[0]) + "\r\n");
        return;
    }
    if (this->params.size() == 1) {
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            this->client->sendMessage(RPL_NOTOPIC(this->client->getNickname(), this->params[0]) + "\r\n");
            return;
        }
        this->client->sendMessage(RPL_TOPIC(this->client->getNickname(), this->params[0], topic) + "\r\n");
    }
    else {
        std::string topic = this->params[1];
        if (!topic.empty() && topic[0] == ':') {
            topic = topic.substr(1);
        }
        if (channel->getRestriction() && !channel->isOperator(client)) {
            this->client->sendMessage(ERR_CHANOPRIVSNEEDED(this->client->getNickname(), this->params[0]) + "\r\n");
            return;
        }
        channel->setTopic(topic, this->client);
    }
}