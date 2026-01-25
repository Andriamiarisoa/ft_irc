#include "../includes/InviteCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>

InviteCommand::InviteCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

InviteCommand::~InviteCommand() {
}

void InviteCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";

    if (!client->isRegistered()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return;
    }
    if (params.empty() || params[0].empty()) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "INVITE") + "\r\n");
        return;
    }
    std::string targetNick = params[0];;
    std::string channelName = params[1];
    Channel* channel = server->getChannel(channelName);
    if (!channel) {
        client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
        return;
    }
    if (!channel->isMember(client)) {
        client->sendMessage(ERR_NOTONCHANNEL(nick, channelName) + "\r\n");
        return;
    }
    if (channel->isChannelInvitOnly() && !channel->isOperator(client)) {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(nick, channelName) + "\r\n");
        return;
    }
    Client* targetClient = server->getClientByNick(targetNick);
    if (!targetClient) {
        client->sendMessage(ERR_NOSUCHNICK(nick, targetNick) + "\r\n");
        return;
    }
    if (channel->isMember(targetClient)) {
        client->sendMessage(ERR_USERONCHANNEL(nick, targetNick, channelName) + "\r\n");
        return;
    }
    channel->inviteUser(targetClient);
    std::string inviteMsg =client->getPrefix() + " INVITE " +
                          targetNick + " " + channelName + "\r\n";
    targetClient->sendMessage(inviteMsg);
    client->sendMessage(RPL_INVITING(nick, targetNick, channelName) + "\r\n");
}