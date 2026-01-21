/**
 * @file test_usercommand_standalone.cpp
 * @brief Standalone test suite for UserCommand class
 * 
 * This file contains mock implementations of Server, Client, and Command
 * so that UserCommand can be tested independently without requiring
 * the full implementation of other classes.
 * 
 * Compile with:
 *   g++ -Wall -Wextra -Werror -std=c++98 tests/test_usercommand_standalone.cpp -o test_usercommand
 * 
 * Run with:
 *   ./test_usercommand
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

// ============================================================================
// TEST MACROS AND UTILITIES
// ============================================================================

#define TEST(name) std::cout << "Testing " << name << "... "
#define PASS() std::cout << "\033[32mPASS\033[0m" << std::endl; passCount++
#define FAIL(msg) std::cout << "\033[31mFAIL: " << msg << "\033[0m" << std::endl; failCount++
#define SECTION(name) std::cout << "\n--- " << name << " ---\n" << std::endl

static int passCount = 0;
static int failCount = 0;

// C++98 compatible int to string conversion
static std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// ============================================================================
// MOCK CLASSES - These replace the real implementations for testing
// ============================================================================

class Server;
class Client;
class Channel;

/**
 * @brief Mock Server class for testing
 */
class Server {
private:
    int port;
    std::string password;

public:
    Server(int p, const std::string& pass) : port(p), password(pass) {}
    ~Server() {}
    
    int getPort() const { return port; }
    const std::string& getPassword() const { return password; }
};

/**
 * @brief Mock Client class for testing
 */
class Client {
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string hostname;
    bool authenticated;
    bool registered;

public:
    Client(int socketFd) : fd(socketFd), authenticated(false), registered(false) {}
    ~Client() {}
    
    int getFd() const { return fd; }
    std::string getNickname() const { return nickname; }
    std::string getUsername() const { return username; }
    std::string getRealname() const { return realname; }
    std::string getHostname() const { return hostname; }
    bool isAuthenticated() const { return authenticated; }
    bool isRegistered() const { return registered; }
    
    void setNickname(const std::string& nick) { nickname = nick; }
    void setUsername(const std::string& user) { username = user; }
    void setRealname(const std::string& real) { realname = real; }
    void setHostname(const std::string& host) { hostname = host; }
    void authenticate() { authenticated = true; }
    void registerClient() { registered = true; }
    
    void sendMessage(const std::string& msg) {
        send(fd, msg.c_str(), msg.length(), 0);
    }
};

/**
 * @brief Mock Command base class for testing
 */
class Command {
protected:
    Server* server;
    Client* client;
    std::vector<std::string> params;
    
    std::string formatCode(int code) {
        std::ostringstream oss;
        if (code < 10)
            oss << "00" << code;
        else if (code < 100)
            oss << "0" << code;
        else
            oss << code;
        return oss.str();
    }
    
    std::string getClientNick() {
        if (client->isRegistered() && !client->getNickname().empty())
            return client->getNickname();
        return "*";
    }
    
    std::string getNickname() {
        return client->getNickname();
    }
    
    void sendReply(int code, const std::string& msg) {
        std::ostringstream oss;
        std::string hostname = "irc.example.com";
        oss << ":" << hostname 
            << " " << formatCode(code) 
            << " " << getClientNick() 
            << " " << msg 
            << "\r\n";
        client->sendMessage(oss.str());
    }
    
    void sendError(int code, const std::string& msg) {
        std::ostringstream oss;
        std::string hostname = "irc.example.com";
        oss << ":" << hostname 
            << " " << formatCode(code) 
            << " " << getClientNick() 
            << " :" << msg 
            << "\r\n";
        client->sendMessage(oss.str());
    }

public:
    Command(Server* srv, Client* cli, const std::vector<std::string>& p)
        : server(srv), client(cli), params(p) {
        if (srv == NULL || cli == NULL) {
            throw std::invalid_argument("Server or Client pointer is null");
        }
    }
    
    virtual ~Command() {}
    virtual void execute() = 0;
};

// ============================================================================
// USERCOMMAND CLASS - Copy of the actual implementation
// ============================================================================

