#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "../includes/Server.hpp"

class Client;

class Channel {
private:
    std::string name;
    std::string topic;
    std::string key;
    int userLimit;
    std::set<Client*> members;
    std::set<Client*> operators;
    std::set<Client*> invitedUsers;
    bool inviteOnly;
    bool topicRestricted;
    Server* server;

public:
    Channel(const std::string& name);
    ~Channel();
    
    std::string getName() const;
    std::string getTopic() const;
    std::set<Client*> getMembers() const;
    
    void setTopic(const std::string& topic, Client* client);
    void setKey(const std::string& key);
    bool hasKey() const;
    bool checkKey(const std::string& key) const;
    
    void addMember(Client* client);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);
    
    bool isOperator(Client* client) const;
    bool isMember(Client* client) const;
    
    void broadcast(const std::string& msg, Client* exclude);
    void setInviteOnly(bool mode);
    void setTopicRestricted(bool mode);
    void setUserLimit(int limit);
    void inviteUser(Client* client);
    bool isInvited(Client* client) const;
    void kickMember(Client* kicker, Client* client, const std::string& reason);
    void clearAllSet();
    bool isChannelInvitOnly() const;
    bool isChannelFull() const;
    size_t getMembersCount() const;
    
};

#endif
