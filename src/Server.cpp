/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:18 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/19 14:57:54 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <csignal>
#include <iostream>
#include <sys/socket.h>

volatile sig_atomic_t g_running = 1;


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

    if (serverSocket != -1) {
        close (serverSocket);
    }
}

void signalHandler(int signum) {
    (void) signum;
    g_running = 0;
}

void Server::start() {
    setupSocket();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    this->running = true;
    g_running = 1;
    while (running && g_running)
    {
        handleSelect();
    }
    stop();
}

void Server::stop() {
    std::cout << "SERVER SHUTDOWN!" << std::endl;
    
    std::map<int, Client*>::iterator it;
    std::string exitMessage = "ERROR: Server is shutting down\r\n";
    
    for (it = clients.begin()  ; it != clients.end()  ;  it++) {
        int clientFd = it->first;

        int sent = send(clientFd, exitMessage.c_str(), exitMessage.length(), 0);
        if (sent > 0)
            std::cout << "Shutdown message sent succesfully to fd:   " <<  clientFd  << std::endl;
        else
            std::cout << "Shutdown message not sent to fd:   " <<  clientFd  << std::endl;
    }
    for (it = clients.begin() ; it != clients.end() ; it++) {
        close(it->first);
    }

    if (serverSocket != -1)
        close(serverSocket);

    std::cout << "All clients connexion closed succesfully, server has now shut down" << std::endl;
    this->running = false;
}

void Server::setupSocket() {
}

void Server::handleSelect() {
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
