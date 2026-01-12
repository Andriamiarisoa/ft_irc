#include "../includes/Command.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

Command::Command(Server* srv, Client* cli, const std::vector<std::string>& params)
    : server(srv), client(cli), params(params) {
}

Command::~Command() {
}

void Command::sendReply(int code, const std::string& msg) {
    (void)code;
    (void)msg;
}

void Command::sendError(int code, const std::string& msg) {
    (void)code;
    (void)msg;
}
