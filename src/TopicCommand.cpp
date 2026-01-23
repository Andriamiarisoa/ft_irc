#include "../includes/TopicCommand.hpp"
#include "../includes/Channel.hpp"

TopicCommand::TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

TopicCommand::~TopicCommand() {
}

void TopicCommand::execute() {
    if (!this->client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (((this->params.size() != 1) && (this->params.size() != 2)) ||
    (this->params.size() == 2 && this->params[1][0] != ':')) {
        sendError(461, "TOPIC: Wrong parameters");
        return;
    }
    Channel *channel = this->server->getChannel(this->params[0]);
    if (channel == NULL) {
        sendError(403, this->client->getPrefix() + " :No such channel");
        return;
    }
    if (!channel->isMember(this->client)) {
        sendError(442, this->client->getPrefix() + " :You're not on that channel");
        return;
    }
    if (this->params.size() == 1) {
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            sendReply(331, this->client->getPrefix() + " :No topic is set");
            return;
        }
        this->sendReply(332, this->client->getPrefix() + " :" + channel->getTopic());
    }
    else {
        std::string topic = this->params[1].substr(1);
        if (channel->getTopic() == "+t" && !channel->isOperator(client)) {
            sendError(482, this->client->getPrefix() + " :You're not channel operator");
            return;
        }
        channel->setTopic(topic, this->client);
        channel->broadcast(this->client->getPrefix() + " TOPIC " + this->params[0] + " :" + topic.substr(1) + "\r\n", this->client);
    }
}