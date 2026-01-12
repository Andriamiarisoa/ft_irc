#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "Command.hpp"

class JoinCommand : public Command {
public:
    JoinCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~JoinCommand();
    
    void execute();
};

#endif
