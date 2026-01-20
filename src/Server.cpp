/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:18 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/20 12:51:03 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <bits/types/struct_timeval.h>
#include <csignal>
#include <cerrno>
#include <ctime>
#include <vector>
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
    sleep (1);
    for (it = clients.begin() ; it != clients.end() ; it++) {
        close(it->first);
    }

    if (serverSocket != -1)
        close(serverSocket);

    std::cout << "All clients connexion closed succesfully, server has now shut down" << std::endl;
    this->running = false;
}

int Server::gettingSocketReady() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: failed to create socket, program is stopping" << std::endl;
        return (1);
    }
    std::cout << "Socket created succesfully. FD: " << serverSocket << std::endl;
    return (0);
}

int Server::setupBind() {
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
    std::cout << "Socket bound to port: " << this->port << std::endl; 
    return (0);
}

int Server::setupListen() {
    int listenResult = listen(serverSocket, 10);
    if (listenResult == -1) {
        std::cerr << "Error: failed to listen to port: " << this->port << std::endl;
        close (serverSocket);
        return (1);
    }
    std::cout << "Server ready to listen on port: " << this->port << std::endl;
    return (0);
}

int Server::setupSocket() {
    if (this->port < 1 || this->port > 65535) {
        std::cerr << "Error: Invalid port: " << this->port << std::endl;
        std::cerr << "Port must be between 1 and 65535" << std::endl;
        return (1);
    }

    if (gettingSocketReady() == 1)
        return (1);
   
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int flags = fcntl(serverSocket, F_GETFL, 0);
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    if (setupBind() == 1)
        return (1);

    if (setupListen() == 1)
        return (1);
    std::cout << "\n=== SERVER SETUP COMPLETE ===" << std::endl;
    std::cout << "Server listening on port: " << this->port << std::endl;
    std::cout << "Socket fd: " << serverSocket << std::endl;
    std::cout << "Ready to accept connections" << std::endl << std::endl;
    return (0);
}

void Server::handleSelect() {
    fd_set readfds;
    FD_ZERO(&readfds);

    FD_SET(serverSocket, &readfds);
    std::map<int, Client*>::iterator it;
    for (it = clients.begin() ; it != clients.end() ; it++) {
        int fd = it->first;
        FD_SET(fd, &readfds);
    }
    int maxFd = serverSocket;
    for (it = clients.begin() ; it != clients.end() ; it++) {
        int fd = it->first;
        if (fd > maxFd)
            maxFd = fd;
    }
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int selectResult = select(maxFd + 1, &readfds, NULL, NULL, &timeout);

    if (selectResult == -1) {
        if (errno == EINTR)
            return;
        std::cerr << "Error: select had an unexcepted behavior, quitting the server" << std::endl;
        running = false;
        return;
    }
    else if (selectResult == 0) {
        static int animFrame = 0;
        static time_t lastTime = 0;
        time_t now = time(NULL);
        
        const char* spinner[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        const int spinnerSize = 10;
        
        if (now != lastTime) {
            std::cout << "\r\033[K" << spinner[animFrame % spinnerSize] 
                      << " Waiting for connections... [" << clients.size() 
                      << " client(s) connected]" << std::flush;
            animFrame++;
            lastTime = now;
        }
        return;
    }
    else {
        if (FD_ISSET(serverSocket, &readfds)) 
        {
            acceptNewClient();
        }
        std::vector<int> readyFds;
        for (it = clients.begin() ; it != clients.end() ; it++) {
            int fd = it->first;
            if (FD_ISSET(fd, &readfds)) {
                readyFds.push_back(fd);
            }
        }
        for (size_t i = 0; i < readyFds.size(); i++) {
            int fd = readyFds[i];
            if (clients.find(fd) != clients.end()) {
                handleClientMessage(fd);
            }
        }        
    }
}

void Server::acceptNewClient() {
    struct  sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1) {
        std::cerr << "Error: failed to accept new client" << std::endl;
        return;
    }
    
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    
    int clientPort = ntohs(clientAddr.sin_port);
    
    int flags = fcntl(clientFd, F_GETFL, 0);
    fcntl (clientFd, F_SETFL, flags | O_NONBLOCK);
    
    Client* newClient = new Client(clientFd);
    clients[clientFd] = newClient;
    std::cout << std::endl;
    std::cout << "New client connected succesfully, FD: " << clientFd << " from: " << clientIP << " : " << clientPort << std::endl;

    std::string welcome = ":Server ft_ic : welcome tho the IRC Server\r\n";
    send (clientFd, welcome.c_str(), welcome.length(), 0);
}

void Server::handleClientMessage(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end())  {
        std::cerr << "Client from FD: " << fd << " not found in map" << std::endl;
        return;
    }
    Client* client = it->second;
    char buffer[512];
    int byteReceived = recv(fd, buffer, sizeof(buffer) -  1, 0);
    
    if (byteReceived == 0) {
        disconnectClient(fd);
        return;
    }
    if (byteReceived == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        std::cerr << "Error: failed to receive the message from  client FD: " << fd << std::endl;
        disconnectClient(fd);
        return;
    }
    buffer[byteReceived] = '\0';
    client->appendToBuffer(std::string(buffer, byteReceived));

    std::string command;
    while ((command = client->extractCommand()) != "") {
        std::cout << "command found ready to be executed : " << command << std::endl;
        executeCommand(client, command);
    }
}

void Server::disconnectClient(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end()) {
        std::cerr << "Error: Client not found in the map" << std::endl;
        return;
    }
    Client* client = it->second;
    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "Unkown";

    std::string quitMess = ": " + nickname + " QUIT :Client disconnected\r\n";

    std::map<std::string, Channel*>::iterator itc;
    for (itc = channels.begin() ; itc != channels.end() ; itc++) {
        Channel* channel = itc->second;
        if (channel->isMember(client)) {
            channel->broadcast(quitMess, client);
            channel->removeMember(client);
        }
    }
    close (fd);
    delete client;
    clients.erase(it);
    std::cout << "Client with FD: " << fd << " , [nickname] : " << nickname << " disconnected" << std::endl;
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
