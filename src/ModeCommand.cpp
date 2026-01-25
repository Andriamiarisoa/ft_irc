#include "../includes/ModeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>
#include <sstream>

ModeCommand::ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

ModeCommand::~ModeCommand() {
}

Channel*    ModeCommand::checkErrorModes() {
    std::string nick = client->getNickname();
    if (nick.empty())
        nick = "*";
    if (!client->isRegistered()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return NULL;
    }
    if (params.size() == 0) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "MODE") + "\r\n");
        return NULL;
    }

    std::string channelName = params[0];
    Channel* channel = server->getChannel(channelName);

    if(channel == NULL) {
        client->sendMessage(ERR_NOSUCHCHANNEL(nick, channelName) + "\r\n");
        return NULL;
    }
    if (!channel->isMember(client)) {
        client->sendMessage(ERR_NOTONCHANNEL(nick, channelName) + "\r\n");
        return NULL;
    }
    return channel;
}

void    ModeCommand::showModes(Channel* channel) {
    std::string modeStr = "+";
    if (channel->isChannelInvitOnly())
        modeStr += "i";
    if (channel->getRestriction())
        modeStr += "t";
    if (channel->hasKey())
        modeStr += "k";
    if (channel->getUserLimit() > 0)
    {
        std::stringstream ss;
        ss << channel->getUserLimit();
        modeStr += ("l " + ss.str());
    }
    client->sendMessage(RPL_CHANNELMODEIS(client->getNickname(), channel->getName(), modeStr, "") + "\r\n");
}

void ModeCommand::handleModeI(Channel* channel, bool adding, std::string& appliedModes) {
    if (adding) {
        channel->setInviteOnly(true);
        appliedModes += "+i";
    }
    else {
        channel->setInviteOnly(false);
        appliedModes += "-i";
    }
}

void ModeCommand::handleModeT(Channel* channel, bool adding, std::string& appliedModes) {
    if (adding) {
        channel->setTopicRestricted(true);
        appliedModes += "+t";
    }
    else {
        channel->setTopicRestricted(false);
        appliedModes += "-t";
    }
}

void ModeCommand::handleModeK(Channel* channel, bool adding, size_t& paramIndex,
                              std::string& appliedModes, std::string& appliedParams) {
    if (adding) {
        if (paramIndex < params.size()) {
            channel->setKey(params[paramIndex]);
            appliedModes += "+k";
            appliedParams += " " + params[paramIndex];
            paramIndex++;
        }
    }
    else {
        channel->setKey("");
        appliedModes += "-k";
    }
}

void ModeCommand::handleModeO(Channel* channel, bool adding, size_t& paramIndex,
                              std::string& appliedModes, std::string& appliedParams) {
    if (paramIndex >= params.size())
        return;

    Client* target = server->getClientByNick(params[paramIndex]);
    if (target && channel->isMember(target)) {
        if (adding) {
            channel->addOperator(target);
            appliedModes += "+o";
        }
        else {
            channel->removeOperator(target);
            appliedModes += "-o";
        }
        appliedParams += " " + params[paramIndex];
    }
    paramIndex++;
}

void ModeCommand::handleModeL(Channel* channel, bool adding, size_t& paramIndex,
                              std::string& appliedModes, std::string& appliedParams) {
    if (adding) {
        if (paramIndex < params.size()) {
            std::istringstream iss(params[paramIndex]);
            int limit;
            iss >> limit;
            if (limit > 0) {
                channel->setUserLimit(limit);
                appliedModes += "+l";
                appliedParams += " " + params[paramIndex];
            }
            paramIndex++;
        }
    }
    else {
        channel->setUserLimit(0);
        appliedModes += "-l";
    }
}

void ModeCommand::processModes(Channel* channel) {
    std::string modeString = params[1];
    size_t paramIndex = 2;
    bool adding = true;
    std::string appliedModes = "";
    std::string appliedParams = "";

    for (size_t i = 0; i < modeString.length(); ++i) {
        char mode = modeString[i];

        if (mode == '+') {
            adding = true;
            continue;
        }
        if (mode == '-') {
            adding = false;
            continue;
        }
        switch (mode) {
            case 'i':
                handleModeI(channel, adding, appliedModes);
                break;
            case 't':
                handleModeT(channel, adding, appliedModes);
                break;
            case 'k':
                handleModeK(channel, adding, paramIndex, appliedModes, appliedParams);
                break;
            case 'o':
                handleModeO(channel, adding, paramIndex, appliedModes, appliedParams);
                break;
            case 'l':
                handleModeL(channel, adding, paramIndex, appliedModes, appliedParams);
                break;
            default:
                client->sendMessage("472 " + client->getNickname() + " " + std::string(1, mode) + " :is unknown mode char to me\r\n");
                break;
        }
    }
    if (!appliedModes.empty()) {
        channel->broadcast(RPL_CHANNELMODEIS(client->getNickname(), channel->getName(), appliedModes, appliedParams), this->client);
    }
}

void ModeCommand::execute() {
    Channel* channel = checkErrorModes();
    if (channel == NULL)
        return;

    if (params.size() == 1) {
        showModes(channel);
        return;
    }
    if (!channel->isOperator(client)) {
        client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNickname(), channel->getName()) + "\r\n");
        return;
    }
    else
        processModes(channel);
}
