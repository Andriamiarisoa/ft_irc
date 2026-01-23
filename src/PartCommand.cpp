#include "../includes/PartCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
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
    if (!client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (params.empty() || params[0].empty() || params.size() > 2) {
        sendError(461, "PART :Not enough parameters");
        return;
    }
    std::vector<std::string> channelsToPart = split(params[0], ',');
    std::string partReason = (params.size() > 1) ? params[1] : "";
    for (size_t i = 0; i < channelsToPart.size(); i++) {
        Channel* channel = server->getChannel(channelsToPart[i]);
        if (!channel) {
            sendError(403, channelsToPart[i] + " :No such channel");
            continue;
        }
        if (!channel->isMember(client)) {
            sendError(442, channelsToPart[i] + " :You're not on that channel");
            continue;
        }
        std::string partMsg = client->getPrefix() + " PART " + channelsToPart[i];
        if (!partReason.empty()) {
            if (partReason[0] == ':') {
                partMsg += " " + partReason;
            } else {
                partMsg += " :" + partReason;
            }
        }
        partMsg += "\r\n";
        channel->broadcast(partMsg, client);
        channel->removeMember(client);
        if (channel->getMembers().empty()) {
            server->removeChannel(channelsToPart[i]);
        }
    }
}