
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Client::Client(int fd) 
    : fd(fd), authenticated(false), registered(false) {
}

Client::~Client() {
    channels.clear();
    buffer.clear();
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
    // TODO: Validate nickname according to IRC rules
    this->nickname = nick;
    // TODO: Check if already registered and send appropriate response to Channel
    if (authenticated && !nickname.empty() && !username.empty()) {
        registered = true;
    }
}

void Client::setUsername(const std::string& user) {
    this->username = user;
    if (authenticated && !nickname.empty() && !username.empty()) {
        registered = true;
        // TODO: Send welcome message to client
    }
}

void Client::authenticate() {
    this->authenticated = true;
}

void Client::addToChannel(Channel* channel) {
    if (!channel)
        return;
    channels.insert(channel);
}

void Client::removeFromChannel(Channel* channel) {
    if (!channel || channels.find(channel) == channels.end())
        return;
    channels.erase(channel);
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