class UserCommand : public Command {
public:
    UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params) {
    }

    ~UserCommand() {
    }

    void execute() {
        if (!this->client->isAuthenticated()) {
            sendError(464, "USER :You must send PASS first");
            return;
        }
        if (this->params.size() < 4) {
            sendError(461, "USER :Not enough parameters");
            return;
        }
        if (this->client->isRegistered()) {
            sendError(462, "USER:You may not reregister");
            return;
        }
        this->client->setUsername(this->params[0]);
        this->client->setRealname(this->params[3]);
        if (!this->client->getNickname().empty() && !this->client->getUsername().empty()) {
            this->client->registerClient();
            sendReply(001, ":Welcome to the IRC Network " + getNickname());
        }
    }
};

// ============================================================================
// TEST UTILITIES
// ============================================================================

int createTestSocketPair(int& readFd, int& writeFd) {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        return -1;
    }
    readFd = fds[0];
    writeFd = fds[1];
    return 0;
}

std::string readFromSocket(int fd, size_t maxLen = 1024) {
    char buffer[1025];
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(fd, buffer, maxLen, MSG_DONTWAIT);
    if (n > 0) {
        return std::string(buffer, n);
    }
    return "";
}

void clearSocket(int fd) {
    char buffer[1024];
    while (recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT) > 0) {}
}

bool containsString(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int countOccurrences(const std::string& haystack, const std::string& needle) {
    int count = 0;
    size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        count++;
        pos += needle.length();
    }
    return count;
}

std::vector<std::string> makeParams(const std::string& p1 = "", 
                                     const std::string& p2 = "",
                                     const std::string& p3 = "", 
                                     const std::string& p4 = "") {
    std::vector<std::string> params;
    if (!p1.empty()) params.push_back(p1);
    if (!p2.empty()) params.push_back(p2);
    if (!p3.empty()) params.push_back(p3);
    if (!p4.empty()) params.push_back(p4);
    return params;
}

// ============================================================================
// CONSTRUCTOR TESTS
// ============================================================================

void test_constructor_valid() {
    TEST("Constructor with valid parameters");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_constructor_null_server() {
    TEST("Constructor with NULL server throws exception");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Client client(writeFd);
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    bool exceptionThrown = false;
    try {
        UserCommand cmd(NULL, &client, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    close(readFd);
    close(writeFd);
    
    if (exceptionThrown) {
        PASS();
    } else {
        FAIL("Expected std::invalid_argument exception");
    }
}

void test_constructor_null_client() {
    TEST("Constructor with NULL client throws exception");
    
    Server server(6667, "password");
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    bool exceptionThrown = false;
    try {
        UserCommand cmd(&server, NULL, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    if (exceptionThrown) {
        PASS();
    } else {
        FAIL("Expected std::invalid_argument exception");
    }
}

void test_constructor_null_both() {
    TEST("Constructor with NULL server AND client throws exception");
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    bool exceptionThrown = false;
    try {
        UserCommand cmd(NULL, NULL, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    if (exceptionThrown) {
        PASS();
    } else {
        FAIL("Expected std::invalid_argument exception");
    }
}

void test_constructor_empty_params() {
    TEST("Constructor with empty params (validation in execute)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_constructor_many_params() {
    TEST("Constructor with many parameters");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    for (int i = 0; i < 100; i++) {
        params.push_back("param");
    }
    
    UserCommand cmd(&server, &client, params);
    
    close(readFd);
    close(writeFd);
    PASS();
}

// ============================================================================
// AUTHENTICATION TESTS
// ============================================================================

void test_user_without_pass() {
    TEST("USER without PASS first returns error 464");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "464")) {
        PASS();
    } else {
        FAIL("Expected error 464, got: " + response);
    }
}

void test_user_without_pass_message_content() {
    TEST("USER without PASS error message mentions PASS");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "PASS") || containsString(response, "pass")) {
        PASS();
    } else {
        FAIL("Error message should mention PASS, got: " + response);
    }
}

void test_user_with_pass() {
    TEST("USER after PASS proceeds to parameter validation");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (!containsString(response, "464")) {
        PASS();
    } else {
        FAIL("Should not get error 464 when authenticated");
    }
}

void test_user_unauthenticated_with_nick() {
    TEST("USER unauthenticated even with NICK set returns 464");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.setNickname("testnick");  // NICK set but no PASS
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "464")) {
        PASS();
    } else {
        FAIL("Expected error 464, got: " + response);
    }
}

void test_user_unauthenticated_no_params() {
    TEST("USER unauthenticated with no params still returns 464 first");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    
    std::vector<std::string> params;  // No params
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    // Should get 464 (auth error) not 461 (param error)
    if (containsString(response, "464") && !containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected only error 464, got: " + response);
    }
}

// ============================================================================
// PARAMETER VALIDATION TESTS
// ============================================================================

void test_user_no_params() {
    TEST("USER with no parameters returns error 461");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected error 461, got: " + response);
    }
}

void test_user_one_param() {
    TEST("USER with 1 parameter returns error 461");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("username");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected error 461, got: " + response);
    }
}

