#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <vector>

class UserCommand : public Command {
public:
    UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~UserCommand();
    
    void execute();
};

#endif
