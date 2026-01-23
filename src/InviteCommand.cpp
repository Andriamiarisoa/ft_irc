#include "../includes/InviteCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <vector>
#include <string>

InviteCommand::InviteCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

InviteCommand::~InviteCommand() {
}

void InviteCommand::execute() {
    if (!client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (params.empty() || params[0].empty() || params.size() < 2) {
        sendError(461, "INVITE :Not enough parameters");
        return;
    }
    std::string targetNick = params[0];;
    std::string channelName = params[1];
    Channel* channel = server->getChannel(channelName);
    if (!channel) {
        sendError(403, channelName + " :No such channel");
        return;
    }
    if (!channel->isMember(client)) {
        sendError(442, channelName + " :You're not on that channel");
        return;
    }
    if (channel->isChannelInvitOnly() && !channel->isOperator(client)) {
        sendError(482, channelName + " :You're not channel operator");
        return;
    }
    Client* targetClient = server->getClientByNick(targetNick);
    if (!targetClient) {
        sendError(401, targetNick + " :No such nick/channel");
        return;
    }
    if (channel->isMember(targetClient)) {
        sendError(443, targetNick + " " + channelName + " :is already on channel");
        return;
    }
    channel->inviteUser(targetClient);
    std::string inviteMsg = ":" + client->getNickname() + "!" +
                          client->getUsername() + "@host INVITE " +
                          targetNick + " " + channelName + "\r\n";
    targetClient->sendMessage(inviteMsg);
    sendReply(341, targetNick + " " + channelName);
}