void test_user_two_params() {
    TEST("USER with 2 parameters returns error 461");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected error 461, got: " + response);
    }
}

void test_user_three_params() {
    TEST("USER with 3 parameters returns error 461");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected error 461, got: " + response);
    }
}

void test_user_four_params() {
    TEST("USER with 4 parameters is accepted");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (!containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Should not get error 461 with 4 parameters");
    }
}

void test_user_five_params() {
    TEST("USER with 5 parameters is accepted");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("username");
    params.push_back("0");
    params.push_back("*");
    params.push_back("Real Name");
    params.push_back("extra");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (!containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Should not get error 461 with 5 parameters");
    }
}

void test_user_ten_params() {
    TEST("USER with 10 parameters is accepted");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("username");
    params.push_back("0");
    params.push_back("*");
    params.push_back("Real Name");
    for (int i = 0; i < 6; i++) {
        params.push_back("extra");
    }
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (!containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Should not get error 461 with 10 parameters");
    }
}

void test_user_insufficient_params_message() {
    TEST("Insufficient params error mentions USER");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "USER") || containsString(response, "parameter")) {
        PASS();
    } else {
        FAIL("Error message should mention USER or parameters, got: " + response);
    }
}

void test_user_params_not_modified_on_error() {
    TEST("Client state not modified when params insufficient");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::string originalUsername = client.getUsername();
    std::string originalRealname = client.getRealname();
    
    std::vector<std::string> params = makeParams("newuser", "0");  // Only 2 params
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == originalUsername && client.getRealname() == originalRealname) {
        PASS();
    } else {
        FAIL("Client state should not change on param error");
    }
}

// ============================================================================
// RE-REGISTRATION TESTS
// ============================================================================

void test_user_already_registered() {
    TEST("USER when already registered returns error 462");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    client.setUsername("testuser");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "462")) {
        PASS();
    } else {
        FAIL("Expected error 462, got: " + response);
    }
}

void test_user_already_registered_message() {
    TEST("Already registered error message is appropriate");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    client.setUsername("testuser");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "reregister") || containsString(response, "already")) {
        PASS();
    } else {
        FAIL("Error message should mention re-registration, got: " + response);
    }
}

void test_user_does_not_change_when_registered() {
    TEST("Username not changed when already registered");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    client.setUsername("originaluser");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "originaluser") {
        PASS();
    } else {
        FAIL("Username should not change, got: " + client.getUsername());
    }
}

void test_user_realname_not_changed_when_registered() {
    TEST("Realname not changed when already registered");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    client.setUsername("testuser");
    client.setRealname("Original Real Name");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "Original Real Name") {
        PASS();
    } else {
        FAIL("Realname should not change, got: " + client.getRealname());
    }
}

void test_user_registered_uses_nick_in_error() {
    TEST("Error 462 uses nickname in response when registered");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("registereduser");
    client.setUsername("testuser");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "registereduser")) {
        PASS();
    } else {
        FAIL("Error should contain registered nickname, got: " + response);
    }
}

// ============================================================================
// USERNAME SETTING TESTS
// ============================================================================

void test_user_sets_username() {
    TEST("USER sets username on client");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("testusername", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "testusername") {
        PASS();
    } else {
        FAIL("Username should be 'testusername', got: " + client.getUsername());
    }
}

void test_user_username_special_chars() {
    TEST("Username with special characters (underscore, dash)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("test_user-name", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "test_user-name") {
        PASS();
    } else {
        FAIL("Username should be 'test_user-name', got: " + client.getUsername());
    }
}

void test_user_username_with_numbers() {
    TEST("Username with numbers");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("user123", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "user123") {
        PASS();
    } else {
        FAIL("Username should be 'user123', got: " + client.getUsername());
    }
}

void test_user_username_all_numbers() {
    TEST("Username with only numbers");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("12345", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "12345") {
        PASS();
    } else {
        FAIL("Username should be '12345', got: " + client.getUsername());
    }
}

