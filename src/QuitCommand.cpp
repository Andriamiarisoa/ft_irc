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
    server->broadcastQuitNotification(client, quitMsg);
    std::vector<Channel*> channels = server->getClientChannels(client);






    server->disconnectClient(client->getFd());

}
