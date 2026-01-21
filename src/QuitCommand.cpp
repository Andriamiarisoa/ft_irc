#include "../includes/QuitCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include <vector>

QuitCommand::QuitCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

QuitCommand::~QuitCommand() {
}

void QuitCommand::execute() {
    std::string quitMsg;

    if (params.empty()) {
        quitMsg = ":" + client->getNickname() + "!" + 
                      client->getUsername() + "@host QUIT" + "\r\n";
    }
    else {
        quitMsg = ":" + client->getNickname() + "!" + 
                      client->getUsername() + "@host QUIT :" + 
                      params[0] + "\r\n";
    }    
    std::vector<Channel*> channels = server->getClientChannels(client);    
    server->broadcastQuitNotification(client, quitMsg);
    
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* channel = *it;
        if (channel->getMembers().empty()) {
            server->removeChannel(channel->getName());
        }
    }
    server->disconnectClient(client->getFd());
}