void test_user_username_single_char() {
    TEST("Username with single character");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("x", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "x") {
        PASS();
    } else {
        FAIL("Username should be 'x', got: " + client.getUsername());
    }
}

void test_user_username_uppercase() {
    TEST("Username with uppercase letters");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("TestUser", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "TestUser") {
        PASS();
    } else {
        FAIL("Username should be 'TestUser', got: " + client.getUsername());
    }
}

void test_user_username_mixed_case() {
    TEST("Username with mixed case");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("TeSt_UsEr123", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "TeSt_UsEr123") {
        PASS();
    } else {
        FAIL("Username should be 'TeSt_UsEr123', got: " + client.getUsername());
    }
}

// ============================================================================
// REALNAME SETTING TESTS
// ============================================================================

void test_user_sets_realname() {
    TEST("USER sets realname on client");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John Doe");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John Doe") {
        PASS();
    } else {
        FAIL("Realname should be 'John Doe', got: " + client.getRealname());
    }
}

void test_user_realname_with_spaces() {
    TEST("Realname with multiple spaces");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John Michael Doe Jr");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John Michael Doe Jr") {
        PASS();
    } else {
        FAIL("Realname should preserve spaces, got: " + client.getRealname());
    }
}

void test_user_realname_with_special_chars() {
    TEST("Realname with special characters");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John (The Great) Doe");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John (The Great) Doe") {
        PASS();
    } else {
        FAIL("Realname should preserve special chars, got: " + client.getRealname());
    }
}

void test_user_realname_with_colon() {
    TEST("Realname containing colon");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John: The Great");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John: The Great") {
        PASS();
    } else {
        FAIL("Realname should preserve colon, got: " + client.getRealname());
    }
}

void test_user_realname_with_numbers() {
    TEST("Realname containing numbers");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John Doe 3rd");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John Doe 3rd") {
        PASS();
    } else {
        FAIL("Realname should preserve numbers, got: " + client.getRealname());
    }
}

void test_user_realname_unicode_like() {
    TEST("Realname with accented characters simulation");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Jose Garcia");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "Jose Garcia") {
        PASS();
    } else {
        FAIL("Realname should be preserved, got: " + client.getRealname());
    }
}

void test_user_realname_symbols() {
    TEST("Realname with various symbols");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "John <Doe> & Jane!");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "John <Doe> & Jane!") {
        PASS();
    } else {
        FAIL("Realname should preserve symbols, got: " + client.getRealname());
    }
}

// ============================================================================
// REGISTRATION COMPLETION TESTS
// ============================================================================

void test_registration_pass_nick_user() {
    TEST("Registration: PASS -> NICK -> USER completes registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.isRegistered()) {
        PASS();
    } else {
        FAIL("Client should be registered after PASS, NICK, USER");
    }
}

void test_registration_pass_user_only() {
    TEST("Registration: PASS -> USER does not complete registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    
    client.authenticate();
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (!client.isRegistered()) {
        PASS();
    } else {
        FAIL("Client should NOT be registered without NICK");
    }
}

void test_user_sets_username_without_nick() {
    TEST("USER sets username even without NICK");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "testuser") {
        PASS();
    } else {
        FAIL("Username should be set, got: " + client.getUsername());
    }
}

void test_user_sets_realname_without_nick() {
    TEST("USER sets realname even without NICK");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname() == "Test Real Name") {
        PASS();
    } else {
        FAIL("Realname should be set, got: " + client.getRealname());
    }
}

void test_user_empty_nick_no_registration() {
    TEST("Empty nickname does not trigger registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("");  // Empty nickname
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (!client.isRegistered()) {
        PASS();
    } else {
        FAIL("Client should NOT be registered with empty nickname");
    }
}

// ============================================================================
// WELCOME MESSAGE TESTS
// ============================================================================

void test_welcome_001_sent() {
    TEST("RPL_WELCOME (001) sent on registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "001")) {
        PASS();
    } else {
        FAIL("Expected 001 in response, got: " + response);
    }
}

void test_welcome_contains_welcome_text() {
    TEST("Welcome message contains 'Welcome'");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "Welcome") || containsString(response, "welcome")) {
        PASS();
    } else {
        FAIL("Expected 'Welcome' in response, got: " + response);
    }
}

void test_welcome_contains_nickname() {
    TEST("Welcome message contains nickname");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "testnick")) {
        PASS();
    } else {
        FAIL("Expected 'testnick' in response, got: " + response);
    }
}

