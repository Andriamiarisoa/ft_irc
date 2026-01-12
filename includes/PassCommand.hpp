#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"

class PassCommand : public Command {
public:
    PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PassCommand();
    
    void execute();
};

#endif
