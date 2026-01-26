#include "../includes/PartCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>

PartCommand::PartCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PartCommand::~PartCommand() {
}

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

void PartCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";
    if (!client->isRegistered()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return;
    }
    if (params.empty() || params[0].empty() || params.size() > 2) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "PART") + "\r\n");
        return;
    }
    std::vector<std::string> channelsToPart = split(params[0], ',');
    std::string partReason = (params.size() > 1) ? params[1] : "";
    for (size_t i = 0; i < channelsToPart.size(); i++) {
        std::string channelName = channelsToPart[i];
        Channel* channel = server->getChannel(channelName);
        if (!channel) {
            client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
            continue;
        }
        if (!channel->isMember(client)) {
            client->sendMessage(ERR_NOTONCHANNEL(nick, channelName) + "\r\n");
            continue;
        }
        std::string partMsg = client->getPrefix() + " PART " + channelName;
        if (!partReason.empty()) {
            if (partReason[0] == ':') {
                partMsg += " " + partReason;
            } else {
                partMsg += " :" + partReason;
            }
        }
        partMsg += "\r\n";
        channel->broadcast(partMsg, client);
        client->sendMessage(partMsg);
        channel->removeMember(client);
        // Note: removeMember() already handles removing empty channels internally
    }
}