void test_welcome_contains_irc_network() {
    TEST("Welcome message mentions IRC Network");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "IRC") || containsString(response, "Network") || containsString(response, "irc")) {
        PASS();
    } else {
        FAIL("Expected IRC/Network mention in response, got: " + response);
    }
}

void test_no_welcome_without_nick() {
    TEST("No welcome message sent without NICK");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (!containsString(response, "001") && !containsString(response, "Welcome")) {
        PASS();
    } else {
        FAIL("Should not send welcome without NICK, got: " + response);
    }
}

void test_response_ends_with_crlf() {
    TEST("Response ends with CRLF");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (response.length() >= 2 && 
        response[response.length() - 2] == '\r' && 
        response[response.length() - 1] == '\n') {
        PASS();
    } else {
        FAIL("Response should end with CRLF");
    }
}

void test_welcome_single_response() {
    TEST("Only one response sent on successful registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    int crlfCount = countOccurrences(response, "\r\n");
    if (crlfCount == 1) {
        PASS();
    } else {
        FAIL("Expected exactly 1 response, got " + intToString(crlfCount));
    }
}

void test_welcome_starts_with_colon() {
    TEST("Welcome response starts with colon (server prefix)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("testuser", "0", "*", "Test User");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (response.length() > 0 && response[0] == ':') {
        PASS();
    } else {
        FAIL("Response should start with colon, got: " + response);
    }
}

// ============================================================================
// ERROR PRECEDENCE TESTS
// ============================================================================

void test_auth_checked_before_params() {
    TEST("Authentication checked before parameter count");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "464")) {
        PASS();
    } else {
        FAIL("Expected error 464 first, got: " + response);
    }
}

void test_params_checked_before_registration() {
    TEST("Parameters checked before registration status");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "461")) {
        PASS();
    } else {
        FAIL("Expected error 461, got: " + response);
    }
}

void test_single_error_response() {
    TEST("Only one error response sent (not multiple)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    // Not authenticated, no params - could trigger multiple errors
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    int crlfCount = countOccurrences(response, "\r\n");
    if (crlfCount == 1) {
        PASS();
    } else {
        FAIL("Expected exactly 1 error, got " + intToString(crlfCount));
    }
}

// ============================================================================
// MULTIPLE COMMANDS TESTS
// ============================================================================

void test_multiple_user_commands() {
    TEST("Second USER command fails after registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    std::vector<std::string> params1 = makeParams("user1", "0", "*", "User One");
    UserCommand cmd1(&server, &client, params1);
    cmd1.execute();
    
    clearSocket(readFd);
    
    std::vector<std::string> params2 = makeParams("user2", "0", "*", "User Two");
    UserCommand cmd2(&server, &client, params2);
    cmd2.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "462")) {
        PASS();
    } else {
        FAIL("Expected error 462 on second USER, got: " + response);
    }
}

void test_first_username_preserved() {
    TEST("First username preserved after failed re-registration");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    std::vector<std::string> params1 = makeParams("originaluser", "0", "*", "Original User");
    UserCommand cmd1(&server, &client, params1);
    cmd1.execute();
    
    clearSocket(readFd);
    
    std::vector<std::string> params2 = makeParams("newuser", "0", "*", "New User");
    UserCommand cmd2(&server, &client, params2);
    cmd2.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "originaluser") {
        PASS();
    } else {
        FAIL("Original username should be preserved, got: " + client.getUsername());
    }
}

void test_three_user_commands() {
    TEST("Third USER command also fails");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    std::vector<std::string> params1 = makeParams("user1", "0", "*", "User One");
    UserCommand cmd1(&server, &client, params1);
    cmd1.execute();
    
    clearSocket(readFd);
    
    std::vector<std::string> params2 = makeParams("user2", "0", "*", "User Two");
    UserCommand cmd2(&server, &client, params2);
    cmd2.execute();
    
    clearSocket(readFd);
    
    std::vector<std::string> params3 = makeParams("user3", "0", "*", "User Three");
    UserCommand cmd3(&server, &client, params3);
    cmd3.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    if (containsString(response, "462")) {
        PASS();
    } else {
        FAIL("Expected error 462 on third USER, got: " + response);
    }
}

// ============================================================================
// EDGE CASES TESTS
// ============================================================================

