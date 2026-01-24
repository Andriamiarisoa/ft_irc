#include "../includes/ModeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <vector>
#include <string>

ModeCommand::ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

ModeCommand::~ModeCommand() {
}

void ModeCommand::execute() {
    if (!client->isRegistered()) {
        sendError(451, ":You have not registered");
        return;
    }
    if (params.empty() || params[0].empty()) {
        sendError(461, "MODE :Not enough parameters");
        return;
    }
    std::string channelName = params[0];
    Channel* channel = server->getChannel(channelName);
    if(channel == NULL) {
        sendError(403, channelName + " :No such channel");
        return;
    }
    if (!channel->isMember(client)) {
        sendError(442, channelName + " :You're not on that channel");
        return;
    }
    if (params.size() == 1) {
        std::string modeStr = "+";
        if (channel->isChannelInvitOnly()) modeStr += "i";
        if (channel->hasKey()) modeStr += "k";
        if (channel->getUserLimit() > 0) modeStr += ("l " + channel->getUserLimit());
        sendReply(324, channelName + " " + modeStr);
        return;
    }
    if (!channel->isOperator(client)) {
        sendError(482, channelName + " :You're not channel operator");
        return;
    }
}
