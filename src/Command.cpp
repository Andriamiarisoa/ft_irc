#include "../includes/Command.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <sstream>

Command::Command(Server* srv, Client* cli, const std::vector<std::string>& params)
    : server(srv), client(cli), params(params) {
    if (srv == nullptr || cli == nullptr) {
        throw std::invalid_argument("Server or Client pointer is null");
    }
}

Command::~Command() {
}

void Command::sendReply(int code, const std::string& msg) {
    std::ostringstream  oss;
    std::string         nickname;       

    if (client->isRegistered())
        nickname = client->getNickname();
    else
        nickname = "*";
    oss << ":" << "irc.example.com" 
        << " " << code 
        << " " << nickname 
        << " :" << msg 
        << "\r\n";
    client->sendMessage(oss.str());
}

void Command::sendError(int code, const std::string& msg) {
    std::ostringstream  oss;
    std::string         nickname;       

    if (client->isRegistered())
        nickname = client->getNickname();
    else
        nickname = "*";
    oss << ":" << "irc.example.com" 
        << " " << code 
        << " " << nickname 
        << " :" << msg 
        << "\r\n";
    client->sendMessage(oss.str());
}
