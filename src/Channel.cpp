
#include <sstream>
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Command.hpp"
#include "../includes/Replies.hpp"


Channel::Channel(const std::string& name, Server* srv) 
    : name(name), userLimit(0), inviteOnly(false), topicRestricted(false), server(srv) {
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
            client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNickname(), name) + "\r\n");
            return;
        }
    }
    this->topic = topic;
    std::string msg = USER_PREFIX(client->getNickname(), 
                                  client->getUsername(), "host") + 
                      " TOPIC " + name + " :" + topic + "\r\n";
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

bool    Channel::getRestriction() const {
    if (this->topicRestricted)
        return (true);
    else
        return (false);
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
        client->sendMessage(ERR_CHANNELISFULL(client->getNickname(), name) + "\r\n");
        return;
    }
    
    members.insert(client);
    client->addToChannel(this);
    
    invitedUsers.erase(client);
    if (getMembers().size() == 1) {
        addOperator(client);
    }
    if (topic.empty()) {
        client->sendMessage(RPL_NOTOPIC(client->getNickname(), name) + "\r\n");
    }
    else {
        client->sendMessage(RPL_TOPIC(client->getNickname(), name, topic) + "\r\n");
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
    client->sendMessage(RPL_NAMREPLY(client->getNickname(), name, memmberList) + "\r\n");
    client->sendMessage(RPL_ENDOFNAMES(client->getNickname(), name) + "\r\n");
}

void Channel::removeMember(Client* client) {
    if (!isMember(client))
        return;
    
    members.erase(client);
    operators.erase(client);      
    invitedUsers.erase(client);
    client->removeFromChannel(this);
    if (members.empty() && server != NULL) {
        server->removeChannel(getName());
        return;
    }
    if (operators.empty() && !members.empty()) {
        Client* newOp = *(members.begin());
        operators.insert(newOp);
        std::string msg = ":" SERVER_NAME " MODE " + name + " +o " + 
                          newOp->getNickname() + "\r\n";
        broadcast(msg, NULL);
    }
}

void Channel::addOperator(Client* target, Client* setter) {
    if (!isMember(target)) {
        return;
    }

    operators.insert(target);
    std::string msg;
    if (setter != NULL) {
        msg = setter->getPrefix() + " MODE " + name + " +o " + 
              target->getNickname() + "\r\n";
    } else {
        msg = ":" SERVER_NAME " MODE " + name + " +o " + 
              target->getNickname() + "\r\n";
    }
    broadcast(msg, NULL);
}

void Channel::removeOperator(Client* target, Client* setter) {
    if (!isOperator(target)) {
        return;
    }
    
    if (operators.size() == 1) {
        std::string notice = ":" SERVER_NAME " NOTICE " + target->getNickname() + 
                           " :Cannot remove last operator from " + name + "\r\n";
        target->sendMessage(notice);
        return;
    }
    
    operators.erase(target);
    
    std::string msg;
    if (setter != NULL) {
        msg = setter->getPrefix() + " MODE " + name + " -o " + 
              target->getNickname() + "\r\n";
    } else {
        msg = ":" SERVER_NAME " MODE " + name + " -o " + 
              target->getNickname() + "\r\n";
    }
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

void Channel::setInviteOnly(bool mode, Client* client) {
    (void)client;
    inviteOnly = mode;
    if (!mode) {
        invitedUsers.clear();
    }
}

void Channel::setTopicRestricted(bool mode, Client* client) {
    (void)client;
    topicRestricted = mode;
}

void Channel::setUserLimit(int limit, Client* client) {
    (void)client;
    if (limit < 0) {
        return;
    }
    userLimit = limit;
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
        kicker->sendMessage(ERR_CHANOPRIVSNEEDED(kicker->getNickname(), name) + "\r\n");
        return;
    }

    if (!isMember(client)) {
        kicker->sendMessage(ERR_USERNOTINCHANNEL(kicker->getNickname(), 
                                                 client->getNickname(), name) + "\r\n");
        return;
    }
    
    std::string kickMsg = USER_PREFIX(kicker->getNickname(), 
                                      kicker->getUsername(), "host") + 
                          " KICK " + name + " " + client->getNickname() + 
                          " :" + reason + "\r\n";
    
    broadcast(kickMsg, NULL);

    members.erase(client);
    operators.erase(client);
    invitedUsers.erase(client);
    client->removeFromChannel(this);
    
    if (members.empty() && server != NULL) {
        server->removeChannel(getName());
        return;
    }
    if (operators.empty() && !members.empty()) {
        Client* newOp = *(members.begin());
        operators.insert(newOp);
        std::string msg = ":" SERVER_NAME " MODE " + name + " +o " + 
                          newOp->getNickname() + "\r\n";
        broadcast(msg, NULL);
    }
}

std::set<Client*> Channel::getMembers() const {
    return (members);
}

int Channel::getUserLimit() const {
    return (userLimit);
}