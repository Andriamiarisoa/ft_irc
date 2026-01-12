#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class Client;
class Channel;

class Server {
private:
    int port;
    std::string password;
    int serverSocket;
    std::vector<pollfd> fds;
    std::map<int, Client*> clients;
    std::map<std::string, Channel*> channels;

    void setupSocket();
    void handlePoll();
    void acceptNewClient();
    void handleClientMessage(int fd);
    void disconnectClient(int fd);
    Client* getClientByNick(const std::string& nick);
    Channel* getOrCreateChannel(const std::string& name);
    void executeCommand(Client* client, const std::string& cmd);

public:
    Server(int port, const std::string& password);
    ~Server();
    
    void start();
    void stop();
};

#endif
