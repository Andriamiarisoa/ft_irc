#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "Command.hpp"

class PartCommand : public Command {
public:
    PartCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PartCommand();
    
    void execute();
};

#endif
