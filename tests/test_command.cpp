#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(name) std::cout << "Testing " << name << "... "
#define PASS() std::cout << "\033[32mPASS\033[0m" << std::endl
#define FAIL(msg) std::cout << "\033[31mFAIL: " << msg << "\033[0m" << std::endl

// Créer une paire de sockets pour les tests
int createTestSocketPair(int& readFd, int& writeFd) {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        return -1;
    }
    readFd = fds[0];
    writeFd = fds[1];
    return 0;
}

std::string readFromSocket(int fd, size_t maxLen = 512) {
    char buffer[513];
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(fd, buffer, maxLen, MSG_DONTWAIT);
    if (n > 0) {
        return std::string(buffer, n);
    }
    return "";
}

// Classe de test concrète dérivée de Command
class TestCommand : public Command {
public:
    TestCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params) {}
    
    void execute() {
        // Implémentation vide pour les tests
    }
    
    // Expose les méthodes protégées pour les tests
    void testSendReply(int code, const std::string& msg) {
        sendReply(code, msg);
    }
    
    void testSendError(int code, const std::string& msg) {
        sendError(code, msg);
    }
    
    Server* getServer() { return server; }
    Client* getClient() { return client; }
    std::vector<std::string>& getParams() { return params; }
};

void test_constructor_valid() {
    TEST("Constructor with valid pointers");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    params.push_back("param1");
    params.push_back("param2");
    
    TestCommand cmd(&server, &client, params);
    
    assert(cmd.getServer() == &server);
    assert(cmd.getClient() == &client);
    assert(cmd.getParams().size() == 2);
    assert(cmd.getParams()[0] == "param1");
    assert(cmd.getParams()[1] == "param2");
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_constructor_null_server() {
    TEST("Constructor with NULL server throws exception");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Client client(writeFd);
    std::vector<std::string> params;
    
    bool exceptionThrown = false;
    try {
        TestCommand cmd(NULL, &client, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    assert(exceptionThrown);
    close(readFd);
    close(writeFd);
    PASS();
}

void test_constructor_null_client() {
    TEST("Constructor with NULL client throws exception");
    
    Server server(6667, "password");
    std::vector<std::string> params;
    
    bool exceptionThrown = false;
    try {
        TestCommand cmd(&server, NULL, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    assert(exceptionThrown);
    PASS();
}

void test_constructor_empty_params() {
    TEST("Constructor with empty params");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    assert(cmd.getParams().empty());
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendReply_format_unregistered() {
    TEST("sendReply format for unregistered client");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    cmd.testSendReply(1, ":Welcome to IRC");
    
    std::string received = readFromSocket(readFd);
    std::string expected = ":irc.example.com 001 * :Welcome to IRC\r\n";
    assert(received == expected);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendReply_format_registered() {
    TEST("sendReply format for registered client");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    // Enregistrer le client
    client.authenticate();
    client.setNickname("TestUser");
    client.setUsername("testuser");
    
    TestCommand cmd(&server, &client, params);
    
    cmd.testSendReply(332, "#channel :This is the topic");
    
    std::string received = readFromSocket(readFd);
    std::string expected = ":irc.example.com 332 TestUser #channel :This is the topic\r\n";
    assert(received == expected);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendReply_code_padding() {
    TEST("sendReply code padding (3 digits)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    // Code à 1 chiffre -> 001
    cmd.testSendReply(1, ":test");
    std::string received = readFromSocket(readFd);
    assert(received.find(" 001 ") != std::string::npos);
    
    // Code à 2 chiffres -> 042
    cmd.testSendReply(42, ":test");
    received = readFromSocket(readFd);
    assert(received.find(" 042 ") != std::string::npos);
    
    // Code à 3 chiffres -> 461
    cmd.testSendReply(461, ":test");
    received = readFromSocket(readFd);
    assert(received.find(" 461 ") != std::string::npos);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendError_format() {
    TEST("sendError format (with colon before message)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    cmd.testSendError(461, "Not enough parameters");
    
    // sendError ajoute ":" avant le message
    std::string received = readFromSocket(readFd);
    std::string expected = ":irc.example.com 461 * :Not enough parameters\r\n";
    assert(received == expected);
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendReply_ends_with_crlf() {
    TEST("sendReply ends with CRLF");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    cmd.testSendReply(1, ":test");
    
    std::string received = readFromSocket(readFd);
    size_t len = received.length();
    assert(len >= 2);
    assert(received[len - 2] == '\r');
    assert(received[len - 1] == '\n');
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_sendError_ends_with_crlf() {
    TEST("sendError ends with CRLF");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    cmd.testSendError(461, "error message");
    
    std::string received = readFromSocket(readFd);
    size_t len = received.length();
    assert(len >= 2);
    assert(received[len - 2] == '\r');
    assert(received[len - 1] == '\n');
    
    close(readFd);
    close(writeFd);
    PASS();
}

void test_polymorphic_destruction() {
    TEST("Polymorphic destruction (virtual destructor)");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    Command* basePtr = new TestCommand(&server, &client, params);
    
    // Ceci devrait fonctionner sans fuite si le destructeur est virtuel
    delete basePtr;
    
    close(readFd);
    close(writeFd);
    PASS();
}

int main() {
    std::cout << "\n=== Tests de la classe Command ===\n" << std::endl;
    
    test_constructor_valid();
    test_constructor_null_server();
    test_constructor_null_client();
    test_constructor_empty_params();
    test_sendReply_format_unregistered();
    test_sendReply_format_registered();
    test_sendReply_code_padding();
    test_sendError_format();
    test_sendReply_ends_with_crlf();
    test_sendError_ends_with_crlf();
    test_polymorphic_destruction();
    
    std::cout << "\n\033[32m=== Tous les tests sont passés ! ===\033[0m\n" << std::endl;
    
    return 0;
}
