#include "../includes/QuitCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Replies.hpp"
#include <vector>

QuitCommand::QuitCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute() {
    std::string quitMsg;

    if (params.empty()) {
        quitMsg = USER_PREFIX(client->getNickname(), 
                              client->getUsername(), "unknown.host") + " QUIT\r\n";
    }
    else {
        std::string msg = params[0];
        if (params[0][0] == ':') {
            msg = params[0].substr(1);
        }
        quitMsg = USER_PREFIX(client->getNickname(), 
                              client->getUsername(), "unknown.host") + " QUIT :" + 
                  msg + "\r\n";
    }    
    
    server->broadcastQuitNotification(client, quitMsg);
    server->disconnectClient(client->getFd());
    return;
}
