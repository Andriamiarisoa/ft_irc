#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "Command.hpp"
#include "Client.hpp"
#include "Server.hpp"

class PrivmsgCommand : public Command {
private:
    void    executeSingle(const std::string &receptor);
public:
    PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PrivmsgCommand();
    
    void execute();
};

#endif
