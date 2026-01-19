/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:18 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/19 09:50:10 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"


Server::Server(int port, const std::string &password) : port(port), password(password), serverSocket(-1) {
    this->running = false;
}
Server::~Server() {
    std::map <int, Client*>::iterator it;
    std::map <std::string, Channel*>::iterator itc;
    
    for (it = clients.begin() ; it != clients.end() ; it++) {
        close (it->first);
        delete (it->second);
    }
    clients.clear();

    for (itc = channels.begin()  ; itc != channels.end()  ; itc++) {
        delete (itc->second);
    }
    channels.clear();

    fds.clear();

    if (serverSocket != -1) {
        close (serverSocket);
    }
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
