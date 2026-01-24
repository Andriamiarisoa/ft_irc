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
                              client->getUsername(), "host") + " QUIT\r\n";
    }
    else {
        quitMsg = USER_PREFIX(client->getNickname(), 
                              client->getUsername(), "host") + " QUIT :" + 
                  params[0] + "\r\n";
    }    
    std::vector<std::string> channelNames;
    std::vector<Channel*> channels = server->getClientChannels(client);
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        channelNames.push_back((*it)->getName());
    }
    
    server->broadcastQuitNotification(client, quitMsg);
    
    for (size_t i = 0; i < channelNames.size(); i++) {
        Channel* channel = server->getChannel(channelNames[i]);
        if (channel && channel->getMembers().empty()) {
            server->removeChannel(channelNames[i]);
        }
    }
    server->disconnectClient(client->getFd());
    return;
}
