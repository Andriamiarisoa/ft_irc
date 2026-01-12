#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "Command.hpp"

class NickCommand : public Command {
public:
    NickCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~NickCommand();
    
    void execute();
};

#endif
