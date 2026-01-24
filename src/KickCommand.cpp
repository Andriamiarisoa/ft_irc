#include "../includes/KickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"


KickCommand::KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

KickCommand::~KickCommand() {
}

void KickCommand::execute() {
    if (!client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }

    if (params.size() < 2) {
        sendError(461, "KICK :Not enough parameters");
        return;
    }

    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = client->getNickname(); 
    if (params.size() >= 3) {
        reason = params[2];
    }

    if (!server->channelExistOrNot(channelName)) {
        sendError(403, channelName + " :No such channel");
        return;
    }
    Channel* channel = server->getChannel(channelName);

    if (!channel->isMember(client)) {
        sendError(442, channelName + " :You're not on that channel");
        return;
    }

    if (!channel->isOperator(client)) {
        sendError(482, channelName + " :You're not a channel operator");
        return;
    }
    
    Client* target = server->getClientByNick(targetNick);
    if (target == NULL) {
        sendError(401, targetNick + " :No such nick/channel");
        return;
    }

    if (!channel->isMember(target)) {
        sendError(441, targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }
    channel->kickMember(client, target, reason);
}
