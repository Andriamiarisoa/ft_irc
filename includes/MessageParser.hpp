#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include <string>
#include <vector>

class Server;
class Client;
class Command;

class MessageParser {
private:
    static std::vector<std::string> splitParams(const std::string& str);
    static std::string extractPrefix(const std::string& line);
    static std::string removePrefix(const std::string& line);
    static Command* createCommand(const std::string& cmd, Server* srv, Client* cli,
        const std::vector<std::string>& params);
public:
    static Command* parse(const std::string& line, Server* srv, Client* cli);
};

#endif
