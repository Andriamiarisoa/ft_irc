#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

Channel::Channel(const std::string& name) 
    : name(name), userLimit(0), inviteOnly(false), topicRestricted(false) {
    (void)this->userLimit;
    (void)this->inviteOnly;
    (void)this->topicRestricted;
}

Channel::~Channel() {
}

std::string Channel::getName() const {
    return name;
}

std::string Channel::getTopic() const {
    return topic;
}

void Channel::setTopic(const std::string& topic, Client* client) {
    (void)topic;
    (void)client;
}

void Channel::setKey(const std::string& key) {
    (void)key;
}

bool Channel::hasKey() const {
    return false;
}

bool Channel::checkKey(const std::string& key) const {
    (void)key;
    return false;
}

void Channel::addMember(Client* client) {
    (void)client;
}

void Channel::removeMember(Client* client) {
    (void)client;
}

void Channel::addOperator(Client* client) {
    (void)client;
}

void Channel::removeOperator(Client* client) {
    (void)client;
}

bool Channel::isOperator(Client* client) const {
    (void)client;
    return false;
}

bool Channel::isMember(Client* client) const {
    (void)client;
    return false;
}

void Channel::broadcast(const std::string& msg, Client* exclude) {
    (void)msg;
    (void)exclude;
}

void Channel::setInviteOnly(bool mode) {
    (void)mode;
}

void Channel::setTopicRestricted(bool mode) {
    (void)mode;
}

void Channel::setUserLimit(int limit) {
    (void)limit;
}

void Channel::inviteUser(Client* client) {
    (void)client;
}

bool Channel::isInvited(Client* client) const {
    (void)client;
    return false;
}

void Channel::kickMember(Client* client, const std::string& reason) {
    (void)client;
    (void)reason;
}

std::set<Client*> Channel::getMembers() const {
    return (members);
}

