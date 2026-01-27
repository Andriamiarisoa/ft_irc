#include "../includes/ModeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <cctype>

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
    channel->setInviteOnly(adding, client);
    appliedModes += adding ? "+i" : "-i";
    std::string msg = client->getPrefix() + " MODE " + channel->getName() + " " + (adding ? "+i" : "-i") + "\r\n";
    channel->broadcast(msg, NULL);
}

void ModeCommand::handleModeT(Channel* channel, bool adding, std::string& appliedModes) {
    channel->setTopicRestricted(adding, client);
    appliedModes += adding ? "+t" : "-t";
    std::string msg = client->getPrefix() + " MODE " + channel->getName() + " " + (adding ? "+t" : "-t") + "\r\n";
    channel->broadcast(msg, NULL);
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
        else {
            client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +k") + "\r\n");
        }
    }
    else {
        channel->setKey("");
        appliedModes += "-k";
    }
    std::string msg;
    if (appliedModes == "+k") {
        msg = client->getPrefix() + " MODE " + channel->getName() + " " + appliedModes + appliedParams + "\r\n";
    }
    else if (appliedModes == "-k") {
        msg = client->getPrefix() + " MODE " + channel->getName() + " " + appliedModes + "\r\n";
    }
    channel->broadcast(msg, NULL);
}

void ModeCommand::handleModeO(Channel* channel, bool adding, size_t& paramIndex,
                              std::string& appliedModes, std::string& appliedParams) {
    if (paramIndex >= params.size()) {
        client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +o") + "\r\n");
        return;
    }

    std::string targetNick = params[paramIndex];
    Client* target = server->getClientByNick(targetNick);
    
    if (target == NULL) {
        client->sendMessage(ERR_NOSUCHNICK(client->getNickname(), targetNick) + "\r\n");
        paramIndex++;
        return;
    }
    if (!channel->isMember(target)) {
        client->sendMessage(ERR_USERNOTINCHANNEL(client->getNickname(), targetNick, channel->getName()) + "\r\n");
        paramIndex++;
        return;
    }
    
    if (adding) {
        channel->addOperator(target, client);
        appliedModes += "+o";
    }
    else {
        channel->removeOperator(target, client);
        appliedModes += "-o";
    }
    appliedParams += " " + targetNick;
    paramIndex++;
}

void ModeCommand::handleModeL(Channel* channel, bool adding, size_t& paramIndex,
                              std::string& appliedModes, std::string& appliedParams) {
    if (adding) {
        if (paramIndex < params.size()) {
            std::string limitStr = params[paramIndex];
            bool isValid = !limitStr.empty();
            for (size_t i = 0; i < limitStr.length() && isValid; ++i) {
                if (!isdigit(limitStr[i])) {
                    isValid = false;
                }
            }
            if (!isValid || limitStr.length() > 9) {
                client->sendMessage(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), "l", limitStr, "Invalid limit value (must be a positive number)") + "\r\n");
                paramIndex++;
                return;
            }
            std::istringstream iss(limitStr);
            long limit;
            iss >> limit;
            if (limit <= 0 || limit > 2147483647) {
                client->sendMessage(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), "l", limitStr, "Limit must be a positive number") + "\r\n");
                paramIndex++;
                return;
            }
            channel->setUserLimit(static_cast<int>(limit), client);
            appliedModes += "+l";
            appliedParams += " " + params[paramIndex];
            std::stringstream ss;
            ss << limit;
            std::string msg = client->getPrefix() + " MODE " + channel->getName() + " +l " + ss.str() + "\r\n";
            channel->broadcast(msg, NULL);
            paramIndex++;
        }
        else {
            client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +l") + "\r\n");
        }
    }
    else {
        channel->setUserLimit(0, client);
        appliedModes += "-l";
        std::string msg = client->getPrefix() + " MODE " + channel->getName() + " -l\r\n";
        channel->broadcast(msg, NULL);
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
                client->sendMessage(ERR_UNKNOWNMODE(client->getNickname(), std::string(1, mode)) + "\r\n");
                break;
        }
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
