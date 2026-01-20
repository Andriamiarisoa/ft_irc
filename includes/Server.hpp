/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 08:20:30 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/22 09:27:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
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
    int gettingSocketReady();
    int setupBind();
    int  setupListen();
    void acceptNewClient();
    void handleClientMessage(int fd);
    void disconnectClient(int fd);
    Channel* getOrCreateChannel(const std::string& name);
    void executeCommand(Client* client, const std::string& cmd);

public:
    Server(int port, const std::string& password);
    ~Server();
    
    void start();
    void stop();
<<<<<<< HEAD
    Client* getClientByNick(const std::string& nick);
=======
    std::string toLower(const std::string& str);
>>>>>>> 0cf348e (feat: function to find the client by their nickname implemented)
};

#endif
