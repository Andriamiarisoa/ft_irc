/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:18 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/19 17:20:26 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <csignal>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 

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
    int errorFlag = setupSocket();

    if (errorFlag == 1)
        return;
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

int Server::setupSocket() {

    if (this->port < 1 || this->port > 65535) {
        std::cerr << "Error: Invalid port: " << this->port << std::endl;
        std::cerr << "Port must be between 1 and 65535" << std::endl;
        return (1);
    }
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: failed to create socket, program is stopping" << std::endl;
        return (1);
    }
    std::cout << "Socket created succesfully. FD: " << serverSocket << std::endl;
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int flags = fcntl(serverSocket, F_GETFL, 0);
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int bindResult = bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr));

    if (bindResult == -1) {
        std::cerr << "Error: failed to bind to port: " << this->port << std::endl;
        std::cerr << "Error: port might be already used" << std::endl;
        close (serverSocket);
        return (1);
    }
    std::cout << "Socket boumd to port: " << this->port << std::endl; 

    int listenResult = listen(serverSocket, 10);
    
    if (listenResult == -1) {
        std::cerr << "Error: failed to listen to port: " << this->port << std::endl;
        close (serverSocket);
        return (1);
    }
    std::cout << "Server ready to listen on port: " << this->port << std::endl;

    std::cout << "\n=== SERVER SETUP COMPLETE ===" << std::endl;
    std::cout << "Server listening on port: " << this->port << std::endl;
    std::cout << "Socket fd: " << serverSocket << std::endl;
    std::cout << "Ready to accept connections" << std::endl << std::endl;

    return (0);
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
