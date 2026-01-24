#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

class PingCommand : public Command {
public:
    PingCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PingCommand();
    
    void execute();
};

#endif
