/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:18 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/21 22:40:04 by herrakot         ###   ########.fr       */
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
#include "../includes/Command.hpp"

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
    std::cout << std::endl;
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║          SERVER SHUTTING DOWN          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    std::map<int, Client*>::iterator it;
    std::string exitMessage = "ERROR: Server is shutting down\r\n";
    
    for (it = clients.begin()  ; it != clients.end()  ;  it++) {
        int clientFd = it->first;

        int sent = send(clientFd, exitMessage.c_str(), exitMessage.length(), 0);
        if (sent > 0)
            std::cout << "  [OK] Shutdown message sent to FD: " << clientFd << std::endl;
        else
            std::cout << "  [!!] Failed to send shutdown to FD: " << clientFd << std::endl;
    }
    sleep (1);
    for (it = clients.begin() ; it != clients.end() ; it++) {
        close(it->first);
    }

    if (serverSocket != -1)
        close(serverSocket);

    std::cout << "\n  All connections closed. Server stopped." << std::endl;
    std::cout << "════════════════════════════════════════════\n" << std::endl;
    this->running = false;
}

int Server::gettingSocketReady() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "[ERROR] Failed to create socket" << std::endl;
        return (1);
    }
    std::cout << "  [OK] Socket created (FD: " << serverSocket << ")" << std::endl;
    return (0);
}

int Server::setupBind() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int bindResult = bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr));
    if (bindResult == -1) {
        std::cerr << "[ERROR] Failed to bind to port " << this->port << std::endl;
        std::cerr << "        Port may already be in use" << std::endl;
        close (serverSocket);
        return (1);
    }
    std::cout << "  [OK] Bound to port " << this->port << std::endl; 
    return (0);
}

int Server::setupListen() {
    int listenResult = listen(serverSocket, 10);
    if (listenResult == -1) {
        std::cerr << "[ERROR] Failed to listen on port " << this->port << std::endl;
        close (serverSocket);
        return (1);
    }
    std::cout << "  [OK] Listening (backlog: 10)" << std::endl;
    return (0);
}

int Server::setupSocket() {
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║         FT_IRC SERVER STARTING         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
    
    if (this->port < 1 || this->port > 65535) {
        std::cerr << "[ERROR] Invalid port: " << this->port << std::endl;
        std::cerr << "        Must be between 1 and 65535" << std::endl;
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
    
    std::cout << "\n════════════════════════════════════════════" << std::endl;
    std::cout << "  Server ready on port " << this->port << std::endl;
    std::cout << "  Press Ctrl+C to stop" << std::endl;
    std::cout << "════════════════════════════════════════════\n" << std::endl;
    return (0);
}

int Server::prepareSelectFds(fd_set& readfds) {
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    
    int maxFd = serverSocket;
    std::map<int, Client*>::iterator it;
    for (it = clients.begin() ; it != clients.end() ; it++) {
        int fd = it->first;
        FD_SET(fd, &readfds);
        if (fd > maxFd)
            maxFd = fd;
    }
    return maxFd;
}

void Server::displayIdleAnimation() {
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
}

void Server::processReadyClients(fd_set& readfds) {
    if (FD_ISSET(serverSocket, &readfds))
        acceptNewClient();
    
    std::vector<int> readyFds;
    std::map<int, Client*>::iterator it;
    for (it = clients.begin() ; it != clients.end() ; it++) {
        int fd = it->first;
        if (FD_ISSET(fd, &readfds))
            readyFds.push_back(fd);
    }
    
    for (size_t i = 0; i < readyFds.size(); i++) {
        int fd = readyFds[i];
        if (clients.find(fd) != clients.end())
            handleClientMessage(fd);
    }
}

void Server::handleSelect() {
    fd_set readfds;
    int maxFd = prepareSelectFds(readfds);
    
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int selectResult = select(maxFd + 1, &readfds, NULL, NULL, &timeout);

    if (selectResult == -1) {
        if (errno == EINTR)
            return;
        std::cerr << "\n[ERROR] Select failed unexpectedly, shutting down" << std::endl;
        running = false;
        return;
    }
    if (selectResult == 0) {
        displayIdleAnimation();
        return;
    }
    processReadyClients(readfds);
}

void Server::acceptNewClient() {
    struct  sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1) {
        std::cerr << "  [!!] Failed to accept new client" << std::endl;
        return;
    }
    
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    
    int clientPort = ntohs(clientAddr.sin_port);
    
    int flags = fcntl(clientFd, F_GETFL, 0);
    fcntl (clientFd, F_SETFL, flags | O_NONBLOCK);
    
    Client* newClient = new Client(clientFd);
    clients[clientFd] = newClient;
    std::cout << "\n  [+] New client connected" << std::endl;
    std::cout << "      FD: " << clientFd << " | IP: " << clientIP << ":" << clientPort << std::endl;

    std::string welcome = ":Server ft_irc :Welcome to the IRC Server\r\n";
    send (clientFd, welcome.c_str(), welcome.length(), 0);
}

