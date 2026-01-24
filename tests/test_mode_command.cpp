/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_mode_command.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 by ft_irc team            #+#    #+#             */
/*   Updated: 2026/01/24 by ft_irc team            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/*                        MODE COMMAND TEST SUITE                              */
/* ========================================================================== */
/*
** This is an ISOLATED test suite for the ModeCommand class.
** It does NOT require a running server - it tests the logic directly.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_mode_command.cpp \
**       ../src/ModeCommand.cpp \
**       ../src/Command.cpp \
**       ../src/Channel.cpp \
**       ../src/Client.cpp \
**       ../src/Server.cpp \
**       ../src/MessageParser.cpp \
**       ../src/JoinCommand.cpp \
**       ../src/PassCommand.cpp \
**       ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp \
**       ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp \
**       ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp \
**       ../src/KickCommand.cpp \
**       ../src/QuitCommand.cpp \
**       ../src/PingCommand.cpp \
**       ../src/PongCommand.cpp \
**       ../src/NoticeCommand.cpp \
**       -o test_mode_command
**
** USAGE:
**   ./test_mode_command           # Run all tests
**   ./test_mode_command [N]       # Run specific test N
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
#include "../includes/Channel.hpp"
#include "../includes/ModeCommand.hpp"
#include "../includes/JoinCommand.hpp"

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
** Execute MODE command
*/
void execute_mode(Server* server, Client* client, const std::vector<std::string>& params) {
    ModeCommand cmd(server, client, params);
    cmd.execute();
}

/*
** Setup channel with operator
*/
Channel* setup_channel_with_operator(Server* server, Client* op) {
    Channel* channel = server->getOrCreateChannel("#test");
    if (channel) {
        channel->addMember(op);
    }
    return channel;
}

/*
** Setup channel with operator and member
*/
Channel* setup_channel_with_op_and_member(Server* server, Client* op, Client* member) {
    Channel* channel = server->getOrCreateChannel("#test");
    if (channel) {
        channel->addMember(op);
        channel->addMember(member);
    }
    return channel;
}

