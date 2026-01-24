#include "../includes/MessageParser.hpp"
#include "../includes/Command.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/InviteCommand.hpp"
#include "../includes/JoinCommand.hpp"
#include "../includes/KickCommand.hpp"
#include "../includes/ModeCommand.hpp"
#include "../includes/NickCommand.hpp"
#include "../includes/PartCommand.hpp"
#include "../includes/PassCommand.hpp"
#include "../includes/PrivmsgCommand.hpp"
#include "../includes/QuitCommand.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/UserCommand.hpp"
#include "../includes/NoticeCommand.hpp"
#include "../includes/PingCommand.hpp"
#include "../includes/PongCommand.hpp"
#include "../includes/Replies.hpp"
#include <iostream>
#include <string>
#include <exception>

Command* MessageParser::createCommand(const std::string& cmd, Server* srv, Client* cli,
        const std::vector<std::string>& params)
{
    try{
        if (cmd == "INVITE") {
            return new InviteCommand(srv, cli, params);
        }
        else if (cmd == "JOIN") {
            return new JoinCommand(srv, cli, params);
        }
        else if (cmd == "KICK") {
            return new KickCommand(srv, cli, params);
        }
        else if (cmd == "MODE") {
            return new ModeCommand(srv, cli, params);
        }
        else if (cmd == "NICK") {
            return new NickCommand(srv, cli, params);
        }
        else if (cmd == "PART") {
            return new PartCommand(srv, cli, params);
        }
        else if (cmd == "PASS") {
            return new PassCommand(srv, cli, params);
        }
        else if (cmd == "PRIVMSG") {
            return new PrivmsgCommand(srv, cli, params);
        }
        else if (cmd == "QUIT") {
            return new QuitCommand(srv, cli, params);
        }
        else if (cmd == "TOPIC") {
            return new TopicCommand(srv, cli, params);
        }
        else if (cmd == "USER") {
            return new UserCommand(srv, cli, params);
        }
        else if (cmd == "NOTICE") {
            return new NoticeCommand(srv, cli, params);
        }
        else if (cmd == "PING") {
            return new PingCommand(srv, cli, params);
        }
        else if (cmd == "PONG") {
            return new PongCommand(srv, cli, params);
        }
    }
    catch (std::exception& e) {
        cli->sendMessage(e.what());
        return NULL;
    }
    cli->sendMessage(ERR_UNKNOWNCOMMAND(cli->getNickname(), cmd) );
    return NULL;
}

std::string MessageParser::extractPrefix(const std::string& line) {
    size_t  pos;

    if (line.empty() || line[0] != ':')
        return "";
    pos = line.find(" ");
    if (pos == std::string::npos)
        return line;
    return line.substr(0, pos);
}

std::string MessageParser::removePrefix(const std::string& line) {
    std::string prefix;
    std::string cmdLine;
    size_t      pos;

    cmdLine = line;
    if (line.empty())
        return "";
    if (line[0] == ':')
    {
        prefix = MessageParser::extractPrefix(line);
        pos = prefix.size();
        while (line.size() > pos && line[pos] == ' ')
            ++pos;
        cmdLine = line.substr(pos, line.size() - pos);
    }
    return (cmdLine);
}

std::vector<std::string> MessageParser::splitParams(const std::string& str) {
    std::vector<std::string>    result;
    std::string                 cmdLine;
    size_t                      pos;

    cmdLine = MessageParser::removePrefix(str);
    if (cmdLine.empty())
        return std::vector<std::string>();
    while (!cmdLine.empty() && cmdLine[0] != ':')
    {
        pos = cmdLine.find(" ");
        if (pos == std::string::npos)
            pos = cmdLine.size();
        result.push_back(cmdLine.substr(0, pos));
        while (cmdLine[pos] == ' ')
            ++pos;
        cmdLine = cmdLine.substr(pos, cmdLine.size() - pos);
    }
    if (!cmdLine.empty())
    {
        if (cmdLine.size() > 1 && cmdLine[1] == ' ')
            return std::vector<std::string>();
        else
            result.push_back(cmdLine);
    }
    if (!result.empty()) {
        std::string& last = result.back();
        if (last.size() >= 2 && last[last.size() - 2] == '\r' && last[last.size() - 1] == '\n') {
            last = last.substr(0, last.size() - 2);
        } else if (last.size() >= 1 && last[last.size() - 1] == '\n') {
            last = last.substr(0, last.size() - 1);
        } else if (last.size() >= 1 && last[last.size() - 1] == '\r') {
            last = last.substr(0, last.size() - 1);
        }
    }
    return result;
}

Command* MessageParser::parse(const std::string& line, Server* srv, Client* cli) {
    std::vector<std::string>    params;
    std::string                 cmd;

    params = MessageParser::splitParams(line);
    if (params.empty())
    {
        cli->sendMessage(ERR_UNKNOWNCOMMAND(cli->getNickname(), line));
        return NULL;
    }
    cmd = params.front();
    params.erase(params.begin());
    for (long unsigned int i = 0; i < cmd.size(); i++)
        cmd[i] = std::toupper(cmd[i]);
    return MessageParser::createCommand(cmd, srv, cli, params);
}