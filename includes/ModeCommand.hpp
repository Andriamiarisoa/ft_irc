#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"
#include "Channel.hpp"

class ModeCommand : public Command {
private:
    Channel*    checkErrorModes();
    void        showModes(Channel* channel);
    void        handleModeI(Channel* channel, bool adding, std::string& appliedModes);
    void        handleModeT(Channel* channel, bool adding, std::string& appliedModes);
    void        handleModeK(Channel* channel, bool adding, size_t& paramIndex,
                            std::string& appliedModes, std::string& appliedParams);
    void        handleModeO(Channel* channel, bool adding, size_t& paramIndex,
                            std::string& appliedModes, std::string& appliedParams);
    void        handleModeL(Channel* channel, bool adding, size_t& paramIndex,
                            std::string& appliedModes, std::string& appliedParams);
    void        processModes(Channel* channel);
public:
    ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params);
    ~ModeCommand();
    
    void execute();
};

#endif