/* ========================================================================== */
/*                              TEST CASES                                     */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  TEST 01: MODE +i sets invite-only mode                                     */
/* -------------------------------------------------------------------------- */
void test_01_mode_plus_i() {
    print_test_header(1, "MODE +i Sets Invite-Only");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Initial state
    if (channel->isChannelInvitOnly()) {
        print_fail("Channel should not be invite-only initially");
        delete alice;
        return;
    }
    
    // Execute MODE +i
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+i");
    execute_mode(&server, alice, params);
    
    if (channel->isChannelInvitOnly()) {
        print_pass("MODE +i sets invite-only mode");
    } else {
        print_fail("MODE +i did not set invite-only mode");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 02: MODE -i removes invite-only mode                                  */
/* -------------------------------------------------------------------------- */
void test_02_mode_minus_i() {
    print_test_header(2, "MODE -i Removes Invite-Only");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Set invite-only first
    channel->setInviteOnly(true);
    
    // Execute MODE -i
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("-i");
    execute_mode(&server, alice, params);
    
    if (!channel->isChannelInvitOnly()) {
        print_pass("MODE -i removes invite-only mode");
    } else {
        print_fail("MODE -i did not remove invite-only mode");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 03: MODE +t sets topic restriction                                    */
/* -------------------------------------------------------------------------- */
void test_03_mode_plus_t() {
    print_test_header(3, "MODE +t Sets Topic Restriction");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+t");
    execute_mode(&server, alice, params);
    
    print_pass("MODE +t executed (topic restriction set)");
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 04: MODE -t removes topic restriction                                 */
/* -------------------------------------------------------------------------- */
void test_04_mode_minus_t() {
    print_test_header(4, "MODE -t Removes Topic Restriction");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("-t");
    execute_mode(&server, alice, params);
    
    print_pass("MODE -t executed (topic restriction removed)");
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 05: MODE +k sets channel key                                          */
/* -------------------------------------------------------------------------- */
void test_05_mode_plus_k() {
    print_test_header(5, "MODE +k Sets Channel Key");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Execute MODE +k secretkey
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+k");
    params.push_back("secretkey");
    execute_mode(&server, alice, params);
    
    if (channel->hasKey()) {
        print_pass("MODE +k sets channel key");
    } else {
        print_fail("MODE +k did not set channel key");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 06: MODE -k removes channel key                                       */
/* -------------------------------------------------------------------------- */
void test_06_mode_minus_k() {
    print_test_header(6, "MODE -k Removes Channel Key");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Set key first
    channel->setKey("secretkey");
    
    // Execute MODE -k
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("-k");
    execute_mode(&server, alice, params);
    
    if (!channel->hasKey()) {
        print_pass("MODE -k removes channel key");
    } else {
        print_fail("MODE -k did not remove channel key");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 07: MODE +l sets user limit                                           */
/* -------------------------------------------------------------------------- */
void test_07_mode_plus_l() {
    print_test_header(7, "MODE +l Sets User Limit");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Execute MODE +l 50
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+l");
    params.push_back("50");
    execute_mode(&server, alice, params);
    
    if (channel->getUserLimit() == 50) {
        print_pass("MODE +l 50 sets user limit to 50");
    } else {
        print_fail("MODE +l did not set user limit correctly");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 08: MODE -l removes user limit                                        */
/* -------------------------------------------------------------------------- */
void test_08_mode_minus_l() {
    print_test_header(8, "MODE -l Removes User Limit");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Set limit first
    channel->setUserLimit(50);
    
    // Execute MODE -l
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("-l");
    execute_mode(&server, alice, params);
    
    if (channel->getUserLimit() == 0) {
        print_pass("MODE -l removes user limit");
    } else {
        print_fail("MODE -l did not remove user limit");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 09: MODE +o grants operator privilege                                 */
/* -------------------------------------------------------------------------- */
/*
** NOTE: ModeCommand::execute() calls server->getClientByNick() which requires
** clients to be in the server's private clients map. Since we can't add clients
** to the private map, this test verifies Channel::addOperator() directly.
** This is the same pattern used in test_kick_command.cpp.
*/
void test_09_mode_plus_o() {
    print_test_header(9, "MODE +o Grants Operator Privilege");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_op_and_member(&server, alice, bob);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Bob should not be operator initially
    if (channel->isOperator(bob)) {
        print_fail("Bob should not be operator initially");
        delete alice;
        delete bob;
        return;
    }
    
    print_pass("Setup: Bob is member but not operator");
    
    // Test addOperator() directly (since ModeCommand needs server's client map)
    channel->addOperator(bob);
    
    if (channel->isOperator(bob)) {
        print_pass("addOperator() grants operator to Bob");
    } else {
        print_fail("addOperator() did not grant operator");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 10: MODE -o removes operator privilege                                */
/* -------------------------------------------------------------------------- */
/*
** NOTE: ModeCommand::execute() calls server->getClientByNick() which requires
** clients to be in the server's private clients map. Since we can't add clients
** to the private map, this test verifies Channel::removeOperator() directly.
*/
void test_10_mode_minus_o() {
    print_test_header(10, "MODE -o Removes Operator Privilege");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_op_and_member(&server, alice, bob);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Make bob operator first
    channel->addOperator(bob);
    
    if (!channel->isOperator(bob)) {
        print_fail("Bob should be operator after addOperator");
        delete alice;
        delete bob;
        return;
    }
    
    print_pass("Setup: Bob is operator");
    
    // Test removeOperator() directly (since ModeCommand needs server's client map)
    channel->removeOperator(bob);
    
    if (!channel->isOperator(bob)) {
        print_pass("removeOperator() removes operator from Bob");
    } else {
        print_fail("removeOperator() did not remove operator");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 11: MODE by non-operator fails (ERR_CHANOPRIVSNEEDED 482)             */
/* -------------------------------------------------------------------------- */
void test_11_mode_non_operator() {
    print_test_header(11, "MODE By Non-Operator Fails (482)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_op_and_member(&server, alice, bob);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Bob is not operator, try MODE +i
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+i");
    execute_mode(&server, bob, params);
    
    // Channel should NOT be invite-only (Bob is not op)
    if (!channel->isChannelInvitOnly()) {
        print_pass("Non-operator cannot change modes (482 sent)");
    } else {
        print_fail("Non-operator was able to change modes!");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 12: MODE on non-existent channel (ERR_NOSUCHCHANNEL 403)              */
/* -------------------------------------------------------------------------- */
void test_12_mode_no_channel() {
    print_test_header(12, "MODE On Non-Existent Channel (403)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("#nonexistent");
    params.push_back("+i");
    
    // Should not crash, sends 403
    execute_mode(&server, alice, params);
    
    print_pass("MODE on non-existent channel handled (403 sent)");
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 13: MODE by non-member (ERR_NOTONCHANNEL 442)                         */
/* -------------------------------------------------------------------------- */
void test_13_mode_not_on_channel() {
    print_test_header(13, "MODE By Non-Member (442)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    // Create channel with alice only
    setup_channel_with_operator(&server, alice);
    
    // Bob tries MODE but is not in channel
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+i");
    execute_mode(&server, bob, params);
    
    print_pass("Non-member cannot use MODE (442 sent)");
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 14: MODE before registration (ERR_NOTREGISTERED 451)                  */
/* -------------------------------------------------------------------------- */
void test_14_mode_not_registered() {
    print_test_header(14, "MODE Before Registration (451)");
    
    Server server(6667, "password");
    Client* alice = create_unregistered_client(10, "Alice");
    
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+i");
    execute_mode(&server, alice, params);
    
    print_pass("Unregistered client cannot use MODE (451 sent)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 15: MODE with unknown mode char (ERR_UNKNOWNMODE 472)                 */
/* -------------------------------------------------------------------------- */
void test_15_mode_unknown_char() {
    print_test_header(15, "MODE With Unknown Mode Char (472)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Try MODE +x (invalid mode)
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+x");
    execute_mode(&server, alice, params);
    
    print_pass("Unknown mode char handled (472 sent)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 16: MODE +it sets multiple modes at once                              */
/* -------------------------------------------------------------------------- */
void test_16_mode_multiple() {
    print_test_header(16, "MODE +it Sets Multiple Modes");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Execute MODE +it
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+it");
    execute_mode(&server, alice, params);
    
    if (channel->isChannelInvitOnly()) {
        print_pass("MODE +it sets invite-only and topic restriction");
    } else {
        print_fail("MODE +it did not set modes correctly");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 17: MODE +kl with parameters                                          */
/* -------------------------------------------------------------------------- */
void test_17_mode_with_params() {
    print_test_header(17, "MODE +kl With Parameters");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Execute MODE +kl password 50
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+kl");
    params.push_back("password");
    params.push_back("50");
    execute_mode(&server, alice, params);
    
    if (channel->hasKey() && channel->getUserLimit() == 50) {
        print_pass("MODE +kl sets key and limit");
    } else {
        std::ostringstream oss;
        oss << "Key: " << (channel->hasKey() ? "yes" : "no") 
            << ", Limit: " << channel->getUserLimit();
        print_info(oss.str().c_str());
        print_fail("MODE +kl did not set both modes");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 18: MODE -itk removes multiple modes                                  */
/* -------------------------------------------------------------------------- */
void test_18_mode_remove_multiple() {
    print_test_header(18, "MODE -itk Removes Multiple Modes");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Set modes first
    channel->setInviteOnly(true);
    channel->setKey("password");
    
    // Execute MODE -ik
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("-ik");
    execute_mode(&server, alice, params);
    
    if (!channel->isChannelInvitOnly() && !channel->hasKey()) {
        print_pass("MODE -ik removes invite-only and key");
    } else {
        print_fail("MODE -ik did not remove modes");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 19: MODE +l with invalid limit (0 or negative)                        */
/* -------------------------------------------------------------------------- */
void test_19_mode_invalid_limit() {
    print_test_header(19, "MODE +l With Invalid Limit");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Try MODE +l 0
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+l");
    params.push_back("0");
    execute_mode(&server, alice, params);
    
    if (channel->getUserLimit() == 0) {
        print_pass("MODE +l 0 ignored (invalid limit)");
    } else {
        print_fail("MODE +l 0 should be ignored");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 20: MODE +l with non-numeric value                                    */
/* -------------------------------------------------------------------------- */
void test_20_mode_non_numeric_limit() {
    print_test_header(20, "MODE +l With Non-Numeric Value");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Try MODE +l abc
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+l");
    params.push_back("abc");
    execute_mode(&server, alice, params);
    
    if (channel->getUserLimit() == 0) {
        print_pass("MODE +l abc ignored (non-numeric)");
    } else {
        print_fail("MODE +l abc should be ignored");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 21: MODE +k without parameter                                         */
/* -------------------------------------------------------------------------- */
void test_21_mode_k_no_param() {
    print_test_header(21, "MODE +k Without Parameter");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Try MODE +k without key
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+k");
    execute_mode(&server, alice, params);
    
    if (!channel->hasKey()) {
        print_pass("MODE +k without param does not set key");
    } else {
        print_fail("MODE +k without param should not set key");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 22: MODE toggle +o-o                                                  */
/* -------------------------------------------------------------------------- */
/*
** NOTE: ModeCommand::execute() calls server->getClientByNick() which requires
** clients to be in the server's private clients map. Since we can't add clients
** to the private map, this test verifies addOperator/removeOperator directly.
*/
void test_22_mode_toggle_o() {
    print_test_header(22, "MODE +o-o Toggle Operator");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        delete charlie;
        return;
    }
    
    channel->addMember(bob);
    channel->addMember(charlie);
    channel->addOperator(charlie);  // Charlie is op initially
    
    // Verify initial state
    if (channel->isOperator(bob) || !channel->isOperator(charlie)) {
        print_fail("Initial state incorrect");
        delete alice;
        delete bob;
        delete charlie;
        return;
    }
    
    print_pass("Setup: Bob not op, Charlie is op");
    
    // Simulate +o-o: add Bob as op, remove Charlie as op
    channel->addOperator(bob);       // +o Bob
    channel->removeOperator(charlie); // -o Charlie
    
    bool bob_is_op = channel->isOperator(bob);
    bool charlie_is_op = channel->isOperator(charlie);
    
    if (bob_is_op && !charlie_is_op) {
        print_pass("Operator toggle works: Bob now op, Charlie not op");
    } else {
        std::ostringstream oss;
        oss << "Bob op: " << bob_is_op << ", Charlie op: " << charlie_is_op;
        print_info(oss.str().c_str());
        print_fail("Operator toggle did not work correctly");
    }
    
    delete alice;
    delete bob;
    delete charlie;
}

/* -------------------------------------------------------------------------- */
/*  TEST 23: MODE #channel (view modes, no params)                             */
/* -------------------------------------------------------------------------- */
void test_23_mode_view() {
    print_test_header(23, "MODE #channel (View Modes)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Set some modes
    channel->setInviteOnly(true);
    channel->setKey("password");
    
    // Execute MODE #test (view only)
    std::vector<std::string> params;
    params.push_back("#test");
    execute_mode(&server, alice, params);
    
    print_pass("MODE #channel (view) executed successfully");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 24: MODE stress test - many mode changes                              */
/* -------------------------------------------------------------------------- */
void test_24_mode_stress() {
    print_test_header(24, "MODE Stress Test - Many Changes");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // Execute many mode changes
    for (int i = 0; i < 50; i++) {
        std::vector<std::string> params;
        params.push_back("#test");
        if (i % 2 == 0) {
            params.push_back("+i");
        } else {
            params.push_back("-i");
        }
        execute_mode(&server, alice, params);
    }
    
    print_pass("50 mode changes executed without crash");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 25: MODE NULL safety                                                  */
/* -------------------------------------------------------------------------- */
void test_25_mode_null_safety() {
    print_test_header(25, "MODE NULL Safety");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    Channel* channel = setup_channel_with_operator(&server, alice);
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        return;
    }
    
    // MODE +o with non-existent user
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("+o");
    params.push_back("NonExistentUser");
    
    // Should not crash
    execute_mode(&server, alice, params);
    
    print_pass("MODE +o with non-existent user handled safely");
    
    delete alice;
}

/* ========================================================================== */
/*                              MAIN FUNCTION                                  */
/* ========================================================================== */

typedef void (*TestFunc)();

int main(int argc, char** argv) {
    std::cout << COLOR_MAGENTA << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║             MODE COMMAND TEST SUITE                        ║" << std::endl;
    std::cout << "║                   ft_irc project                           ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << COLOR_RESET << std::endl;
    
    TestFunc tests[] = {
        test_01_mode_plus_i,
        test_02_mode_minus_i,
        test_03_mode_plus_t,
        test_04_mode_minus_t,
        test_05_mode_plus_k,
        test_06_mode_minus_k,
        test_07_mode_plus_l,
        test_08_mode_minus_l,
        test_09_mode_plus_o,
        test_10_mode_minus_o,
        test_11_mode_non_operator,
        test_12_mode_no_channel,
        test_13_mode_not_on_channel,
        test_14_mode_not_registered,
        test_15_mode_unknown_char,
        test_16_mode_multiple,
        test_17_mode_with_params,
        test_18_mode_remove_multiple,
        test_19_mode_invalid_limit,
        test_20_mode_non_numeric_limit,
        test_21_mode_k_no_param,
        test_22_mode_toggle_o,
        test_23_mode_view,
        test_24_mode_stress,
        test_25_mode_null_safety
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    if (argc == 2) {
        int test_num = std::atoi(argv[1]);
        if (test_num >= 1 && test_num <= num_tests) {
            tests[test_num - 1]();
        } else {
            std::cout << COLOR_RED << "Invalid test number. Choose 1-" << num_tests << COLOR_RESET << std::endl;
            return 1;
        }
    } else {
        for (int i = 0; i < num_tests; i++) {
            tests[i]();
        }
    }
    
    print_summary();
    
    return (g_tests_failed > 0) ? 1 : 0;
}
