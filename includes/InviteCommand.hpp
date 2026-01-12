#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "Command.hpp"

class InviteCommand : public Command {
public:
    InviteCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~InviteCommand();
    
    void execute();
};

#endif
