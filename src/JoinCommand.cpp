#include "../includes/JoinCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>

JoinCommand::JoinCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

JoinCommand::~JoinCommand() {
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

void JoinCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";
    if (!client->isRegistered()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return;
    }
    if (params.empty() || params[0].empty()) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "JOIN") + "\r\n");
        return;
    }
    std::vector<std::string> channelsToJoin = split(params[0], ',');
    std::vector<std::string> channelKeys = split(params.size() > 1 ? params[1] : "", ',');

    for (size_t i = 0; i < channelsToJoin.size(); i++) {
        std::string channelName = channelsToJoin[i];
        if (!server->isValidName(channelName)) {
            client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
            continue;
        }
        bool isNewChannel = !server->channelExistOrNot(channelName);
        Channel* channel = server->getOrCreateChannel(channelName);
        if(channel == NULL) {
            client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
            continue;
        }
        if (isNewChannel) {
            channel->addOperator(client);
        } else {
            if (channel->isMember(client)) {
                continue;
            }
            if (channel->hasKey()) {
                std::string providedKey = (i < channelKeys.size()) ? channelKeys[i] : "";
                if (!channel->checkKey(providedKey)) {
                    client->sendMessage(ERR_BADCHANNELKEY(nick, channelName) + "\r\n");
                    continue;
                }
            }
            if (channel->isChannelInvitOnly() && channel->isInvited(client) == false) {
                client->sendMessage(ERR_INVITEONLYCHAN(nick, channelName) + "\r\n");
                continue;
            }
            if (channel->isChannelFull()) {
                client->sendMessage(ERR_CHANNELISFULL(nick, channelName) + "\r\n");
                continue;
            }
        }
        channel->addMember(client);
        std::string joinMsg = client->getPrefix() + " JOIN " + channelName + "\r\n";
        client->sendMessage(joinMsg);
        channel->broadcast(joinMsg, client);
    }
}
