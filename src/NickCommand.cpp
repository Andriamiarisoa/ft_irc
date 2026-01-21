#include "../includes/NickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"

NickCommand::NickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

NickCommand::~NickCommand() {
}

void NickCommand::execute() {
    std::string newNick = params.empty() ? "" : params[0];
    std::string currentNick = client->getNickname();

    if (!client->isAuthenticated()) {
        sendError(464, ":You must send PASS first");
        return;
    }
    if (params.empty()) {
        sendError(431, ":No nickname given");
        return;
    }
    //TODO: Vérifier si le surnom est déjà utilisé par un autre client
    client->setNickname(newNick);
    if (client->getNickname() != newNick) {
        sendError(432, newNick + " :Erroneous nickname");
        return;
    }
    if (client->isRegistered()) {
        std::string msg = ":" + currentNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick;
        client->sendMessage(msg);
        
        // Broadcast to all channels the client is a member of
        const std::set<Channel*>& channels = client->getChannels();
        for (std::set<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
            (*it)->broadcast(msg, client);
        }
    }
    if (!client->isRegistered() && !client->getUsername().empty()) {
        std::string serverName = "irc.example.com"; 
        std::string version = "1.0";

        client->registerClient();
        sendReply(001, ":Welcome to the IRC Network " + client->getNickname() + "!" +
            client->getUsername() + "@" + client->getHostname());
        sendReply(002, ":Your host is " + serverName +
            ", running version " + version);
        sendReply(003, ":This server was created just now");
        sendReply(004, serverName + " " + version +
            " o o");
    }
}