void test_empty_username() {
    TEST("Empty username parameter");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("");
    params.push_back("0");
    params.push_back("*");
    params.push_back("Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_empty_realname() {
    TEST("Empty realname parameter");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("username");
    params.push_back("0");
    params.push_back("*");
    params.push_back("");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getRealname().empty()) {
        PASS();
    } else {
        FAIL("Realname should be empty");
    }
}

void test_long_username() {
    TEST("Very long username (100 chars)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::string longUser(100, 'a');
    std::vector<std::string> params;
    params.push_back(longUser);
    params.push_back("0");
    params.push_back("*");
    params.push_back("Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_very_long_username() {
    TEST("Extremely long username (1000 chars)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::string longUser(1000, 'x');
    std::vector<std::string> params;
    params.push_back(longUser);
    params.push_back("0");
    params.push_back("*");
    params.push_back("Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_long_realname() {
    TEST("Very long realname (500 chars)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::string longReal(500, 'a');
    std::vector<std::string> params;
    params.push_back("username");
    params.push_back("0");
    params.push_back("*");
    params.push_back(longReal);
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_mode_parameter_zero() {
    TEST("Mode parameter 0 (default)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("user0", "0", "*", "Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "user0") {
        PASS();
    } else {
        FAIL("Username should be set with mode 0");
    }
}

void test_mode_parameter_four() {
    TEST("Mode parameter 4 (wallops)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("user4", "4", "*", "Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "user4") {
        PASS();
    } else {
        FAIL("Username should be set with mode 4");
    }
}

void test_mode_parameter_eight() {
    TEST("Mode parameter 8 (invisible)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("user8", "8", "*", "Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "user8") {
        PASS();
    } else {
        FAIL("Username should be set with mode 8");
    }
}

void test_mode_parameter_invalid() {
    TEST("Mode parameter invalid value (99)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("user99", "99", "*", "Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    // Should still work - mode is typically ignored
    if (client.getUsername() == "user99") {
        PASS();
    } else {
        FAIL("Username should be set even with invalid mode");
    }
}

void test_mode_parameter_text() {
    TEST("Mode parameter as text");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("usertext", "text", "*", "Real Name");
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    // Should still work - mode is ignored
    if (client.getUsername() == "usertext") {
        PASS();
    } else {
        FAIL("Username should be set even with text mode");
    }
}

void test_unused_parameter_asterisk() {
    TEST("Unused parameter as asterisk (standard)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "username") {
        PASS();
    } else {
        FAIL("Unused parameter * should be ignored");
    }
}

void test_unused_parameter_custom() {
    TEST("Unused parameter as custom value");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "customvalue", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "username") {
        PASS();
    } else {
        FAIL("Unused parameter should be ignored");
    }
}

void test_unused_parameter_empty() {
    TEST("Unused parameter as empty string");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("username");
    params.push_back("0");
    params.push_back("");
    params.push_back("Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    if (client.getUsername() == "username") {
        PASS();
    } else {
        FAIL("Empty unused parameter should be ignored");
    }
}

void test_whitespace_in_params() {
    TEST("Whitespace handling in parameters");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "   Leading and trailing spaces   ");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    close(readFd);
    close(writeFd);
    
    // Should preserve the spaces as-is
    if (client.getRealname() == "   Leading and trailing spaces   ") {
        PASS();
    } else {
        FAIL("Whitespace should be preserved in realname");
    }
}

// ============================================================================
// DESTRUCTOR TESTS
// ============================================================================

void test_destructor() {
    TEST("Destructor cleanup");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    {
        UserCommand cmd(&server, &client, params);
        cmd.execute();
    }
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_polymorphic_destruction() {
    TEST("Polymorphic destruction through Command pointer");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    Command* basePtr = new UserCommand(&server, &client, params);
    basePtr->execute();
    delete basePtr;
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_multiple_allocations() {
    TEST("Multiple UserCommand allocations and deletions");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    for (int i = 0; i < 10; i++) {
        UserCommand* cmd = new UserCommand(&server, &client, params);
        delete cmd;
    }
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

// ============================================================================
// ERROR FORMAT TESTS
// ============================================================================

void test_error_461_format() {
    TEST("Error 461 response format");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    bool hasCode = containsString(response, "461");
    bool hasCRLF = response.length() >= 2 && 
                   response[response.length() - 2] == '\r' &&
                   response[response.length() - 1] == '\n';
    bool startsWithColon = response.length() > 0 && response[0] == ':';
    
    if (hasCode && hasCRLF && startsWithColon) {
        PASS();
    } else {
        FAIL("Error 461 format incorrect, got: " + response);
    }
}

void test_error_462_format() {
    TEST("Error 462 response format");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    client.setUsername("testuser");
    client.registerClient();
    
    clearSocket(readFd);
    
    std::vector<std::string> params = makeParams("newuser", "0", "*", "New Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    bool hasCode = containsString(response, "462");
    bool hasCRLF = response.length() >= 2 &&
                   response[response.length() - 2] == '\r' &&
                   response[response.length() - 1] == '\n';
    bool startsWithColon = response.length() > 0 && response[0] == ':';
    
    if (hasCode && hasCRLF && startsWithColon) {
        PASS();
    } else {
        FAIL("Error 462 format incorrect, got: " + response);
    }
}

void test_error_464_format() {
    TEST("Error 464 response format");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    bool hasCode = containsString(response, "464");
    bool hasCRLF = response.length() >= 2 &&
                   response[response.length() - 2] == '\r' &&
                   response[response.length() - 1] == '\n';
    bool startsWithColon = response.length() > 0 && response[0] == ':';
    
    if (hasCode && hasCRLF && startsWithColon) {
        PASS();
    } else {
        FAIL("Error 464 format incorrect, got: " + response);
    }
}

void test_error_uses_asterisk_when_unregistered() {
    TEST("Error response uses * for unregistered client");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    // Not registered, no nickname
    
    std::vector<std::string> params;
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    // Should contain "* :" somewhere (asterisk for unregistered)
    if (containsString(response, " * :") || containsString(response, " * USER")) {
        PASS();
    } else {
        FAIL("Should use * for unregistered client, got: " + response);
    }
}

void test_error_code_padding() {
    TEST("Error code is 3 digits (zero-padded)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    // Not authenticated
    
    std::vector<std::string> params = makeParams("username", "0", "*", "Real Name");
    
    UserCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    close(readFd);
    close(writeFd);
    
    // Should have " 464 " (3 digits)
    if (containsString(response, " 464 ")) {
        PASS();
    } else {
        FAIL("Error code should be 3 digits, got: " + response);
    }
}

// ============================================================================
// STRESS TESTS
// ============================================================================

void test_rapid_commands() {
    TEST("Rapid successive USER commands");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("testnick");
    
    for (int i = 0; i < 50; i++) {
        std::vector<std::string> params = makeParams("user", "0", "*", "Real");
        UserCommand cmd(&server, &client, params);
        cmd.execute();
        clearSocket(readFd);
    }
    
    close(readFd);
    close(writeFd);
    
    PASS();
}

void test_many_clients_simulation() {
    TEST("Multiple clients simulation (10 clients)");
    
    bool allPassed = true;
    
    for (int i = 0; i < 10; i++) {
        int readFd, writeFd;
        if (createTestSocketPair(readFd, writeFd) != 0) {
            allPassed = false;
            break;
        }
        
        Server server(6667, "password");
        Client client(writeFd);
        client.authenticate();
        client.setNickname("nick" + intToString(i));
        
        std::vector<std::string> params = makeParams("user" + intToString(i), "0", "*", "Real Name");
        UserCommand cmd(&server, &client, params);
        cmd.execute();
        
        if (!client.isRegistered()) {
            allPassed = false;
        }
        
        close(readFd);
        close(writeFd);
    }
    
    if (allPassed) {
        PASS();
    } else {
        FAIL("Not all simulated clients registered successfully");
    }
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          USERCOMMAND STANDALONE TEST SUITE - ft_irc Project        ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║  This test uses mock implementations - no external dependencies    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    // Constructor Tests
    SECTION("CONSTRUCTOR TESTS (6 tests)");
    test_constructor_valid();
    test_constructor_null_server();
    test_constructor_null_client();
    test_constructor_null_both();
    test_constructor_empty_params();
    test_constructor_many_params();
    
    // Authentication Tests
    SECTION("AUTHENTICATION TESTS (5 tests)");
    test_user_without_pass();
    test_user_without_pass_message_content();
    test_user_with_pass();
    test_user_unauthenticated_with_nick();
    test_user_unauthenticated_no_params();
    
    // Parameter Validation Tests
    SECTION("PARAMETER VALIDATION TESTS (10 tests)");
    test_user_no_params();
    test_user_one_param();
    test_user_two_params();
    test_user_three_params();
    test_user_four_params();
    test_user_five_params();
    test_user_ten_params();
    test_user_insufficient_params_message();
    test_user_params_not_modified_on_error();
    
    // Re-registration Tests
    SECTION("RE-REGISTRATION TESTS (5 tests)");
    test_user_already_registered();
    test_user_already_registered_message();
    test_user_does_not_change_when_registered();
    test_user_realname_not_changed_when_registered();
    test_user_registered_uses_nick_in_error();
    
    // Username Setting Tests
    SECTION("USERNAME SETTING TESTS (7 tests)");
    test_user_sets_username();
    test_user_username_special_chars();
    test_user_username_with_numbers();
    test_user_username_all_numbers();
    test_user_username_single_char();
    test_user_username_uppercase();
    test_user_username_mixed_case();
    
    // Realname Setting Tests
    SECTION("REALNAME SETTING TESTS (7 tests)");
    test_user_sets_realname();
    test_user_realname_with_spaces();
    test_user_realname_with_special_chars();
    test_user_realname_with_colon();
    test_user_realname_with_numbers();
    test_user_realname_unicode_like();
    test_user_realname_symbols();
    
    // Registration Completion Tests
    SECTION("REGISTRATION COMPLETION TESTS (5 tests)");
    test_registration_pass_nick_user();
    test_registration_pass_user_only();
    test_user_sets_username_without_nick();
    test_user_sets_realname_without_nick();
    test_user_empty_nick_no_registration();
    
    // Welcome Message Tests
    SECTION("WELCOME MESSAGE TESTS (8 tests)");
    test_welcome_001_sent();
    test_welcome_contains_welcome_text();
    test_welcome_contains_nickname();
    test_welcome_contains_irc_network();
    test_no_welcome_without_nick();
    test_response_ends_with_crlf();
    test_welcome_single_response();
    test_welcome_starts_with_colon();
    
    // Error Precedence Tests
    SECTION("ERROR PRECEDENCE TESTS (3 tests)");
    test_auth_checked_before_params();
    test_params_checked_before_registration();
    test_single_error_response();
    
    // Multiple Commands Tests
    SECTION("MULTIPLE COMMANDS TESTS (3 tests)");
    test_multiple_user_commands();
    test_first_username_preserved();
    test_three_user_commands();
    
    // Edge Cases Tests
    SECTION("EDGE CASES TESTS (14 tests)");
    test_empty_username();
    test_empty_realname();
    test_long_username();
    test_very_long_username();
    test_long_realname();
    test_mode_parameter_zero();
    test_mode_parameter_four();
    test_mode_parameter_eight();
    test_mode_parameter_invalid();
    test_mode_parameter_text();
    test_unused_parameter_asterisk();
    test_unused_parameter_custom();
    test_unused_parameter_empty();
    test_whitespace_in_params();
    
    // Destructor Tests
    SECTION("DESTRUCTOR TESTS (3 tests)");
    test_destructor();
    test_polymorphic_destruction();
    test_multiple_allocations();
    
    // Error Format Tests
    SECTION("ERROR FORMAT TESTS (5 tests)");
    test_error_461_format();
    test_error_462_format();
    test_error_464_format();
    test_error_uses_asterisk_when_unregistered();
    test_error_code_padding();
    
    // Stress Tests
    SECTION("STRESS TESTS (2 tests)");
    test_rapid_commands();
    test_many_clients_simulation();
    
    // Summary
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                          TEST SUMMARY                              ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "  Total tests run: " << (passCount + failCount) << "\n";
    std::cout << "  \033[32mPassed: " << passCount << "\033[0m\n";
    std::cout << "  \033[31mFailed: " << failCount << "\033[0m\n";
    std::cout << "\n";
    
    if (failCount == 0) {
        std::cout << "\033[32m╔════════════════════════════════════════════════════════════════════╗\033[0m\n";
        std::cout << "\033[32m║                 ALL TESTS PASSED SUCCESSFULLY!                    ║\033[0m\n";
        std::cout << "\033[32m╚════════════════════════════════════════════════════════════════════╝\033[0m\n";
    } else {
        std::cout << "\033[31m╔════════════════════════════════════════════════════════════════════╗\033[0m\n";
        std::cout << "\033[31m║                 SOME TESTS FAILED - PLEASE FIX!                   ║\033[0m\n";
        std::cout << "\033[31m╚════════════════════════════════════════════════════════════════════╝\033[0m\n";
    }
    std::cout << "\n";
    
    return failCount > 0 ? 1 : 0;
}
