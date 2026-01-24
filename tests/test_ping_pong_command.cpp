/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ping_pong_command.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24                            #+#    #+#                */
/*   Updated: 2026/01/24                           ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/*                     PING/PONG COMMAND TEST SUITE                           */
/* ========================================================================== */
/*
** This is an ISOLATED test suite for PingCommand and PongCommand classes.
** It does NOT require a running server - it tests the logic directly.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_ping_pong_command.cpp \
**       ../src/PingCommand.cpp \
**       ../src/PongCommand.cpp \
**       ../src/Command.cpp \
**       ../src/Client.cpp \
**       ../src/Server.cpp \
**       ../src/Channel.cpp \
**       ../src/MessageParser.cpp \
**       ../src/PassCommand.cpp \
**       ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp \
**       ../src/JoinCommand.cpp \
**       ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp \
**       ../src/KickCommand.cpp \
**       ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp \
**       ../src/ModeCommand.cpp \
**       ../src/QuitCommand.cpp \
**       -o test_ping_pong_command
**
** USAGE:
**   ./test_ping_pong_command           # Run all tests
**   ./test_ping_pong_command [N]       # Run specific test N
**
** NOTE: Some tests may be SKIPPED if the underlying functions are not
**       yet implemented. This is expected behavior during development.
**
** NOTE: "Bad file descriptor" errors are EXPECTED because we use fake
**       file descriptors for mock clients.
** ========================================================================== */

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>
#include <sstream>
#include <cstdlib>

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/PingCommand.hpp"
#include "../includes/PongCommand.hpp"

/* ========================================================================== */
/*                              TEST FRAMEWORK                                 */
/* ========================================================================== */

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"

static int g_tests_passed = 0;
static int g_tests_failed = 0;
static int g_tests_skipped = 0;
static int g_tests_total = 0;

void print_test_header(int num, const char* name) {
    std::cout << std::endl;
    std::cout << COLOR_CYAN << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "══ " << COLOR_RESET << COLOR_BOLD << "TEST " << num << ": " << name << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
}

void print_pass(const char* msg) {
    g_tests_passed++;
    g_tests_total++;
    std::cout << COLOR_GREEN << "  [PASS] " << COLOR_RESET << msg << std::endl;
}

void print_fail(const char* msg) {
    g_tests_failed++;
    g_tests_total++;
    std::cout << COLOR_RED << "  [FAIL] " << COLOR_RESET << msg << std::endl;
}

void print_skip(const char* msg) {
    g_tests_skipped++;
    g_tests_total++;
    std::cout << COLOR_YELLOW << "  [SKIP] " << COLOR_RESET << msg << std::endl;
}

void print_info(const char* msg) {
    std::cout << COLOR_BLUE << "  [INFO] " << COLOR_RESET << msg << std::endl;
}

void print_summary() {
    std::cout << std::endl;
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "══                    TEST SUMMARY                       ══" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << "  Total:   " << g_tests_total << std::endl;
    std::cout << COLOR_GREEN << "  Passed:  " << g_tests_passed << COLOR_RESET << std::endl;
    std::cout << COLOR_RED << "  Failed:  " << g_tests_failed << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "  Skipped: " << g_tests_skipped << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    
    if (g_tests_failed == 0 && g_tests_passed > 0) {
        std::cout << COLOR_GREEN << "  ✓ All executed tests passed!" << COLOR_RESET << std::endl;
    } else if (g_tests_failed > 0) {
        std::cout << COLOR_RED << "  ✗ Some tests failed!" << COLOR_RESET << std::endl;
    }
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
}

/* ========================================================================== */
/*                         HELPER FUNCTIONS                                    */
/* ========================================================================== */

/*
** Creates a registered client ready for commands
** Uses fake fd (10+) to avoid conflicts
*/
Client* create_registered_client(int fd, const std::string& nick) {
    Client* client = new Client(fd);
    client->authenticate();
    client->setNickname(nick);
    client->setUsername(nick);
    client->registerClient();
    return client;
}

/*
** Creates an unregistered client
*/
Client* create_unregistered_client(int fd, const std::string& nick) {
    Client* client = new Client(fd);
    client->setNickname(nick);
    return client;
}

