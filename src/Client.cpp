#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Client::Client(int fd) 
    : fd(fd), authenticated(false), registered(false) {
}

Client::~Client() {
}

int Client::getFd() const {
    return fd;
}

std::string Client::getNickname() const {
    return nickname;
}

std::string Client::getUsername() const {
    return username;
}

bool Client::isAuthenticated() const {
    return authenticated;
}

bool Client::isRegistered() const {
    return registered;
}

void Client::setNickname(const std::string& nick) {
    (void)nick;
}

void Client::setUsername(const std::string& user) {
    (void)user;
}

void Client::authenticate() {
}

void Client::addToChannel(Channel* channel) {
    (void)channel;
}

void Client::removeFromChannel(Channel* channel) {
    (void)channel;
}

void Client::appendToBuffer(const std::string& data) {
    (void)data;
}

std::string Client::extractCommand() {
    return "";
}

void Client::sendMessage(const std::string& msg) {
    (void)msg;
}
