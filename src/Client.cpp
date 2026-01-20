
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>

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
    if (nick.empty() || nick.length() > 9 || !std::isalpha(nick[0])) {
        return;
    }
    // RFC 1459: caractères autorisés après le premier: lettres, chiffres, -, [, ], \, `, ^, {, }
    const std::string specialChars = "-[]\\`^{}";
    for (size_t i = 1; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && specialChars.find(nick[i]) == std::string::npos) {
            return;
        }
    }
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
    if (authenticated && !nickname.empty() && !username.empty()) {
        registered = true;
    }
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
    const size_t MAX_BUFFER_SIZE = 1024;
    
    if (buffer.length() + data.length() > MAX_BUFFER_SIZE) {
        buffer.clear();
        return;
    }
    buffer.append(data);
}

std::string Client::extractCommand() {
    size_t pos = buffer.find("\r\n");
    
    if (pos == std::string::npos) {
        return "";
    }
    
    std::string command = buffer.substr(0, pos);
    buffer.erase(0, pos + 2);
    
    return command;
}

void Client::sendMessage(const std::string& msg) {
    if (msg.empty())
        return;

    std::string toSend = msg;
    
    // S'assurer que le message se termine par "\r\n"
    if (toSend.length() < 2 || toSend.substr(toSend.length() - 2) != "\r\n") {
        toSend += "\r\n";
    }

    size_t totalSent = 0;
    size_t remaining = toSend.length();

    int retryCount = 0;
    const int MAX_RETRIES = 10;
    
    while (totalSent < toSend.length()) {
        ssize_t sent = send(fd, toSend.c_str() + totalSent, remaining, 0);
        
        if (sent < 0) {
            // Gérer les envois partiels (EAGAIN, EWOULDBLOCK)
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (++retryCount >= MAX_RETRIES) {
                    std::cerr << "Error: max retries reached for client " << fd << std::endl;
                    return;
                }
                continue;
            }
            // Journaliser les erreurs mais ne pas lancer d'exceptions
            std::cerr << "Error sending message to client " << fd << ": " << strerror(errno) << std::endl;
            return;
        }
        
        totalSent += sent;
        remaining -= sent;
    }
}
