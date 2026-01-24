#include "../includes/Command.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <sstream>

Command::Command(Server* srv, Client* cli, const std::vector<std::string>& params)
    : server(srv), client(cli), params(params) {
    if (srv == NULL || cli == NULL) {
        throw std::invalid_argument("Server or Client pointer is null");
    }
}

Command::~Command() {
}

std::string Command::formatCode(int code) {
    std::ostringstream oss;
    if (code < 10)
        oss << "00" << code;
    else if (code < 100)
        oss << "0" << code;
    else
        oss << code;
    return oss.str();
}

std::string Command::getClientNick() {
    if (client->isRegistered() && !client->getNickname().empty())
        return client->getNickname();
    return "*";
}
