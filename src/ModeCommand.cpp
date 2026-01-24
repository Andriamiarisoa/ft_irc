#include "../includes/ModeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>

ModeCommand::ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

ModeCommand::~ModeCommand() {
}

void ModeCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";

    if (!client->isRegistered()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return;
    }
    if (params.size() < 2) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "KICK") + "\r\n");
        return;
    }
    std::string channelName = params[0];
    Channel* channel = server->getChannel(channelName);
    if(channel == NULL) {
        client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
        return;
    }
    if (!channel->isMember(client)) {
        client->sendMessage(ERR_NOTONCHANNEL(nick, channelName) + "\r\n");
        return;
    }
    if (params.size() == 1) {
        std::string modeStr = "+";
        if (channel->isChannelInvitOnly()) modeStr += "i";
        if (channel->hasKey()) modeStr += "k";
        if (channel->getUserLimit() > 0) modeStr += ("l " + channel->getUserLimit());
        client->sendMessage(RPL_CHANNELMODEIS(nick, channelName, modeStr, "") + "\r\n");
        return;
    }
    if (!channel->isOperator(client)) {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(nick, channelName) + "\r\n");
        return;
    }
}
