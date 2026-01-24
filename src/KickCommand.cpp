#include "../includes/KickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <iostream>


KickCommand::KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

KickCommand::~KickCommand() {
}

void KickCommand::execute() {
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
    if (params.size() >3) {
        std::cerr << "Warning: too much parameters, KICK only takes 3 params, other params ignored" << std::endl;
    }
    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = client->getNickname(); 
    if (params.size() >= 3) {
        reason = params[2];
        if (!reason.empty() && reason[0] == ':') {
            reason = reason.substr(1);
        }
    }
    if (reason.empty()) {
        reason = client->getNickname();
    }
    if (!server->channelExistOrNot(channelName)) {
        client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
        return;
    }
    Channel* channel = server->getChannel(channelName);

    if (!channel->isMember(client)) {
        client->sendMessage(ERR_NOTONCHANNEL(nick, channelName) + "\r\n");
        return;
    }

    if (!channel->isOperator(client)) {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(nick, channelName) + "\r\n");
        return;
    }
    
    Client* target = server->getClientByNick(targetNick);
    if (target == NULL) {
        client->sendMessage(ERR_NOSUCHNICK(nick, targetNick) + "\r\n");
        return;
    }

    if (!channel->isMember(target)) {
        client->sendMessage(ERR_USERNOTINCHANNEL(nick, targetNick, channelName) + "\r\n");
        return;
    }
    channel->kickMember(client, target, reason);
}
