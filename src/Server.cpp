#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Server::Server(int port, const std::string& password) 
    : port(port), password(password), serverSocket(-1) {
    (void)this->port;
    (void)this->serverSocket;
}

Server::~Server() {
}

void Server::start() {
}

void Server::stop() {
}

void Server::setupSocket() {
}

void Server::handlePoll() {
}

void Server::acceptNewClient() {
}

void Server::handleClientMessage(int fd) {
    (void)fd;
}

void Server::disconnectClient(int fd) {
    (void)fd;
}

Client* Server::getClientByNick(const std::string& nick) {
    (void)nick;
    return NULL;
}

Channel* Server::getOrCreateChannel(const std::string& name) {
    (void)name;
    return NULL;
}

void Server::executeCommand(Client* client, const std::string& cmd) {
    (void)client;
    (void)cmd;
}