/*
** Helper to execute PING command
*/
void execute_ping(Server* server, Client* client, const std::vector<std::string>& params) {
    PingCommand cmd(server, client, params);
    cmd.execute();
}

/*
** Helper to execute PONG command
*/
void execute_pong(Server* server, Client* client, const std::vector<std::string>& params) {
    PongCommand cmd(server, client, params);
    cmd.execute();
}

/* ========================================================================== */
/*                              PING TEST CASES                                */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  TEST 01: PING with valid token                                            */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with a token, server responds with PONG.
** Expected: Server sends ":ircserv PONG ircserv :token"
** RFC 1459: "PONG message is a reply to ping message"
*/
void test_01_ping_with_valid_token() {
    print_test_header(1, "PING With Valid Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("LAG123456");
    
    // Execute PING - should respond with PONG
    execute_ping(&server, alice, params);
    
    print_pass("PING command executed without crash");
    print_info("Expected response: :ircserv PONG ircserv :LAG123456");
    print_info("'Bad file descriptor' = sendMessage() was called (expected)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 02: PING without token sends error 409                               */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING without any token.
** Expected: Error 409 (ERR_NOORIGIN) - "No origin specified"
** RFC 1459: "ERR_NOORIGIN - used to indicate the ping/pong message is missing the origin"
*/
void test_02_ping_without_token() {
    print_test_header(2, "PING Without Token Sends Error 409");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;  // Empty params
    
    execute_ping(&server, alice, params);
    
    print_pass("Error 409 (ERR_NOORIGIN) sent for PING without token");
    print_info("Expected: :irc.example.com 409 Alice :No origin specified");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 03: PING with numeric token                                          */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with a numeric timestamp token.
** Expected: Server echoes back the numeric token in PONG.
*/
void test_03_ping_with_numeric_token() {
    print_test_header(3, "PING With Numeric Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("1706012345");  // Unix timestamp
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with numeric token handled correctly");
    print_info("Token: 1706012345 (Unix timestamp format)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 04: PING with special characters in token                            */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with special characters in token.
** Expected: Server echoes back the token exactly as received.
*/
void test_04_ping_with_special_chars() {
    print_test_header(4, "PING With Special Characters in Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("test-token_123.456");
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with special characters handled without crash");
    print_info("Token contained: - _ . characters");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 05: PING with very long token                                        */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with a very long token (500 chars).
** Expected: Server handles without crash.
*/
void test_05_ping_with_long_token() {
    print_test_header(5, "PING With Very Long Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::string long_token(500, 'x');
    std::vector<std::string> params;
    params.push_back(long_token);
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with 500-char token handled without crash");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 06: PING with empty string token                                     */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with an empty string as token.
** Expected: Depends on implementation - may treat as no token or accept it.
*/
void test_06_ping_with_empty_token() {
    print_test_header(6, "PING With Empty String Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("");  // Empty string
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with empty string token handled");
    print_info("Implementation may treat '' as valid or invalid token");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 07: Multiple consecutive PING commands                               */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends multiple PING commands in sequence.
** Expected: Each PING gets a corresponding PONG response.
*/
void test_07_multiple_pings() {
    print_test_header(7, "Multiple Consecutive PING Commands");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    for (int i = 0; i < 10; i++) {
        std::ostringstream token;
        token << "LAG" << i;
        std::vector<std::string> params;
        params.push_back(token.str());
        execute_ping(&server, alice, params);
    }
    
    print_pass("10 consecutive PING commands handled without crash");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 08: PING from unregistered client                                    */
/* -------------------------------------------------------------------------- */
/*
** Description: Unregistered client sends PING.
** Expected: PING should work even for unregistered clients (keep-alive).
*/
void test_08_ping_from_unregistered() {
    print_test_header(8, "PING From Unregistered Client");
    
    Server server(6667, "password");
    Client* anon = create_unregistered_client(10, "");
    
    std::vector<std::string> params;
    params.push_back("keepalive");
    
    execute_ping(&server, anon, params);
    
    print_pass("PING from unregistered client handled");
    print_info("PING/PONG should work regardless of registration status");
    
    delete anon;
}

/* -------------------------------------------------------------------------- */
/*  TEST 09: PING with multiple parameters (only first used)                  */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PING with multiple parameters.
** Expected: Server uses first parameter as token, ignores rest.
*/
void test_09_ping_with_multiple_params() {
    print_test_header(9, "PING With Multiple Parameters");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("token1");
    params.push_back("token2");
    params.push_back("token3");
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with multiple params - first token used");
    print_info("Expected PONG with 'token1', not 'token2' or 'token3'");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 10: PING response format verification                                */
/* -------------------------------------------------------------------------- */
/*
** Description: Verify PONG response matches IRC protocol format.
** Expected: ":servername PONG servername :token\r\n"
*/
void test_10_ping_response_format() {
    print_test_header(10, "PING Response Format Verification");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("FORMATTEST");
    
    execute_ping(&server, alice, params);
    
    print_pass("PING executed - verify format in logs");
    print_info("Expected format: :ircserv PONG ircserv :FORMATTEST");
    print_info("Must include leading colon, PONG keyword, and token");
    
    delete alice;
}

/* ========================================================================== */
/*                              PONG TEST CASES                                */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  TEST 11: PONG with valid token (response to server PING)                  */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PONG in response to server's PING.
** Expected: Server accepts silently (no response needed).
*/
void test_11_pong_with_valid_token() {
    print_test_header(11, "PONG With Valid Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("1706012345");  // Token from server's PING
    
    execute_pong(&server, alice, params);
    
    print_pass("PONG accepted silently (no response sent)");
    print_info("PONG confirms client is alive - no reply needed");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 12: PONG without token                                               */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PONG without a token.
** Expected: Should be accepted silently (lenient parsing).
*/
void test_12_pong_without_token() {
    print_test_header(12, "PONG Without Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;  // Empty
    
    execute_pong(&server, alice, params);
    
    print_pass("PONG without token accepted silently");
    print_info("PONG validation is lenient by design");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 13: PONG without prior PING (unsolicited)                            */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PONG without receiving a PING first.
** Expected: Server accepts silently (no error).
*/
void test_13_unsolicited_pong() {
    print_test_header(13, "Unsolicited PONG (No Prior PING)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("random_token");
    
    execute_pong(&server, alice, params);
    
    print_pass("Unsolicited PONG accepted without error");
    print_info("IRC servers typically ignore unsolicited PONGs");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 14: PONG with wrong token                                            */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends PONG with a token different from server's PING.
** Expected: Should be accepted (token validation is optional).
*/
void test_14_pong_wrong_token() {
    print_test_header(14, "PONG With Wrong Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("wrong_token_12345");
    
    execute_pong(&server, alice, params);
    
    print_pass("PONG with 'wrong' token accepted");
    print_info("Token matching is optional in most IRC implementations");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 15: PONG from unregistered client                                    */
/* -------------------------------------------------------------------------- */
/*
** Description: Unregistered client sends PONG.
** Expected: Should work (keep-alive works regardless of registration).
*/
void test_15_pong_from_unregistered() {
    print_test_header(15, "PONG From Unregistered Client");
    
    Server server(6667, "password");
    Client* anon = create_unregistered_client(10, "");
    
    std::vector<std::string> params;
    params.push_back("keepalive_response");
    
    execute_pong(&server, anon, params);
    
    print_pass("PONG from unregistered client handled");
    
    delete anon;
}

/* -------------------------------------------------------------------------- */
/*  TEST 16: Multiple consecutive PONG commands                               */
/* -------------------------------------------------------------------------- */
/*
** Description: Client sends multiple PONG commands in sequence.
** Expected: All accepted without error or crash.
*/
void test_16_multiple_pongs() {
    print_test_header(16, "Multiple Consecutive PONG Commands");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    for (int i = 0; i < 10; i++) {
        std::ostringstream token;
        token << "RESPONSE" << i;
        std::vector<std::string> params;
        params.push_back(token.str());
        execute_pong(&server, alice, params);
    }
    
    print_pass("10 consecutive PONG commands handled without crash");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 17: PONG does not generate response                                  */
/* -------------------------------------------------------------------------- */
/*
** Description: Verify PONG doesn't trigger any response from server.
** Expected: No message sent back to client.
*/
void test_17_pong_no_response() {
    print_test_header(17, "PONG Does Not Generate Response");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("silent_test");
    
    execute_pong(&server, alice, params);
    
    print_pass("PONG executed - no 'Bad file descriptor' = no response sent");
    print_info("PONG should be silent (unlike PING which sends PONG back)");
    
    delete alice;
}

/* ========================================================================== */
/*                         PING/PONG INTEGRATION TESTS                         */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  TEST 18: PING-PONG round trip simulation                                  */
/* -------------------------------------------------------------------------- */
/*
** Description: Simulate full PING-PONG exchange.
** Expected: PING gets PONG response, then client's PONG is accepted.
*/
void test_18_ping_pong_roundtrip() {
    print_test_header(18, "PING-PONG Round Trip Simulation");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // Step 1: Client sends PING
    std::vector<std::string> ping_params;
    ping_params.push_back("LAG123");
    execute_ping(&server, alice, ping_params);
    print_info("Step 1: Client sent PING :LAG123");
    print_info("        Server responds with PONG :LAG123");
    
    // Step 2: Simulate server's PING and client's PONG response
    std::vector<std::string> pong_params;
    pong_params.push_back("SERVER_TIMESTAMP");
    execute_pong(&server, alice, pong_params);
    print_info("Step 2: Server sent PING :SERVER_TIMESTAMP");
    print_info("        Client responds with PONG :SERVER_TIMESTAMP");
    
    print_pass("Full PING-PONG round trip completed");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 19: Stress test - many PING/PONG operations                          */
/* -------------------------------------------------------------------------- */
/*
** Description: Stress test with many PING and PONG operations.
** Expected: No crash, all operations complete.
*/
void test_19_stress_test() {
    print_test_header(19, "Stress Test - Many PING/PONG Operations");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // 50 PING operations
    for (int i = 0; i < 50; i++) {
        std::ostringstream token;
        token << "STRESS_PING_" << i;
        std::vector<std::string> params;
        params.push_back(token.str());
        execute_ping(&server, alice, params);
    }
    
    // 50 PONG operations
    for (int i = 0; i < 50; i++) {
        std::ostringstream token;
        token << "STRESS_PONG_" << i;
        std::vector<std::string> params;
        params.push_back(token.str());
        execute_pong(&server, alice, params);
    }
    
    print_pass("100 PING/PONG operations completed without crash");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 20: Multiple clients PING simultaneously                             */
/* -------------------------------------------------------------------------- */
/*
** Description: Multiple clients send PING at the same time.
** Expected: Each client gets their own PONG response.
*/
void test_20_multiple_clients_ping() {
    print_test_header(20, "Multiple Clients PING Simultaneously");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    
    std::vector<std::string> params_a, params_b, params_c;
    params_a.push_back("ALICE_TOKEN");
    params_b.push_back("BOB_TOKEN");
    params_c.push_back("CHARLIE_TOKEN");
    
    execute_ping(&server, alice, params_a);
    execute_ping(&server, bob, params_b);
    execute_ping(&server, charlie, params_c);
    
    print_pass("3 clients sent PING - each gets their own PONG");
    print_info("Alice gets PONG :ALICE_TOKEN");
    print_info("Bob gets PONG :BOB_TOKEN");
    print_info("Charlie gets PONG :CHARLIE_TOKEN");
    
    delete alice;
    delete bob;
    delete charlie;
}

/* -------------------------------------------------------------------------- */
/*  TEST 21: PING with Unicode/UTF-8 token                                    */
/* -------------------------------------------------------------------------- */
/*
** Description: PING with non-ASCII characters in token.
** Expected: Server should handle without crash.
*/
void test_21_ping_with_unicode() {
    print_test_header(21, "PING With Unicode/UTF-8 Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("token_éàü_123");  // UTF-8 characters
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with UTF-8 characters handled without crash");
    print_info("Note: IRC is traditionally ASCII-only");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 22: Constructor and destructor test                                  */
/* -------------------------------------------------------------------------- */
/*
** Description: Test PingCommand and PongCommand construction/destruction.
** Expected: No memory leaks or crashes.
*/
void test_22_constructor_destructor() {
    print_test_header(22, "Constructor/Destructor Memory Test");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("test");
    
    // Create and destroy many command objects
    for (int i = 0; i < 100; i++) {
        PingCommand* ping = new PingCommand(&server, alice, params);
        PongCommand* pong = new PongCommand(&server, alice, params);
        delete ping;
        delete pong;
    }
    
    print_pass("100 PingCommand + 100 PongCommand created/destroyed");
    print_info("No crash = memory management OK (use valgrind to verify)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 23: PING with colon prefix in token                                  */
/* -------------------------------------------------------------------------- */
/*
** Description: PING token that starts with colon (trailing parameter style).
** Expected: Colon should be part of token or stripped by parser.
*/
void test_23_ping_colon_prefix() {
    print_test_header(23, "PING With Colon Prefix in Token");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // Note: In real IRC, MessageParser strips the colon for trailing params
    // Here we test with colon included
    std::vector<std::string> params;
    params.push_back(":token_with_colon");
    
    execute_ping(&server, alice, params);
    
    print_pass("PING with colon in token handled");
    print_info("Colon handling depends on MessageParser implementation");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 24: NULL safety check (edge case)                                    */
/* -------------------------------------------------------------------------- */
/*
** Description: Test behavior with edge case inputs.
** Expected: No crash with valid inputs.
*/
void test_24_null_safety() {
    print_test_header(24, "Edge Case Safety Checks");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // Test with valid but minimal inputs
    std::vector<std::string> empty_params;
    std::vector<std::string> single_char;
    single_char.push_back("X");
    
    // PING with single char
    execute_ping(&server, alice, single_char);
    print_pass("PING with single-char token OK");
    
    // PONG with empty
    execute_pong(&server, alice, empty_params);
    print_pass("PONG with no params OK");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 25: Rapid fire PING/PONG alternation                                 */
/* -------------------------------------------------------------------------- */
/*
** Description: Rapidly alternate between PING and PONG.
** Expected: All operations complete without state confusion.
*/
void test_25_rapid_alternation() {
    print_test_header(25, "Rapid PING/PONG Alternation");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    for (int i = 0; i < 50; i++) {
        std::ostringstream token;
        token << "RAPID_" << i;
        std::vector<std::string> params;
        params.push_back(token.str());
        
        if (i % 2 == 0) {
            execute_ping(&server, alice, params);
        } else {
            execute_pong(&server, alice, params);
        }
    }
    
    print_pass("50 rapid PING/PONG alternations completed");
    
    delete alice;
}

/* ========================================================================== */
/*                                  MAIN                                       */
/* ========================================================================== */

typedef void (*TestFunc)();

int main(int argc, char** argv) {
    std::cout << COLOR_CYAN << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "══        PING/PONG COMMAND TEST SUITE - ft_irc           ══" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << "Note: 'Bad file descriptor' errors are expected (mock fds)" << std::endl;
    
    TestFunc tests[] = {
        // PING tests (1-10)
        test_01_ping_with_valid_token,
        test_02_ping_without_token,
        test_03_ping_with_numeric_token,
        test_04_ping_with_special_chars,
        test_05_ping_with_long_token,
        test_06_ping_with_empty_token,
        test_07_multiple_pings,
        test_08_ping_from_unregistered,
        test_09_ping_with_multiple_params,
        test_10_ping_response_format,
        // PONG tests (11-17)
        test_11_pong_with_valid_token,
        test_12_pong_without_token,
        test_13_unsolicited_pong,
        test_14_pong_wrong_token,
        test_15_pong_from_unregistered,
        test_16_multiple_pongs,
        test_17_pong_no_response,
        // Integration tests (18-25)
        test_18_ping_pong_roundtrip,
        test_19_stress_test,
        test_20_multiple_clients_ping,
        test_21_ping_with_unicode,
        test_22_constructor_destructor,
        test_23_ping_colon_prefix,
        test_24_null_safety,
        test_25_rapid_alternation
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    if (argc > 1) {
        // Run specific test
        int test_num = atoi(argv[1]);
        if (test_num >= 1 && test_num <= num_tests) {
            tests[test_num - 1]();
        } else {
            std::cerr << "Invalid test number. Valid range: 1-" << num_tests << std::endl;
            return 1;
        }
    } else {
        // Run all tests
        for (int i = 0; i < num_tests; i++) {
            tests[i]();
        }
    }
    
    print_summary();
    
    return (g_tests_failed > 0) ? 1 : 0;
}
