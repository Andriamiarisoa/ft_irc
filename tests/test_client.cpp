#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(name) std::cout << "Testing " << name << "... "
#define PASS() std::cout << "\033[32mPASS\033[0m" << std::endl
#define FAIL(msg) std::cout << "\033[31mFAIL: " << msg << "\033[0m" << std::endl

void test_constructor() {
    TEST("Constructor");
    Client client(42);
    
    assert(client.getFd() == 42);
    assert(client.getNickname().empty());
    assert(client.getUsername().empty());
    assert(!client.isAuthenticated());
    assert(!client.isRegistered());
    
    PASS();
}

void test_authentication_flow() {
    TEST("Authentication flow");
    Client client(1);
    
    // État initial
    assert(!client.isAuthenticated());
    assert(!client.isRegistered());
    
    // Après authenticate()
    client.authenticate();
    assert(client.isAuthenticated());
    assert(!client.isRegistered()); // Pas encore de NICK/USER
    
    PASS();
}

void test_registration_flow() {
    TEST("Registration flow (PASS -> NICK -> USER)");
    Client client(1);
    
    // PASS
    client.authenticate();
    assert(client.isAuthenticated());
    assert(!client.isRegistered());
    
    // NICK
    client.setNickname("TestUser");
    assert(client.getNickname() == "TestUser");
    assert(!client.isRegistered()); // Pas encore de USER
    
    // USER
    client.setUsername("testuser");
    assert(client.getUsername() == "testuser");
    assert(client.isRegistered()); // Maintenant enregistré
    
    PASS();
}

void test_nickname_validation() {
    TEST("Nickname validation (RFC 1459)");
    Client client(1);
    client.authenticate();
    
    // Cas valides
    client.setNickname("Alice");
    assert(client.getNickname() == "Alice");
    
    client.setNickname("Bob123");
    assert(client.getNickname() == "Bob123");
    
    client.setNickname("a");
    assert(client.getNickname() == "a");
    
    client.setNickname("Test-User");
    assert(client.getNickname() == "Test-User");
    
    // Test caractères spéciaux RFC 1459: -, [, ], \, `, ^, {, }
    client.setNickname("a[]");
    assert(client.getNickname() == "a[]");
    
    client.setNickname("a{}");
    assert(client.getNickname() == "a{}");
    
    // Cas invalides - doit garder l'ancien nickname
    std::string oldNick = client.getNickname();
    
    client.setNickname(""); // Vide
    assert(client.getNickname() == oldNick);
    
    client.setNickname("123abc"); // Commence par un chiffre
    assert(client.getNickname() == oldNick);
    
    client.setNickname("-test"); // Commence par un tiret
    assert(client.getNickname() == oldNick);
    
    client.setNickname("VeryLongNickname"); // Plus de 9 caractères
    assert(client.getNickname() == oldNick);
    
    client.setNickname("test@user"); // Caractère @ invalide
    assert(client.getNickname() == oldNick);
    
    PASS();
}

void test_buffer_operations() {
    TEST("Buffer operations");
    Client client(1);
    
    // Ajout au buffer
    client.appendToBuffer("NICK test");
    client.appendToBuffer("\r\n");
    
    // Extraction de commande
    std::string cmd = client.extractCommand();
    assert(cmd == "NICK test");
    
    // Buffer vide après extraction
    cmd = client.extractCommand();
    assert(cmd.empty());
    
    PASS();
}

void test_multiple_commands_in_buffer() {
    TEST("Multiple commands in buffer");
    Client client(1);
    
    client.appendToBuffer("NICK alice\r\nUSER alice 0 * :Alice\r\nJOIN #test\r\n");
    
    std::string cmd1 = client.extractCommand();
    assert(cmd1 == "NICK alice");
    
    std::string cmd2 = client.extractCommand();
    assert(cmd2 == "USER alice 0 * :Alice");
    
    std::string cmd3 = client.extractCommand();
    assert(cmd3 == "JOIN #test");
    
    std::string cmd4 = client.extractCommand();
    assert(cmd4.empty());
    
    PASS();
}

void test_incomplete_command() {
    TEST("Incomplete command in buffer");
    Client client(1);
    
    client.appendToBuffer("NICK test");
    std::string cmd = client.extractCommand();
    assert(cmd.empty()); // Pas de \r\n, donc pas de commande complète
    
    client.appendToBuffer("\r\n");
    cmd = client.extractCommand();
    assert(cmd == "NICK test");
    
    PASS();
}

void test_buffer_overflow_protection() {
    TEST("Buffer overflow protection");
    Client client(1);
    
    // Créer une chaîne de 500 caractères
    std::string bigData(500, 'A');
    client.appendToBuffer(bigData);
    
    // Ajouter encore 600 caractères devrait dépasser la limite de 1024
    std::string moreData(600, 'B');
    client.appendToBuffer(moreData);
    
    // Le buffer devrait être vidé (protection contre overflow)
    std::string cmd = client.extractCommand();
    assert(cmd.empty());
    
    PASS();
}

void test_order_independence() {
    TEST("Order independence (USER before NICK)");
    Client client(1);
    
    client.authenticate();
    
    // USER d'abord
    client.setUsername("testuser");
    assert(!client.isRegistered()); // Pas encore de NICK
    
    // NICK ensuite
    client.setNickname("TestNick");
    assert(client.isRegistered()); // Maintenant enregistré
    
    PASS();
}

void test_no_auth_no_registration() {
    TEST("No auth = no registration");
    Client client(1);
    
    // Sans authenticate(), même avec NICK et USER, pas d'enregistrement
    client.setNickname("TestUser");
    client.setUsername("testuser");
    
    assert(!client.isRegistered());
    
    // Après authenticate()
    client.authenticate();
    assert(client.isRegistered());
    
    PASS();
}

int main() {
    std::cout << "\n=== Tests de la classe Client ===\n" << std::endl;
    
    test_constructor();
    test_authentication_flow();
    test_registration_flow();
    test_nickname_validation();
    test_buffer_operations();
    test_multiple_commands_in_buffer();
    test_incomplete_command();
    test_buffer_overflow_protection();
    test_order_independence();
    test_no_auth_no_registration();
    
    std::cout << "\n\033[32m=== Tous les tests sont passés ! ===\033[0m\n" << std::endl;
    
    return 0;
}
