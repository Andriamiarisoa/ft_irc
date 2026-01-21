#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Channel;

class Client {
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string hostname;
    bool authenticated;
    bool registered;
    std::string buffer;
    std::set<Channel*> channels;

public:
    Client(int fd);
    ~Client();
    
    int getFd() const;
    std::string getRealname() const;
    std::string getHostname() const;    
    std::string getNickname() const;
    std::string getUsername() const;
    std::set<Channel*> getChannels() const;
    bool isAuthenticated() const;
    bool isRegistered() const;
    
    void setRealname(const std::string& real);
    void setHostname(const std::string& host);
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void authenticate();
    void registerClient();
    void addToChannel(Channel* channel);
    void removeFromChannel(Channel* channel);
    void appendToBuffer(const std::string& data);
    std::string extractCommand();
    void sendMessage(const std::string& msg);
};

#endif
