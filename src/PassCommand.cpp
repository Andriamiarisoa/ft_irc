#include "../includes/PassCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <iostream>

PassCommand::PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PassCommand::~PassCommand() {
}

void PassCommand::execute() {
    if (params.size() == 0) {
        sendError(461, "PASS :Not enough parameters");
        return;
    }

    if (params.size() > 0 && params[0].empty()) {
        std::cerr << "Error: empty password not authorized" <<std::endl;
        return;
    }
    
    if (client->isRegistered()) {
        sendError(462, ":You may not register");
        return;
    }
    
    if (params[0] == server->getPassword()) {
        std::cout << "DEBUG: authenticate called succesfully" << std::endl;
        client->authenticate();
        return;
    }
    else {
        sendError(464, ":Password incorrect");
        return;
    }
}
