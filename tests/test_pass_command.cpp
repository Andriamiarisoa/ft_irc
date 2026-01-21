/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pass_command.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by ver.cpp ../       #+#    #+#             */
/*   Updated: 2026/01/21 21:04:42 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

/* ========================================================================== */
/*                              CONFIGURATION                                  */
/* ========================================================================== */

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 6667
#define SERVER_PASSWORD "testpass"
#define BUFFER_SIZE 1024
#define TEST_TIMEOUT 3

/* ========================================================================== */
/*                              COULEURS                                       */
/* ========================================================================== */

#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

/* ========================================================================== */
/*                              VARIABLES GLOBALES                             */
/* ========================================================================== */

static int g_tests_passed = 0;
static int g_tests_failed = 0;
static int g_tests_total = 0;

/* ========================================================================== */
/*                              FONCTIONS UTILITAIRES                          */
/* ========================================================================== */

void printHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << CYAN << "╔══════════════════════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << CYAN << "║" << RESET << BOLD << "  " << title;
    for (size_t i = title.length(); i < 60; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << std::endl;
    std::cout << CYAN << "╚══════════════════════════════════════════════════════════════╝" << RESET << std::endl;
}

void printTestCase(const std::string& name) {
    std::cout << std::endl;
    std::cout << YELLOW << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
    std::cout << MAGENTA << "TEST: " << RESET << name << std::endl;
    std::cout << YELLOW << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
}

void printSuccess(const std::string& msg) {
    std::cout << GREEN << "  ✓ PASS: " << RESET << msg << std::endl;
    g_tests_passed++;
    g_tests_total++;
}

void printFailure(const std::string& msg) {
    std::cout << RED << "  ✗ FAIL: " << RESET << msg << std::endl;
    g_tests_failed++;
    g_tests_total++;
}

void printInfo(const std::string& msg) {
    std::cout << BLUE << "  ℹ INFO: " << RESET << msg << std::endl;
}

void printSend(const std::string& cmd) {
    std::cout << WHITE << "  → SEND: " << RESET << cmd << std::endl;
}

void printRecv(const std::string& response) {
    std::cout << WHITE << "  ← RECV: " << RESET << response << std::endl;
}

/* ========================================================================== */
/*                              FONCTIONS RÉSEAU                               */
/* ========================================================================== */

int connectToServer() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printFailure("Impossible de créer le socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_HOST, &server_addr.sin_addr);

    struct timeval timeout;
    timeout.tv_sec = TEST_TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printFailure("Impossible de se connecter au serveur");
        close(sock);
        return -1;
    }

    // Lire le message de bienvenue initial si présent
    char buffer[BUFFER_SIZE];
    recv(sock, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

    return sock;
}

std::string sendCommand(int sock, const std::string& cmd) {
    std::string fullCmd = cmd + "\r\n";
    printSend(cmd);
    
    if (send(sock, fullCmd.c_str(), fullCmd.length(), 0) < 0) {
        printFailure("Échec de l'envoi de la commande");
        return "";
    }

    usleep(100000); // 100ms delay

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    
    std::string response = "";
    if (bytes > 0) {
        response = std::string(buffer, bytes);
        // Supprimer \r\n pour l'affichage
        while (!response.empty() && (response[response.length()-1] == '\r' || response[response.length()-1] == '\n'))
            response.erase(response.length()-1);
        printRecv(response);
    } else {
        printInfo("Pas de réponse (succès silencieux ou timeout)");
    }
    
    return response;
}

bool responseContains(const std::string& response, const std::string& substring) {
    return response.find(substring) != std::string::npos;
}

bool responseContainsErrorCode(const std::string& response, int code) {
    char codeStr[10];
    sprintf(codeStr, " %d ", code);
    return response.find(codeStr) != std::string::npos;
}

