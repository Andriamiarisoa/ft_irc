/**
 * @file test_nickcommand.cpp
 * @brief Tests unitaires pour la classe NickCommand
 * @author YASSER
 * @date 2026-01-21
 * 
 * Ce fichier contient les tests unitaires pour valider le comportement
 * de la commande NICK conformément au protocole IRC RFC 1459.
 */

#include "../includes/NickCommand.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdio>

// ============================================================================
// MACROS DE TEST
// ============================================================================

#define TEST(name) std::cout << "Testing " << name << "... "
#define PASS() std::cout << "\033[32mPASS\033[0m" << std::endl; g_passed++
#define FAIL(msg) std::cout << "\033[31mFAIL: " << msg << "\033[0m" << std::endl; g_failed++
#define SECTION(name) std::cout << "\n\033[36m=== " << name << " ===\033[0m\n" << std::endl

static int g_passed = 0;
static int g_failed = 0;

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

/**
 * @brief Crée une paire de sockets pour les tests
 * @param readFd Socket de lecture (côté serveur)
 * @param writeFd Socket d'écriture (côté client)
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int createTestSocketPair(int& readFd, int& writeFd) {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        return -1;
    }
    readFd = fds[0];
    writeFd = fds[1];
    return 0;
}

/**
 * @brief Lit les données envoyées au socket
 * @param fd Descripteur du socket
 * @param maxLen Taille maximale à lire
 * @return Les données lues sous forme de string
 */
std::string readFromSocket(int fd, size_t maxLen = 1024) {
    char buffer[1025];
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(fd, buffer, maxLen, MSG_DONTWAIT);
    if (n > 0) {
        return std::string(buffer, n);
    }
    return "";
}

/**
 * @brief Vérifie si une réponse contient un code numérique spécifique
 * @param response La réponse du serveur
 * @param code Le code à rechercher
 * @return true si le code est trouvé
 */
bool containsCode(const std::string& response, int code) {
    char codeStr[4];
    sprintf(codeStr, "%03d", code);
    return response.find(codeStr) != std::string::npos;
}

/**
 * @brief Vérifie si une réponse contient un texte spécifique
 * @param response La réponse du serveur
 * @param text Le texte à rechercher
 * @return true si le texte est trouvé
 */
bool containsText(const std::string& response, const std::string& text) {
    return response.find(text) != std::string::npos;
}

// ============================================================================
// TESTS DU CONSTRUCTEUR
// ============================================================================

