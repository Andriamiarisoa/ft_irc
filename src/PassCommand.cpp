#include "../includes/PassCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Replies.hpp"
#include <iostream>

PassCommand::PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PassCommand::~PassCommand() {
}

void PassCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";
    
    if (params.size() == 0) {
        client->sendMessage(ERR_NEEDMOREPARAMS(nick, "PASS") + "\r\n");
        return;
    }

    if (params.size() > 0 && params[0].empty()) {
        std::cerr << "Error: empty password not authorized" <<std::endl;
        return;
    }
    
    if (client->isRegistered()) {
        client->sendMessage(ERR_ALREADYREGISTERED(nick) + "\r\n");
        return;
    }
    
    if (params[0] == server->getPassword()) {
        client->authenticate();
        return;
    }
    else {
        if (client->isAuthenticated())
            client->unauthenticate();
        client->sendMessage(ERR_PASSWDMISMATCH(nick) + "\r\n");
        return;
    }
}
