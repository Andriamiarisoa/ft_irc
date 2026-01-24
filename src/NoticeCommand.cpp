#include "../includes/NoticeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"

NoticeCommand::NoticeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

NoticeCommand::~NoticeCommand() {
}

void NoticeCommand::execute() {
    // NOTICE does not generate error replies (RFC 1459)
    if (params.size() < 2)
        return;

    std::string target = params[0];
    std::string message = params[1];
    
    if (message.empty())
        return;

    std::string prefix = USER_PREFIX(client->getNickname(), client->getUsername(), client->getHostname());
    std::string noticeMsg = prefix + " NOTICE " + target + " :" + message;

    // Channel message
    if (target[0] == '#') {
        Channel* channel = server->getOrCreateChannel(target);
        if (!channel)
            return;
        if (!channel->isMember(client))
            return;
        channel->broadcast(noticeMsg, client);
    }
    // Private message
    else {
        Client* targetClient = server->getClientByNick(target);
        if (!targetClient)
            return;
        targetClient->sendMessage(noticeMsg);
    }
}