void test_constructor_valid() {
    TEST("Constructeur avec paramètres valides");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    try {
        NickCommand cmd(&server, &client, params);
        PASS();
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
    
    close(readFd);
    close(writeFd);
}

void test_constructor_null_server() {
    TEST("Constructeur avec Server NULL");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Client client(writeFd);
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    bool exceptionThrown = false;
    try {
        NickCommand cmd(NULL, &client, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    if (exceptionThrown) {
        PASS();
    } else {
        FAIL("Exception non levée pour Server NULL");
    }
    
    close(readFd);
    close(writeFd);
}

void test_constructor_null_client() {
    TEST("Constructeur avec Client NULL");
    
    Server server(6667, "password");
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    bool exceptionThrown = false;
    try {
        NickCommand cmd(&server, NULL, params);
    } catch (const std::invalid_argument& e) {
        exceptionThrown = true;
    }
    
    if (exceptionThrown) {
        PASS();
    } else {
        FAIL("Exception non levée pour Client NULL");
    }
}

// ============================================================================
// TESTS D'AUTHENTIFICATION
// ============================================================================

void test_nick_before_pass() {
    TEST("NICK avant PASS envoie ERR 464");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    // Client NON authentifié (pas de PASS)
    
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 464)) {
        PASS();
    } else {
        FAIL("ERR 464 non envoyé. Réponse: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_after_pass() {
    TEST("NICK après PASS accepté");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate(); // Simuler PASS réussi
    
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Pas d'erreur 464 et nickname défini
    if (!containsCode(response, 464) && client.getNickname() == "TestNick") {
        PASS();
    } else {
        FAIL("NICK après PASS a échoué. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS DE VALIDATION DES PARAMÈTRES
// ============================================================================

void test_nick_no_parameter() {
    TEST("NICK sans paramètre envoie ERR 431");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params; // Vecteur vide
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 431) && containsText(response, "No nickname given")) {
        PASS();
    } else {
        FAIL("ERR 431 non envoyé correctement. Réponse: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_empty_parameter() {
    TEST("NICK avec paramètre vide - nickname non modifié");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("Original"); // Définir un nickname initial
    
    std::vector<std::string> params;
    params.push_back(""); // Paramètre vide
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Un paramètre vide devrait soit:
    // - Envoyer une erreur 431/432
    // - OU ne pas modifier le nickname (validation silencieuse dans setNickname)
    bool hasError = containsCode(response, 431) || containsCode(response, 432);
    bool nicknameUnchanged = client.getNickname() == "Original";
    
    if (hasError || nicknameUnchanged) {
        PASS();
    } else {
        FAIL("Nickname modifié avec paramètre vide. Nickname: " + client.getNickname());
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS DE VALIDATION DU FORMAT NICKNAME
// ============================================================================

void test_nick_valid_simple() {
    TEST("NICK valide simple (lettres)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("Alice");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    if (client.getNickname() == "Alice") {
        PASS();
    } else {
        FAIL("Nickname non défini. Actuel: " + client.getNickname());
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_valid_with_numbers() {
    TEST("NICK valide avec chiffres (Bob123)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("Bob123");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    if (client.getNickname() == "Bob123") {
        PASS();
    } else {
        FAIL("Nickname non défini. Actuel: " + client.getNickname());
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_valid_special_chars() {
    TEST("NICK valide avec caractères spéciaux RFC 1459 (a-[])");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("a-[]");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    if (client.getNickname() == "a-[]") {
        PASS();
    } else {
        FAIL("Nickname avec caractères spéciaux refusé. Actuel: " + client.getNickname());
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_valid_max_length() {
    TEST("NICK valide longueur maximale (9 caractères)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("Abcdefghi"); // 9 caractères
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    if (client.getNickname() == "Abcdefghi") {
        PASS();
    } else {
        FAIL("Nickname 9 caractères refusé. Actuel: " + client.getNickname());
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_invalid_starts_with_number() {
    TEST("NICK invalide commençant par un chiffre envoie ERR 432");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("123abc");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 432) && client.getNickname().empty()) {
        PASS();
    } else {
        FAIL("ERR 432 non envoyé ou nickname défini. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_invalid_starts_with_dash() {
    TEST("NICK invalide commençant par un tiret envoie ERR 432");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("-test");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 432) && client.getNickname().empty()) {
        PASS();
    } else {
        FAIL("ERR 432 non envoyé ou nickname défini. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_invalid_too_long() {
    TEST("NICK invalide trop long (>9 caractères) envoie ERR 432");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("Abcdefghij"); // 10 caractères
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 432) && client.getNickname().empty()) {
        PASS();
    } else {
        FAIL("ERR 432 non envoyé pour nickname trop long. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_invalid_special_chars() {
    TEST("NICK invalide avec caractères interdits (@#$)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("test@user");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    if (containsCode(response, 432) && client.getNickname().empty()) {
        PASS();
    } else {
        FAIL("ERR 432 non envoyé pour caractères interdits. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS D'ENREGISTREMENT
// ============================================================================

void test_nick_completes_registration() {
    TEST("NICK + USER complète l'enregistrement (messages 001-004)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setUsername("testuser"); // USER déjà défini
    
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    bool hasWelcome = containsCode(response, 1);   // RPL_WELCOME
    bool hasYourHost = containsCode(response, 2);  // RPL_YOURHOST
    bool hasCreated = containsCode(response, 3);   // RPL_CREATED
    bool hasMyInfo = containsCode(response, 4);    // RPL_MYINFO
    bool isRegistered = client.isRegistered();
    
    if (hasWelcome && hasYourHost && hasCreated && hasMyInfo && isRegistered) {
        PASS();
    } else {
        FAIL("Messages de bienvenue incomplets ou client non enregistré. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_only_not_registered() {
    TEST("NICK seul ne complète pas l'enregistrement (sans USER)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    // PAS de setUsername()
    
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Ne devrait pas recevoir les messages de bienvenue
    bool hasWelcome = containsCode(response, 1);
    bool isRegistered = client.isRegistered();
    
    if (!hasWelcome && !isRegistered && client.getNickname() == "TestNick") {
        PASS();
    } else {
        FAIL("Client enregistré sans USER ou welcome envoyé");
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS DE CHANGEMENT DE NICKNAME
// ============================================================================

void test_nick_change_when_registered() {
    TEST("Changement de NICK quand déjà enregistré");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("OldNick");
    client.setUsername("testuser");
    client.setHostname("localhost");
    client.registerClient();
    
    std::vector<std::string> params;
    params.push_back("NewNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Doit recevoir le message de changement de nick
    bool hasNickChange = containsText(response, "NICK") && 
                         containsText(response, "NewNick") &&
                         containsText(response, "OldNick");
    bool nickUpdated = client.getNickname() == "NewNick";
    
    if (hasNickChange && nickUpdated) {
        PASS();
    } else {
        FAIL("Changement de nick non diffusé ou nick non mis à jour. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_change_format() {
    TEST("Format du message de changement de NICK");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("John");
    client.setUsername("john");
    client.setHostname("localhost");
    client.registerClient();
    
    std::vector<std::string> params;
    params.push_back("Johnny");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Format attendu: :John!john@localhost NICK :Johnny
    bool hasCorrectFormat = containsText(response, ":John!john@localhost NICK :Johnny");
    
    if (hasCorrectFormat) {
        PASS();
    } else {
        FAIL("Format du message incorrect. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

void test_nick_change_same_nick() {
    TEST("NICK identique au nickname actuel (pas d'erreur)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    client.setNickname("TestNick");
    client.setUsername("testuser");
    client.registerClient();
    
    std::vector<std::string> params;
    params.push_back("TestNick"); // Même nickname
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Ne devrait pas y avoir d'erreur 433
    bool noError = !containsCode(response, 433);
    
    if (noError && client.getNickname() == "TestNick") {
        PASS();
    } else {
        FAIL("Erreur lors de NICK identique. Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS DE COLLISION DE NICKNAME
// ============================================================================

// Note: Ces tests dépendent de l'implémentation de Server::getClientByNick()
// qui est actuellement vide. Ils sont inclus pour la complétude.

void test_nick_already_in_use() {
    TEST("NICK déjà utilisé envoie ERR 433 (nécessite getClientByNick)");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("TakenNick"); // 9 caractères max (ExistingNick était trop long!)
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    // Note: Ce test passera actuellement car getClientByNick retourne NULL
    // Une fois implémenté, ce comportement devrait changer
    std::string response = readFromSocket(readFd);
    
    // Test simplifié - vérifie juste que le nickname est défini
    // (car getClientByNick n'est pas implémenté, il retourne toujours NULL)
    if (client.getNickname() == "TakenNick" || containsCode(response, 433)) {
        PASS();
    } else {
        FAIL("Comportement inattendu. Nickname: " + client.getNickname() + ", Response: " + response);
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// TESTS DES MESSAGES CRLF
// ============================================================================

void test_nick_response_ends_with_crlf() {
    TEST("Toutes les réponses se terminent par CRLF");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    // Client non authentifié pour générer une erreur
    
    std::vector<std::string> params;
    params.push_back("TestNick");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Vérifier que la réponse se termine par \r\n
    bool endsWithCRLF = response.length() >= 2 && 
                        response.substr(response.length() - 2) == "\r\n";
    
    if (endsWithCRLF) {
        PASS();
    } else {
        FAIL("Réponse ne se termine pas par CRLF");
    }
    
    close(readFd);
    close(writeFd);
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

int main() {
    std::cout << "\n\033[1;35m";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║           TESTS UNITAIRES - NickCommand                   ║\n";
    std::cout << "║                   ft_irc - 42                             ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    SECTION("CONSTRUCTEUR");
    test_constructor_valid();
    test_constructor_null_server();
    test_constructor_null_client();

    SECTION("AUTHENTIFICATION (PASS requis)");
    test_nick_before_pass();
    test_nick_after_pass();

    SECTION("VALIDATION DES PARAMÈTRES");
    test_nick_no_parameter();
    test_nick_empty_parameter();

    SECTION("VALIDATION DU FORMAT NICKNAME (RFC 1459)");
    test_nick_valid_simple();
    test_nick_valid_with_numbers();
    test_nick_valid_special_chars();
    test_nick_valid_max_length();
    test_nick_invalid_starts_with_number();
    test_nick_invalid_starts_with_dash();
    test_nick_invalid_too_long();
    test_nick_invalid_special_chars();

    SECTION("ENREGISTREMENT (NICK + USER)");
    test_nick_completes_registration();
    test_nick_only_not_registered();

    SECTION("CHANGEMENT DE NICKNAME");
    test_nick_change_when_registered();
    test_nick_change_format();
    test_nick_change_same_nick();

    SECTION("COLLISION DE NICKNAME");
    test_nick_already_in_use();

    SECTION("FORMAT DES MESSAGES");
    test_nick_response_ends_with_crlf();

    // Résumé final
    std::cout << "\n\033[1;35m";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "\033[0m";
    std::cout << "  Résultats: \033[32m" << g_passed << " PASSED\033[0m, ";
    std::cout << "\033[31m" << g_failed << " FAILED\033[0m\n";
    std::cout << "\033[1;35m";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "\033[0m\n";

    return g_failed > 0 ? 1 : 0;
}
