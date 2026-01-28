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
    if (params.size() < 2)
        return;

    std::string target = params[0];
    std::string message = params[1];
    
    if (message.empty())
        return;

    std::string prefix = USER_PREFIX(client->getNickname(), client->getUsername(), "unknown.host");
    std::string noticeMsg = prefix + " NOTICE " + target + " " + message + "\r\n";
    
    if (target[0] == '#' || target[0] == '&') {
        Channel* channel = server->getChannel(target);
        if (!channel)
            return;
        if (!channel->isMember(client))
            return;
        channel->broadcast(noticeMsg, client);
    } else {
        Client* targetClient = server->getClientByNick(target);
        if (!targetClient)
            return;
        targetClient->sendMessage(noticeMsg);
    }
}
