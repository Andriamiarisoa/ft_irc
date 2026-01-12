#include "../includes/MessageParser.hpp"
#include "../includes/Command.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

Command* MessageParser::parse(const std::string& line, Server* srv, Client* cli) {
    (void)line;
    (void)srv;
    (void)cli;
    return NULL;
}

std::vector<std::string> MessageParser::splitParams(const std::string& str) {
    (void)str;
    std::vector<std::string> result;
    return result;
}

std::string MessageParser::extractPrefix(const std::string& line) {
    (void)line;
    return "";
}
