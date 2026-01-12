#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"

class ModeCommand : public Command {
public:
    ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~ModeCommand();
    
    void execute();
};

#endif
