#include "../includes/JoinCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
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
    if (!client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (params.empty() || params[0].empty()) {
        sendError(461, "JOIN :Not enough parameters");
        return;
    }
    if (params[0] == "0") {
        std::set<Channel*> clientChannels = client->getChannels();
        for (std::set<Channel*>::iterator it = clientChannels.begin(); it != clientChannels.end(); ++it) {
            Channel* channel = *it;
            channel->removeMember(client);
            std::string partMsg = client->getPrefix() + " PART " + channel->getName() + "\r\n";
            client->sendMessage(partMsg);
            channel->broadcast(partMsg, client);
        }
        return;
    }

    std::vector<std::string> channelsToJoin = split(params[0], ',');
    std::vector<std::string> channelKeys = split(params.size() > 1 ? params[1] : "", ',');

    for (size_t i = 0; i < channelsToJoin.size(); i++) {
        std::string channelName = channelsToJoin[i];
        if (!server->isValidName(channelName)) {
            sendError(403, channelName + " :No such channel");
            continue;
        }
        bool isNewChannel = !server->channelExistOrNot(channelName);
        Channel* channel = server->getOrCreateChannel(channelName);
        if(channel == NULL) {
            sendError(403, channelName + " :No such channel");
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
                    sendError(475, channelName + " :Cannot join channel (+k) - incorrect key");
                    continue;
                }
            }
            if (channel->isChannelInvitOnly() && channel->isInvited(client) == false) {
                sendError(473, channelName + " :Cannot join channel (+i) - invite only");
                continue;
            }
            if (channel->isChannelFull()) {
                sendError(471, channelName + " :Cannot join channel (+l) - channel is full");
                continue;
            }
        }
        channel->addMember(client);
        std::string joinMsg = client->getPrefix() + " JOIN " + channelName + "\r\n";
        client->sendMessage(joinMsg);
        channel->broadcast(joinMsg, client);
    }
}
