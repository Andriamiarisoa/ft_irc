#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

class TopicCommand : public Command {
public:
    TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~TopicCommand();
    
    void execute();
};

#endif