void Server::handleClientMessage(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end())  {
        std::cerr << "  [!!] Client FD " << fd << " not found" << std::endl;
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
        std::cerr << "  [!!] Failed to receive from FD " << fd << std::endl;
        disconnectClient(fd);
        return;
    }
    buffer[byteReceived] = '\0';
    client->appendToBuffer(std::string(buffer, byteReceived));

    std::string command;
    while ((command = client->extractCommand()) != "") {
        std::cout << "  [CMD] " << command << std::endl;
        executeCommand(client, command);
    }
}

void Server::disconnectClient(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end()) {
        std::cerr << "  [!!] Cannot disconnect: FD " << fd << " not found" << std::endl;
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
    std::cout << "  [-] Client disconnected (FD: " << fd << ", Nick: " << nickname << ")" << std::endl;
}

std::string Server::toLower(const std::string& str) {
    std::string result = str;
    for (size_t i = 0 ; i < result.length() ; i++) {
        result[i] = std::tolower(result[i]);
    }
    return (result);
}

Client* Server::getClientByNick(const std::string& nick) {
    std::map<int, Client*>::iterator it;
    std::string lowerNick = toLower(nick);
    for (it = clients.begin() ; it != clients.end() ; it++) {
        Client* client = it->second;
        if (toLower(client->getNickname()) == lowerNick) {
            return (client);
        }
    }
    return (NULL);
}

bool    Server::isValidName(const std::string& src) {
    if (src.length() < 2 || src.length() > 50) {
        std::cerr << "Error: the channel name must be between 2 and 50 character" << std::endl;
        return (false);
    }
    if (src[0] != '#' && src[0] != '&') {
        std::cerr << "Error: the channel name must with # or &" << std::endl;
        return (false);
    }
    for (size_t i = 0 ; i < src.length() ; i++) {
        if (src[i] == ' ' || src[i] == ',' || src[i] == '\x07' || (src[i] >= 0 && src[i] <= 31)) {
            std::cerr << "Error: the channel name must not contain character such as: [space: ], [control character: ASCII 0-31], [bell character: \x07 ], [comma: ,]" << std::endl;
            return (false);
        }
    }
    return (true);
}

Channel* Server::getOrCreateChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it;
    std::string lowerName = toLower(name);
    for (it = channels.begin() ; it != channels.end() ; it++) {
        Channel* channel = it->second;
        if (toLower(channel->getName()) ==  lowerName) {
            return (channel);
        }
    }
    if (isValidName(lowerName) == false)  {
        return (NULL);
    }
    Channel* newChannel = new Channel(name);
    channels[lowerName] = newChannel;
    return (newChannel);
}


//TO_DO : implement this func, function to actually call the execute() 

void Server::executeCommand(Client* client, const std::string& cmd) {
    (void)client;
    (void)cmd;
}

void    Server::broadcastQuitNotification(Client* client, const std::string& quitMsg) {
    std::map<std::string, Channel*>::iterator it;
    for (it = channels.begin() ; it != channels.end() ; it++) {
        Channel* channel = it->second;
        if (channel->isMember(client)) {
            channel->broadcast(quitMsg, NULL);
            channel->removeMember(client);
        }
    }
}

std::vector<Channel*>   Server::getClientChannels(Client* client) {
    std::vector<Channel*> clientChannels;
    std::map<std::string, Channel*>::iterator it;

    for (it = channels.begin() ; it != channels.end() ; it++) {
        Channel* channel = it->second;
        if (channel->isMember(client)) {
            clientChannels.push_back(channel);
        }
    }
    return (clientChannels);
}

void Server::removeChannel(const std::string& name) {
    std::string lowerName = toLower(name);
    std::map<std::string, Channel*>::iterator it = channels.find(lowerName);
    
    if (it != channels.end()) {
        delete it->second;
        channels.erase(it);
        std::cout << "  [-] Channel removed: " << name << std::endl;
    }
}


