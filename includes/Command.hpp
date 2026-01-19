#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include <stdexcept>

class Server;
class Client;

class Command {
protected:
    Server* server;
    Client* client;
    std::vector<std::string> params;
    
    void sendReply(int code, const std::string& msg);
    void sendError(int code, const std::string& msg);

public:
    Command(Server* srv, Client* cli, const std::vector<std::string>& params);
    virtual ~Command();
    
    virtual void execute() = 0;
};

#endif
