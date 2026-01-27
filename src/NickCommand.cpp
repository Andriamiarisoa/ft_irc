#include "../includes/NickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"

NickCommand::NickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

NickCommand::~NickCommand() {
}

bool NickCommand::checkErrors() {
    std::string newNick = params.empty() ? "" : params[0];
    std::string currentNick = client->getNickname();
    std::string nick = currentNick.empty() ? "*" : currentNick;

    if (!client->isAuthenticated()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return true;
    }
    if (params.empty() || params[0].empty()) {
        client->sendMessage(ERR_NONICKNAMEGIVEN(nick) + "\r\n");
        return true;
    }
    if (server->getClientByNick(newNick) != NULL && newNick != currentNick) {
        client->sendMessage(ERR_NICKNAMEINUSE(newNick) + "\r\n");
        return true;
    }
    if (newNick.length() > 9) {
        client->sendMessage(ERR_ERRONEUSNICKNAME(nick, newNick) + "\r\n");
        return true;
    }
    return false;
}

void NickCommand::execute() {
    std::string newNick = params.empty() ? "" : params[0];
    std::string oldNick = client->getNickname();
    std::string oldPrefix = client->getPrefix();

    if (checkErrors()) {
        return;
    }
    
    client->setNickname(newNick);
    if (client->getNickname() != newNick) {
        std::string nick = oldNick.empty() ? "*" : oldNick;
        client->sendMessage(ERR_ERRONEUSNICKNAME(nick, newNick) + "\r\n");
        return;
    }
    
    if (client->isRegistered()) {
        std::string msg = oldPrefix + " NICK :" + newNick + "\r\n";
        const std::set<Channel*>& channels = client->getChannels();

        client->sendMessage(msg);
        for (std::set<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
            (*it)->broadcast(msg, client);
        }
    }
    else if (!client->getUsername().empty()) {
        client->registerClient();
        client->sendMessage(RPL_WELCOME(newNick, client->getUsername(), client->getHostname()) + "\r\n");
        client->sendMessage(RPL_YOURHOST(newNick) + "\r\n");
        client->sendMessage(RPL_MYINFO(newNick) + "\r\n");
    }
}
