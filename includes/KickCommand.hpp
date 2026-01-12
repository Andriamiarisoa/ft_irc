#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "Command.hpp"

class KickCommand : public Command {
public:
    KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~KickCommand();
    
    void execute();
};

#endif
