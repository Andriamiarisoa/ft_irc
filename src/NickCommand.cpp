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
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";

    if (!client->isAuthenticated()) {
        client->sendMessage(ERR_NOTREGISTERED(nick) + "\r\n");
        return true;
    }
    if (params.empty() || params.size() < 1 || params[0].empty()) {
        client->sendMessage(ERR_NONICKNAMEGIVEN(nick) + "\r\n");
        return true;
    }
    if (server->getClientByNick(newNick) != NULL && newNick != currentNick) {
        client->sendMessage(ERR_NICKNAMEINUSE(nick) + "\r\n");
        return true;
    }
    if (newNick.length() > 9) {
        client->sendMessage("nickname should not be more than 9 characters\r\n");
        return true;
    }
    client->setNickname(newNick);
    if (client->getNickname() != newNick) {
        client->sendMessage(ERR_ERRONEUSNICKNAME(nick, newNick) + "\r\n");
        return true;
    }
    return false;
}

void NickCommand::execute() {
    std::string newNick = params.empty() ? "" : params[0];
    std::string currentNick = client->getNickname();

    if (checkErrors()) {
        return ;
    }
    if (client->isRegistered()) {
        std::string msg = client->getPrefix() + " NICK :" + newNick;
        const std::set<Channel*>& channels = client->getChannels();

        client->sendMessage(msg);
        for (std::set<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
            (*it)->broadcast(msg, client);
        }
    }
    if (!client->isRegistered() && !client->getUsername().empty()) {
        std::string serverName = "irc.example.com"; 
        std::string version = "1.0";

        client->registerClient();
        client->sendMessage(RPL_WELCOME(newNick, client->getUsername(), client->getHostname()) + "\r\n");
        client->sendMessage(RPL_YOURHOST(newNick) + "\r\n");
        client->sendMessage(RPL_CREATED(newNick, " just now") + "\r\n");
        client->sendMessage(RPL_MYINFO(newNick) + "\r\n");
    }
}
