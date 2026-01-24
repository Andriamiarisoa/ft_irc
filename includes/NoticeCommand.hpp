#ifndef NOTICECOMMAND_HPP
#define NOTICECOMMAND_HPP

#include "Command.hpp"

class NoticeCommand : public Command {
public:
    NoticeCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~NoticeCommand();
    
    void execute();
};

#endif
