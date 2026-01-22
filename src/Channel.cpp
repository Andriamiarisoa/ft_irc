
#include <sstream>
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Command.hpp"


Channel::Channel(const std::string& name) 
    : name(name), userLimit(0), inviteOnly(false), topicRestricted(true) {
        this->topic = "";
        this->key = "";
}

void Channel::clearAllSet() {
    members.clear();
    operators.clear();
    invitedUsers.clear();
    topic.clear();
    key.clear();
}

Channel::~Channel() {
    clearAllSet();    
}

std::string Channel::getName() const {
    return name;
}

std::string Channel::getTopic() const {
    return topic;
}

void Channel::setTopic(const std::string& topic, Client* client) {
    if(topicRestricted == true) {
        if (!isOperator(client)) {
            std::string error = ":server 482 " + client->getNickname() + 
                               " " + name + " :You're not channel operator\r\n";
            client->sendMessage(error);
            return;
        }
    }
    this->topic = topic;
    std::string msg = ":" + client->getNickname() + " TOPIC " + name + " :" + topic + "\r\n";
    broadcast(msg, NULL);
}

void Channel::setKey(const std::string& key) {
    this->key = key;
}

bool Channel::hasKey() const {
    if (this->key.empty())
        return (false);
    else
        return (true);
}

bool Channel::checkKey(const std::string& key) const {
    if (this->key == key)
        return (true);
    else if (!hasKey())
        return (true);
    else
        return (false);
}

void Channel::addMember(Client* client) {
    if (client == NULL)
        return;
    
    if (isChannelFull()) {
        std::string error = ":server 471 " + client->getNickname() + 
                           " " + name + " :Cannot join channel (+l)\r\n";
        client->sendMessage(error);
        return;
    }
    
    members.insert(client);
    client->addToChannel(this);
    
    invitedUsers.erase(client);
    if (getMembers().size() == 1) {
        addOperator(client);
    }
    std::string joinMsg = ":" + client->getNickname() + "!" + 
                         client->getUsername() + "@host JOIN " + name + "\r\n";
    
    broadcast(joinMsg, NULL);
    if (topic.empty()) {
       std::string noTopicMsg = ":server 331 " + client->getNickname() + 
            " " + name + " :No topic is set\r\n";
        client->sendMessage(noTopicMsg);
    }
    else {
        std::string topicMsg = ":server 332 " + client->getNickname() + 
            " " + name + " :" + topic + "\r\n";
        client->sendMessage(topicMsg);
    }
    std::string memmberList = "";
    std::set<Client*>::iterator it;
    for (it = members.begin() ; it != members.end() ; it++) {
        if (!memmberList.empty())
            memmberList += " ";
        if (operators.find(*it) != operators.end()) {
            memmberList += "@";
        }
        memmberList += (*it)->getNickname();
    }
    std::string namesMsg = ":server 353 " + client->getNickname() + 
                          " = " + name + " :" + memmberList + "\r\n";
    client->sendMessage(namesMsg);
    
    std::string endMsg = ":server 366 " + client->getNickname() + 
                        " " + name + " :End of /NAMES list\r\n";
    client->sendMessage(endMsg);
}

void Channel::removeMember(Client* client) {
    if (!isMember(client))
        return;
    std::string partMsg = ":" + client->getNickname() + "!" + 
                     client->getUsername() + "@host PART " + name + "\r\n";

    broadcast(partMsg, NULL);
    members.erase(client);
    operators.erase(client);      // erase does nothing if not found
    invitedUsers.erase(client);   // erase does nothing if not found
    client->removeFromChannel(this);
    if (members.empty()) {
        server->removeChannel(getName());
    }
}

void Channel::addOperator(Client* client) {
    if (!isMember(client)) {
        std::string error = ":server 441 " + client->getNickname() + 
                           " " + name + " :They aren't on that channel\r\n";
        client->sendMessage(error);
        return;
    }

    operators.insert(client);
    std::string msg = ":server MODE " + name + " +o " + 
                      client->getNickname() + "\r\n";

    broadcast(msg, NULL);
}

void Channel::removeOperator(Client* client) {
    if (operators.size() == 1) {
        std::string notice = ":server NOTICE " + client->getNickname() + 
                           " :Cannot remove last operator from " + name + "\r\n";
        client->sendMessage(notice);
        return;
    }
    if (isOperator(client)) {
        operators.erase(client);
    }
    std::string msg = ":server MODE " + name + " -o " + 
             client->getNickname() + "\r\n";

    broadcast(msg, NULL);
}

bool Channel::isOperator(Client* client) const {
    if (operators.find(client) != operators.end())
        return (true);
    return (false);
}

bool Channel::isMember(Client* client) const {
    if (members.find(client) != members.end())
        return (true);
    return (false);
}

void Channel::broadcast(const std::string& msg, Client* exclude) {
    std::set<Client*>::iterator it;

    for (it = members.begin() ; it != members.end() ; it++) {
        Client* client = (*it);
        if (client == NULL || client == exclude)
            continue;
        int fd = client->getFd();
        if (fd < 0) {
            continue;
        }
        client->sendMessage(msg);
    }
}

void Channel::setInviteOnly(bool mode) {
    inviteOnly = mode;

    if (!mode) {
        invitedUsers.clear();
    }
    std::string modeStr = mode ? "+i" : "-i";
    std::string msg = ":server MODE " + name + " " + modeStr + "\r\n";
    broadcast (msg, NULL);
}

void Channel::setTopicRestricted(bool mode) {
    topicRestricted = mode;

    std::string modeStr = mode ? "+t" : "-t";
    std::string msg = ":server MODE " + name + " " + modeStr + "\r\n";
    broadcast (msg, NULL);
}

void Channel::setUserLimit(int limit) {
    if (limit < 0) {
        return;
    }
    userLimit = limit;
    std::string msg;
    if (limit == 0) {
        msg = ":server MODE " + name + " -l\r\n";
    }
    else {
        std::stringstream ss;
        ss << limit;
        msg = ":server MODE " + name + " +l " + ss.str() + "\r\n"; 
    }
    broadcast(msg, NULL);
}

bool    Channel::isChannelInvitOnly() const {
    return (inviteOnly);
}

bool Channel::isChannelFull() const {
    if (userLimit == 0)
        return (false);
    if (members.size() >= (size_t)userLimit)
        return (true);
    else
        return (false);
}

size_t Channel::getMembersCount() const {
    return (members.size());
}

void Channel::inviteUser(Client* client) {
    if (client == NULL)
        return;

    if (!inviteOnly)
        return;
    invitedUsers.insert(client);
}

bool Channel::isInvited(Client* client) const {
    std::set<Client*>::iterator it = invitedUsers.find(client);

    if (it != invitedUsers.end()) {
        return (true);
    }
    return (false);
}

void Channel::kickMember(Client* kicker, Client* client, const std::string& reason) {
    if (kicker == NULL || client == NULL) {
        return;
    }

    if (!isOperator(kicker)) {
        std::string error = ":server 482 " + kicker->getNickname() + 
           " " + name + " :You're not channel operator\r\n";
        kicker->sendMessage(error);
        return;
    }

    // if (!isMember(client)) {
    //     std::string error = 
    // }
}

std::set<Client*> Channel::getMembers() const {
    return (members);
}

