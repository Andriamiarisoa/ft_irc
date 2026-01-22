#ifndef PONGCOMMAND_HPP
#define PONGCOMMAND_HPP

#include "Command.hpp"

class PongCommand : public Command {
public:
    PongCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~PongCommand();
    
    void execute();
};

#endif
