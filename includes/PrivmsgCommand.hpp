#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "Command.hpp"

class PrivmsgCommand : public Command {
public:
    PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PrivmsgCommand();
    
    void execute();
};

#endif
