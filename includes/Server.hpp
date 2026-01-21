/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:30 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/21 23:55:43 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <sys/select.h>
#include <unistd.h>

class Client;
class Channel;

class Server {
private:
    int port;
    std::string password;
    int serverSocket;
    bool    running;
    std::map<int, Client*> clients;
    std::map<std::string, Channel*> channels;

    int setupSocket();
    void handleSelect();
    int prepareSelectFds(fd_set& readfds);
    void displayIdleAnimation();
    void processReadyClients(fd_set& readfds);
    int gettingSocketReady();
    int setupBind();
    int  setupListen();
    void acceptNewClient();
    void handleClientMessage(int fd);
    Client* getClientByNick(const std::string& nick);
    void executeCommand(Client* client, const std::string& cmd);
    
    public:
    Server(int port, const std::string& password);
    ~Server();
    
    void start();
    void stop();
    void disconnectClient(int fd);
    Channel* getOrCreateChannel(const std::string& name);
    void    removeChannel(const std::string& name);
    std::string toLower(const std::string& str);
    bool    isValidName(const std::string& src) ;
    const std::string& getPassword();
    void    broadcastQuitNotification(Client* client, const std::string& quitMsg);
    std::vector<Channel*>   getClientChannels(Client* client);
};

#endif
