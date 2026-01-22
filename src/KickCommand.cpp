#include "../includes/KickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"


KickCommand::KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params) {
}

KickCommand::~KickCommand() {
}

void KickCommand::execute() {
    if (!client->isRegistered()) {
        sendError(451, ": You have not registered");
        return;
    }

    if (params.size() < 2) {
        sendError(461, "KICK :Not enough parameters");
        return;
    }

    std::string channelName = params[0];
    std::string targetname = params[1];
    if (params.size() == 3) {
        std::string reason = params[2];
    }

    Channel* channel = server.getChanne

}
