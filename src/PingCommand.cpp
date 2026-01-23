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
    if (params.empty()) {
        sendError(409, "No origin specified");
        return;
    }

    std::string token = params[0];
    std::string pongReply = ":" SERVER_NAME " PONG " SERVER_NAME " :" + token;
    client->sendMessage(pongReply);
}
