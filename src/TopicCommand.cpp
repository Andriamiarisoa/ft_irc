#include "../includes/TopicCommand.hpp"
#include "../includes/Channelhpp"

TopicCommand::TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

TopicCommand::~TopicCommand() {
}
//todo: # [0] :[1]
void TopicCommand::execute() {
    if (!this->client->isregistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (this->params.size() != 1 && this->params.size() != 2) {
        sendError(461, "TOPIC: Wrong parameters");
        return;
    }
    Channel *channel = this->server->getChannel(this->params[0]);
    if (channel == NULL) {
        sendError(442, this->params[0] + " :You're not on that channel");
        return;
    }
    if (this->params.size() == 1) {
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            sendReply(331, this->params[0] + " :No topic is set");
            return;
        }
        this->sendReply(332, this->params[0] + " :" + channel->getTopic());
    }
    else {
        if (channel->getTopic() == "+t" && !this->client->isOperator(client)) {
            sendError(482, this->params[0] + " :You're not channel operator");
            return;
        }
        std::string topic = topic.substr(1);
            channel->broadcast(":" + client->getNickname() + " TOPIC " + this->params[0] + " :" + topic.substr(1) + "\r\n");
    }
}
