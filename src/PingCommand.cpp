#include "../includes/PingCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Replies.hpp"

PingCommand::PingCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

PingCommand::~PingCommand() {
}

void PingCommand::execute() {
    std::string nick = client->getNickname();
    if (nick.empty()) nick = "*";
    if (params.empty()) {
        client->sendMessage(ERR_NOORIGIN(nick) + "\r\n");
        return;
    }

    std::string token = params[0];
    std::string pongReply = ":" SERVER_NAME " PONG " SERVER_NAME " :" + token + "\r\n";
    client->sendMessage(pongReply);
}
