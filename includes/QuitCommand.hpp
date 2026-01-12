#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "Command.hpp"

class QuitCommand : public Command {
public:
    QuitCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~QuitCommand();
    
    void execute();
};

#endif