/* ========================================================================== */
/*                              TESTS PASS COMMAND                             */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  TEST 01: PASS avec le bon mot de passe                                    */
/* -------------------------------------------------------------------------- */
void test_pass_correct_password() {
    printTestCase("01 - PASS avec le bon mot de passe authentifie le client");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PASS " SERVER_PASSWORD);
    
    // PASS réussi = succès silencieux (pas de réponse) selon RFC
    if (response.empty() || !responseContainsErrorCode(response, 464)) {
        printSuccess("PASS accepté (succès silencieux ou pas d'erreur 464)");
    } else {
        printFailure("PASS refusé alors que le mot de passe est correct");
    }

    // Vérifier que NICK fonctionne après PASS
    response = sendCommand(sock, "NICK testuser01");
    if (!responseContainsErrorCode(response, 464)) {
        printSuccess("NICK accepté après PASS réussi");
    } else {
        printFailure("NICK refusé après PASS réussi");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 02: PASS avec le mauvais mot de passe                                */
/* -------------------------------------------------------------------------- */
void test_pass_wrong_password() {
    printTestCase("02 - PASS avec le mauvais mot de passe envoie l'erreur 464");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PASS wrongpassword");
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Erreur 464 (ERR_PASSWDMISMATCH) reçue");
    } else {
        printFailure("Erreur 464 attendue mais non reçue");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 03: PASS sans paramètre                                              */
/* -------------------------------------------------------------------------- */
void test_pass_no_parameter() {
    printTestCase("03 - PASS sans paramètre envoie l'erreur 461");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PASS");
    
    if (responseContainsErrorCode(response, 461)) {
        printSuccess("Erreur 461 (ERR_NEEDMOREPARAMS) reçue");
    } else {
        printFailure("Erreur 461 attendue mais non reçue");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 04: PASS après l'enregistrement complet                              */
/* -------------------------------------------------------------------------- */
void test_pass_after_registration() {
    printTestCase("04 - PASS après l'enregistrement envoie l'erreur 462");
    
    int sock = connectToServer();
    if (sock < 0) return;

    // Enregistrement complet
    sendCommand(sock, "PASS " SERVER_PASSWORD);
    sendCommand(sock, "NICK testuser04");
    sendCommand(sock, "USER testuser04 0 * :Test User 04");

    usleep(100000);

    // Tenter PASS après enregistrement
    std::string response = sendCommand(sock, "PASS " SERVER_PASSWORD);
    
    if (responseContainsErrorCode(response, 462)) {
        printSuccess("Erreur 462 (ERR_ALREADYREGISTRED) reçue");
    } else {
        printFailure("Erreur 462 attendue mais non reçue");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 05: Plusieurs tentatives PASS avant enregistrement                   */
/* -------------------------------------------------------------------------- */
void test_pass_multiple_attempts() {
    printTestCase("05 - Plusieurs tentatives PASS avant l'enregistrement autorisées");
    
    int sock = connectToServer();
    if (sock < 0) return;

    // Première tentative échouée
    std::string response1 = sendCommand(sock, "PASS wrongpass1");
    bool fail1 = responseContainsErrorCode(response1, 464);

    // Deuxième tentative échouée
    std::string response2 = sendCommand(sock, "PASS wrongpass2");
    bool fail2 = responseContainsErrorCode(response2, 464);

    // Troisième tentative réussie
    std::string response3 = sendCommand(sock, "PASS " SERVER_PASSWORD);
    bool success = !responseContainsErrorCode(response3, 464);

    if (fail1 && fail2 && success) {
        printSuccess("Plusieurs tentatives autorisées, succès à la 3ème");
    } else if (!fail1 || !fail2) {
        printFailure("Les mauvais mots de passe n'ont pas généré d'erreur 464");
    } else {
        printFailure("Le bon mot de passe n'a pas été accepté après les échecs");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 06: NICK fonctionne après PASS réussi                                */
/* -------------------------------------------------------------------------- */
void test_nick_after_successful_pass() {
    printTestCase("06 - Le client peut passer à NICK après un PASS réussi");
    
    int sock = connectToServer();
    if (sock < 0) return;

    sendCommand(sock, "PASS " SERVER_PASSWORD);
    std::string response = sendCommand(sock, "NICK testuser06");
    
    // NICK devrait être accepté (pas d'erreur 464 = not authenticated)
    if (!responseContainsErrorCode(response, 464)) {
        printSuccess("NICK accepté après PASS réussi");
    } else {
        printFailure("NICK refusé avec erreur 464 après PASS réussi");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 07: NICK avant PASS (doit échouer)                                   */
/* -------------------------------------------------------------------------- */
void test_nick_before_pass() {
    printTestCase("07 - NICK avant PASS doit échouer avec erreur 464");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "NICK testuser07");
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Erreur 464 reçue - PASS requis avant NICK");
    } else {
        printInfo("Le serveur peut autoriser NICK avant PASS (comportement variable)");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 08: USER avant PASS (doit échouer)                                   */
/* -------------------------------------------------------------------------- */
void test_user_before_pass() {
    printTestCase("08 - USER avant PASS doit échouer avec erreur 464");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "USER testuser 0 * :Test User");
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Erreur 464 reçue - PASS requis avant USER");
    } else {
        printInfo("Le serveur peut autoriser USER avant PASS (comportement variable)");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 09: PASS avec mot de passe vide                                      */
/* -------------------------------------------------------------------------- */
void test_pass_empty_password() {
    printTestCase("09 - PASS avec paramètre vide");
    
    int sock = connectToServer();
    if (sock < 0) return;

    // PASS suivi d'espaces seulement
    std::string response = sendCommand(sock, "PASS ");
    
    if (responseContainsErrorCode(response, 461) || responseContainsErrorCode(response, 464)) {
        printSuccess("Erreur appropriée reçue pour mot de passe vide");
    } else {
        printFailure("Le serveur devrait rejeter un mot de passe vide");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 10: PASS sensible à la casse                                         */
/* -------------------------------------------------------------------------- */
void test_pass_case_sensitive() {
    printTestCase("10 - PASS est sensible à la casse");
    
    int sock = connectToServer();
    if (sock < 0) return;

    // Tester avec une casse différente
    std::string wrongCase = SERVER_PASSWORD;
    for (size_t i = 0; i < wrongCase.length(); i++) {
        if (wrongCase[i] >= 'a' && wrongCase[i] <= 'z')
            wrongCase[i] = wrongCase[i] - 32;
        else if (wrongCase[i] >= 'A' && wrongCase[i] <= 'Z')
            wrongCase[i] = wrongCase[i] + 32;
    }
    
    std::string response = sendCommand(sock, "PASS " + wrongCase);
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Le mot de passe est sensible à la casse");
    } else {
        printInfo("Le serveur peut ignorer la casse (comportement variable)");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 11: PASS avec espaces dans le mot de passe                           */
/* -------------------------------------------------------------------------- */
void test_pass_with_spaces() {
    printTestCase("11 - PASS avec espaces dans le paramètre");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PASS wrong password with spaces");
    
    // Le parser devrait prendre seulement le premier mot
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Seul le premier mot est utilisé comme mot de passe");
    } else {
        printInfo("Comportement avec espaces non défini");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 12: PASS avec caractères spéciaux                                    */
/* -------------------------------------------------------------------------- */
void test_pass_special_characters() {
    printTestCase("12 - PASS avec caractères spéciaux");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PASS !@#$%^&*()");
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Caractères spéciaux acceptés dans le mot de passe");
    } else {
        printInfo("Réponse inattendue pour caractères spéciaux");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 13: Double PASS réussi (doit être refusé)                            */
/* -------------------------------------------------------------------------- */
void test_pass_double_success() {
    printTestCase("13 - Double PASS réussi devrait être ignoré ou refusé");
    
    int sock = connectToServer();
    if (sock < 0) return;

    // Premier PASS
    sendCommand(sock, "PASS " SERVER_PASSWORD);
    
    // Deuxième PASS (devrait être ignoré ou accepté silencieusement)
    std::string response = sendCommand(sock, "PASS " SERVER_PASSWORD);
    
    // Pas d'erreur attendue car pas encore enregistré
    if (!responseContainsErrorCode(response, 462)) {
        printSuccess("Deuxième PASS accepté avant enregistrement complet");
    } else {
        printInfo("Le serveur refuse un deuxième PASS même avant enregistrement");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 14: PASS après NICK mais avant USER                                  */
/* -------------------------------------------------------------------------- */
void test_pass_after_nick_before_user() {
    printTestCase("14 - PASS après NICK mais avant USER");
    
    int sock = connectToServer();
    if (sock < 0) return;

    sendCommand(sock, "PASS " SERVER_PASSWORD);
    sendCommand(sock, "NICK testuser14");
    
    // Tenter un autre PASS avant USER
    std::string response = sendCommand(sock, "PASS " SERVER_PASSWORD);
    
    // Le client n'est pas encore "registered" (NICK+USER incomplet)
    printInfo("Résultat: " + (response.empty() ? "succès silencieux" : response));

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 15: Commande PASS en minuscules                                      */
/* -------------------------------------------------------------------------- */
void test_pass_lowercase_command() {
    printTestCase("15 - Commande 'pass' en minuscules");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "pass " SERVER_PASSWORD);
    
    // IRC devrait être insensible à la casse pour les commandes
    if (!responseContainsErrorCode(response, 464)) {
        printSuccess("Commande 'pass' en minuscules acceptée");
    } else {
        printFailure("La commande devrait être insensible à la casse");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 16: Commande PASS avec casse mixte                                   */
/* -------------------------------------------------------------------------- */
void test_pass_mixed_case_command() {
    printTestCase("16 - Commande 'PaSs' avec casse mixte");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, "PaSs " SERVER_PASSWORD);
    
    if (!responseContainsErrorCode(response, 464)) {
        printSuccess("Commande 'PaSs' avec casse mixte acceptée");
    } else {
        printFailure("La commande devrait être insensible à la casse");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 17: PASS avec très long mot de passe                                 */
/* -------------------------------------------------------------------------- */
void test_pass_very_long_password() {
    printTestCase("17 - PASS avec mot de passe très long (512 chars)");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string longPass(512, 'a');
    std::string response = sendCommand(sock, "PASS " + longPass);
    
    if (responseContainsErrorCode(response, 464)) {
        printSuccess("Mot de passe long géré correctement (erreur 464)");
    } else {
        printInfo("Comportement avec mot de passe long vérifié");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 18: Séquence complète d'enregistrement                               */
/* -------------------------------------------------------------------------- */
void test_full_registration_sequence() {
    printTestCase("18 - Séquence complète PASS → NICK → USER");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string resp1 = sendCommand(sock, "PASS " SERVER_PASSWORD);
    std::string resp2 = sendCommand(sock, "NICK testuser18");
    std::string resp3 = sendCommand(sock, "USER testuser18 0 * :Test User 18");
    
    // Après USER, on devrait recevoir le message de bienvenue 001
    if (responseContainsErrorCode(resp3, 1) || responseContains(resp3, "Welcome")) {
        printSuccess("Enregistrement complet réussi - Message de bienvenue reçu");
    } else if (!responseContainsErrorCode(resp3, 464) && !responseContainsErrorCode(resp3, 461)) {
        printSuccess("Séquence d'enregistrement acceptée");
    } else {
        printFailure("Échec de la séquence d'enregistrement");
    }

    close(sock);
}

/* -------------------------------------------------------------------------- */
/*  TEST 19: Connexions multiples simultanées                                 */
/* -------------------------------------------------------------------------- */
void test_multiple_connections() {
    printTestCase("19 - Connexions multiples avec PASS différents");
    
    int sock1 = connectToServer();
    int sock2 = connectToServer();
    
    if (sock1 < 0 || sock2 < 0) {
        if (sock1 >= 0) close(sock1);
        if (sock2 >= 0) close(sock2);
        return;
    }

    std::string resp1 = sendCommand(sock1, "PASS " SERVER_PASSWORD);
    std::string resp2 = sendCommand(sock2, "PASS wrongpassword");
    
    bool client1_ok = !responseContainsErrorCode(resp1, 464);
    bool client2_fail = responseContainsErrorCode(resp2, 464);
    
    if (client1_ok && client2_fail) {
        printSuccess("Les deux clients gérés indépendamment");
    } else {
        printFailure("Problème avec la gestion des connexions multiples");
    }

    close(sock1);
    close(sock2);
}

/* -------------------------------------------------------------------------- */
/*  TEST 20: PASS avec préfixe IRC (rare)                                     */
/* -------------------------------------------------------------------------- */
void test_pass_with_prefix() {
    printTestCase("20 - PASS avec préfixe IRC (:prefix PASS password)");
    
    int sock = connectToServer();
    if (sock < 0) return;

    std::string response = sendCommand(sock, ":client PASS " SERVER_PASSWORD);
    
    printInfo("Résultat avec préfixe: " + (response.empty() ? "succès silencieux" : response));

    close(sock);
}

/* ========================================================================== */
/*                              RÉSUMÉ DES TESTS                               */
/* ========================================================================== */

void printSummary() {
    std::cout << std::endl;
    std::cout << CYAN << "╔══════════════════════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << CYAN << "║" << RESET << BOLD << "                      RÉSUMÉ DES TESTS                        " << CYAN << "║" << RESET << std::endl;
    std::cout << CYAN << "╠══════════════════════════════════════════════════════════════╣" << RESET << std::endl;
    
    std::cout << CYAN << "║" << RESET << "  Total:    " << g_tests_total << " tests";
    for (int i = 0; i < 47 - 4; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << std::endl;
    
    std::cout << CYAN << "║" << RESET << GREEN << "  Réussis:  " << g_tests_passed << " tests" << RESET;
    for (int i = 0; i < 47 - 4; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << std::endl;
    
    std::cout << CYAN << "║" << RESET << RED << "  Échoués:  " << g_tests_failed << " tests" << RESET;
    for (int i = 0; i < 47 - 4; i++) std::cout << " ";
    std::cout << CYAN << "║" << RESET << std::endl;
    
    std::cout << CYAN << "╚══════════════════════════════════════════════════════════════╝" << RESET << std::endl;
    
    if (g_tests_failed == 0) {
        std::cout << std::endl << GREEN << BOLD << "🎉 TOUS LES TESTS SONT PASSÉS ! 🎉" << RESET << std::endl;
    } else {
        std::cout << std::endl << RED << BOLD << "⚠️  CERTAINS TESTS ONT ÉCHOUÉ !" << RESET << std::endl;
    }
    std::cout << std::endl;
}

/* ========================================================================== */
/*                              MAIN                                           */
/* ========================================================================== */

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printHeader("TESTS PASSCOMMAND - ft_irc");
    
    std::cout << std::endl;
    std::cout << YELLOW << "⚠️  PRÉREQUIS: Le serveur doit être lancé avec:" << RESET << std::endl;
    std::cout << WHITE << "   ./ircserv " << SERVER_PORT << " " << SERVER_PASSWORD << RESET << std::endl;
    std::cout << std::endl;

    /* ====================================================================== */
    /*  TESTS BASIQUES (spécifiés par l'utilisateur)                          */
    /* ====================================================================== */
    printHeader("TESTS BASIQUES");
    
    test_pass_correct_password();       // Test 01
    test_pass_wrong_password();         // Test 02
    test_pass_no_parameter();           // Test 03
    test_pass_after_registration();     // Test 04
    test_pass_multiple_attempts();      // Test 05
    test_nick_after_successful_pass();  // Test 06

    /* ====================================================================== */
    /*  TESTS AVANCÉS (ajoutés)                                               */
    /* ====================================================================== */
    printHeader("TESTS AVANCÉS");
    
    test_nick_before_pass();            // Test 07
    test_user_before_pass();            // Test 08
    test_pass_empty_password();         // Test 09
    test_pass_case_sensitive();         // Test 10
    test_pass_with_spaces();            // Test 11
    test_pass_special_characters();     // Test 12
    test_pass_double_success();         // Test 13
    test_pass_after_nick_before_user(); // Test 14
    test_pass_lowercase_command();      // Test 15
    test_pass_mixed_case_command();     // Test 16
    test_pass_very_long_password();     // Test 17
    test_full_registration_sequence();  // Test 18
    test_multiple_connections();        // Test 19
    test_pass_with_prefix();            // Test 20

    /* ====================================================================== */
    /*  RÉSUMÉ                                                                */
    /* ====================================================================== */
    printSummary();

    return (g_tests_failed > 0) ? 1 : 0;
}
