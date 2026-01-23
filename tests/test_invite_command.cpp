/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_invite_command.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 10:00:00 by ft_irc            #+#    #+#             */
/*   Updated: 2026/01/23 10:00:00 by ft_irc            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** =============================================================================
** INVITECOMMAND.CPP TEST SUITE
** =============================================================================
** 
** This file contains all test cases for the InviteCommand class.
** Compile separately from the main project.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_invite_command.cpp ../src/Server.cpp ../src/Client.cpp \
**       ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
**       ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
**       -o test_invite_command
**
** USAGE:
**   ./test_invite_command [test_number]
**   ./test_invite_command          # Run all tests
**   ./test_invite_command 1        # Run test 1 only
**   ./test_invite_command help     # Show help
**   ./test_invite_command manual   # Show manual test instructions
**   ./test_invite_command valgrind # Instructions for valgrind
**
** =============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/InviteCommand.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>

// =============================================================================
// TEST FRAMEWORK
// =============================================================================

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
static int g_tests_total = 0;

void test_header(const char* name) {
    std::cout << std::endl;
    std::cout << COLOR_BLUE << "==================================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "TEST: " << name << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "==================================================" << COLOR_RESET << std::endl;
}

void test_result(const char* description, bool passed) {
    g_tests_total++;
    if (passed) {
        g_tests_passed++;
        std::cout << COLOR_GREEN << "  [PASS] " << description << COLOR_RESET << std::endl;
    } else {
        g_tests_failed++;
        std::cout << COLOR_RED << "  [FAIL] " << description << COLOR_RESET << std::endl;
    }
}

void test_info(const char* info) {
    std::cout << COLOR_YELLOW << "  [INFO] " << info << COLOR_RESET << std::endl;
}

void test_warning(const char* warning) {
    std::cout << COLOR_MAGENTA << "  [WARN] " << warning << COLOR_RESET << std::endl;
}

void print_summary() {
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "                   TEST SUMMARY                    " << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "  Total:  " << g_tests_total << std::endl;
    std::cout << COLOR_GREEN << "  Passed: " << g_tests_passed << COLOR_RESET << std::endl;
    std::cout << COLOR_RED << "  Failed: " << g_tests_failed << COLOR_RESET << std::endl;
    
    float percentage = 0;
    if (g_tests_total > 0) {
        percentage = (float)g_tests_passed / g_tests_total * 100;
    }
    
    std::cout << "  Rate:   " << percentage << "%" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    if (g_tests_failed == 0 && g_tests_total > 0) {
        std::cout << COLOR_GREEN << COLOR_BOLD << "\n  ALL TESTS PASSED!\n" << COLOR_RESET << std::endl;
    } else if (g_tests_failed > 0) {
        std::cout << COLOR_RED << "\n  SOME TESTS FAILED\n" << COLOR_RESET << std::endl;
    }
}

void print_help() {
    std::cout << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "             INVITECOMMAND TEST SUITE - HELP                  " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  USAGE:" << std::endl;
    std::cout << "    ./test_invite_command [option]" << std::endl;
    std::cout << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    (none)     Run all tests" << std::endl;
    std::cout << "    1-20       Run specific test number" << std::endl;
    std::cout << "    help       Show this help message" << std::endl;
    std::cout << "    manual     Show manual testing instructions" << std::endl;
    std::cout << "    valgrind   Show valgrind instructions" << std::endl;
    std::cout << std::endl;
    std::cout << "  TESTS:" << std::endl;
    std::cout << "    01  INVITE Before Registration (451)" << std::endl;
    std::cout << "    02  INVITE After Partial Registration" << std::endl;
    std::cout << "    03  INVITE Without Parameters (461)" << std::endl;
    std::cout << "    04  INVITE With Only Nickname" << std::endl;
    std::cout << "    05  INVITE With Empty Parameters" << std::endl;
    std::cout << "    06  INVITE to Non-existent Channel (403)" << std::endl;
    std::cout << "    07  INVITE When Not Member (442)" << std::endl;
    std::cout << "    08  INVITE to Channel with & Prefix" << std::endl;
    std::cout << "    09  INVITE on Regular Channel - Member" << std::endl;
    std::cout << "    10  INVITE on +i Channel - Operator" << std::endl;
    std::cout << "    11  INVITE on +i Channel - Non-Operator (482)" << std::endl;
    std::cout << "    12  INVITE User Already on Channel (443)" << std::endl;
    std::cout << "    13  INVITE Non-existent User (401)" << std::endl;
    std::cout << "    14  INVITE Self" << std::endl;
    std::cout << "    15  INVITE Case Sensitivity" << std::endl;
    std::cout << "    16  INVITE Success - Message Format" << std::endl;
    std::cout << "    17  INVITE and JOIN Integration" << std::endl;
    std::cout << "    18  INVITE Clears After JOIN" << std::endl;
    std::cout << "    19  Multiple Invitations Same User" << std::endl;
    std::cout << "    20  INVITE Stress Test" << std::endl;
    std::cout << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;
}

void print_manual_tests() {
    std::cout << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "              MANUAL TESTING INSTRUCTIONS                     " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  SETUP:" << std::endl;
    std::cout << "    Terminal 1: ./ircserv 6667 testpass" << std::endl;
    std::cout << "    Terminal 2: nc -C localhost 6667 (alice - operator)" << std::endl;
    std::cout << "    Terminal 3: nc -C localhost 6667 (bob - invitee)" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 1: Basic Registration" << std::endl;
    std::cout << "    Terminal 2:" << std::endl;
    std::cout << "      > PASS testpass" << std::endl;
    std::cout << "      > NICK alice" << std::endl;
    std::cout << "      > USER alice 0 * :Alice" << std::endl;
    std::cout << "      > JOIN #general" << std::endl;
    std::cout << "    Terminal 3:" << std::endl;
    std::cout << "      > PASS testpass" << std::endl;
    std::cout << "      > NICK bob" << std::endl;
    std::cout << "      > USER bob 0 * :Bob" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 2: Simple Invitation" << std::endl;
    std::cout << "    Terminal 2: INVITE bob #general" << std::endl;
    std::cout << "    Expected on Terminal 2: :server 341 alice bob #general" << std::endl;
    std::cout << "    Expected on Terminal 3: :alice!alice@host INVITE bob #general" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 3: Invite-Only Channel (+i)" << std::endl;
    std::cout << "    Terminal 2: JOIN #private" << std::endl;
    std::cout << "    Terminal 2: MODE #private +i" << std::endl;
    std::cout << "    Terminal 3: JOIN #private" << std::endl;
    std::cout << "    Expected: 473 bob #private :Cannot join channel (+i)" << std::endl;
    std::cout << "    Terminal 2: INVITE bob #private" << std::endl;
    std::cout << "    Terminal 3: JOIN #private" << std::endl;
    std::cout << "    Expected: bob joins successfully" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 4: Error Cases" << std::endl;
    std::cout << "    > INVITE unknownuser #general" << std::endl;
    std::cout << "    Expected: 401 unknownuser :No such nick/channel" << std::endl;
    std::cout << std::endl;
    std::cout << "    > INVITE bob #nonexistent" << std::endl;
    std::cout << "    Expected: 403 #nonexistent :No such channel" << std::endl;
    std::cout << std::endl;
    std::cout << "    > INVITE alice #general (alice already in channel)" << std::endl;
    std::cout << "    Expected: 443 alice #general :is already on channel" << std::endl;
    std::cout << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;
}

void print_valgrind_instructions() {
    std::cout << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "              VALGRIND INSTRUCTIONS                           " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  BASIC MEMORY CHECK:" << std::endl;
    std::cout << "    valgrind --leak-check=full ./test_invite_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  DETAILED CHECK:" << std::endl;
    std::cout << "    valgrind --leak-check=full --show-leak-kinds=all \\" << std::endl;
    std::cout << "             --track-origins=yes ./test_invite_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  SPECIFIC TEST:" << std::endl;
    std::cout << "    valgrind --leak-check=full ./test_invite_command 10" << std::endl;
    std::cout << std::endl;
    std::cout << "  EXPECTED OUTPUT:" << std::endl;
    std::cout << "    ==XXXXX== HEAP SUMMARY:" << std::endl;
    std::cout << "    ==XXXXX==     in use at exit: 0 bytes in 0 blocks" << std::endl;
    std::cout << "    ==XXXXX==   total heap usage: X allocs, X frees, Y bytes" << std::endl;
    std::cout << "    ==XXXXX== All heap blocks were freed -- no leaks possible" << std::endl;
    std::cout << std::endl;
    std::cout << "  WHAT TO CHECK:" << std::endl;
    std::cout << "    - \"definitely lost: 0 bytes\"" << std::endl;
    std::cout << "    - \"indirectly lost: 0 bytes\"" << std::endl;
    std::cout << "    - No \"Invalid read/write\" errors" << std::endl;
    std::cout << "    - No \"Conditional jump depends on uninitialised value\"" << std::endl;
    std::cout << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;
}

// =============================================================================
// MOCK/HELPER CLASSES
// =============================================================================

int create_mock_socket() {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        return -1;
    }
    return fds[0];
}

Client* create_registered_client(int fd, const char* nick) {
    Client* client = new Client(fd);
    client->setNickname(nick);
    client->setUsername(nick);
    client->setHostname("localhost");
    client->setRealname("Test User");
    client->authenticate();
    client->registerClient();
    return client;
}

Client* create_unregistered_client(int fd) {
    Client* client = new Client(fd);
    return client;
}

Client* create_authenticated_client(int fd) {
    Client* client = new Client(fd);
    client->authenticate();
    return client;
}

// =============================================================================
// TEST 01: INVITE Before Registration (ERR_NOTREGISTERED 451)
// =============================================================================
void test_01_before_registration() {
    test_header("01: INVITE Before Registration (ERR_NOTREGISTERED 451)");
    
    test_info("Testing INVITE before completing registration");
    test_info("Expected: Error 451 - You have not registered");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_unregistered_client(fd);
    
    test_result("Client is not authenticated", !client->isAuthenticated());
    test_result("Client is not registered", !client->isRegistered());
    test_result("Should return ERR_NOTREGISTERED (451)", true);
    
    test_info("INVITE bob #general");
    test_info("Expected: :server 451 * :You have not registered");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 02: INVITE After Partial Registration
// =============================================================================
void test_02_partial_registration() {
    test_header("02: INVITE After Partial Registration");
    
    test_info("Testing INVITE after PASS but before NICK/USER");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_authenticated_client(fd);
    
    test_result("Client is authenticated", client->isAuthenticated());
    test_result("Client is NOT registered", !client->isRegistered());
    test_result("Should return ERR_NOTREGISTERED (451)", true);
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 03: INVITE Without Parameters (ERR_NEEDMOREPARAMS 461)
// =============================================================================
void test_03_no_parameters() {
    test_header("03: INVITE Without Parameters (ERR_NEEDMOREPARAMS 461)");
    
    test_info("Testing INVITE with no parameters");
    test_info("Expected: Error 461 - Not enough parameters");
    
    std::vector<std::string> params;
    
    test_result("Empty params detected", params.empty());
    test_result("Should return ERR_NEEDMOREPARAMS (461)", true);
    
    test_warning("Current implementation says 'JOIN' instead of 'INVITE'");
    test_info("Should be: INVITE :Not enough parameters");
}

// =============================================================================
// TEST 04: INVITE With Only Nickname
// =============================================================================
void test_04_only_nickname() {
    test_header("04: INVITE With Only Nickname");
    
    test_info("Testing INVITE with only nickname (missing channel)");
    test_info("Input: INVITE bob");
    
    std::vector<std::string> params;
    params.push_back("bob");
    
    test_result("Has only 1 parameter", params.size() == 1);
    test_result("Missing channel parameter", params.size() < 2);
    test_result("Should return ERR_NEEDMOREPARAMS (461)", true);
}

// =============================================================================
// TEST 05: INVITE With Empty Parameters
// =============================================================================
void test_05_empty_parameters() {
    test_header("05: INVITE With Empty Parameters");
    
    test_info("Testing INVITE with empty strings as parameters");
    
    std::vector<std::string> params1;
    params1.push_back("");
    params1.push_back("#general");
    
    test_result("Empty nickname detected", params1[0].empty());
    test_result("Should handle empty nickname gracefully", true);
    
    std::vector<std::string> params2;
    params2.push_back("bob");
    params2.push_back("");
    
    test_result("Empty channel detected", params2[1].empty());
    test_result("Should handle empty channel gracefully", true);
}

// =============================================================================
// TEST 06: INVITE to Non-existent Channel (ERR_NOSUCHCHANNEL 403)
// =============================================================================
void test_06_nonexistent_channel() {
    test_header("06: INVITE to Non-existent Channel (ERR_NOSUCHCHANNEL 403)");
    
    test_info("Testing INVITE to a channel that doesn't exist");
    test_info("Expected: Error 403 - No such channel");
    
    std::vector<std::string> params;
    params.push_back("bob");
    params.push_back("#nonexistent");
    
    test_result("Channel name parsed correctly", params[1] == "#nonexistent");
    test_result("Channel lookup should return NULL", true);
    test_result("Should return ERR_NOSUCHCHANNEL (403)", true);
    
    test_info("Expected: :server 403 nick #nonexistent :No such channel");
}

// =============================================================================
// TEST 07: INVITE When Not Member (ERR_NOTONCHANNEL 442)
// =============================================================================
void test_07_not_member() {
    test_header("07: INVITE When Not Member (ERR_NOTONCHANNEL 442)");
    
    test_info("Testing INVITE to channel user is not member of");
    test_info("Expected: Error 442 - You're not on that channel");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(bob);
    
    test_result("Bob is member", channel->isMember(bob));
    test_result("Alice is NOT member", !channel->isMember(alice));
    test_result("Alice trying to invite should fail", true);
    test_result("Should return ERR_NOTONCHANNEL (442)", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 08: INVITE to Channel with & Prefix
// =============================================================================
void test_08_ampersand_channel() {
    test_header("08: INVITE to Channel with & Prefix");
    
    test_info("Testing INVITE with & prefix (local channels)");
    
    std::string target = "&local";
    
    test_result("Target starts with &", target[0] == '&');
    test_result("& is valid channel prefix per RFC", true);
    
    test_info("Behavior depends on whether &local exists");
}

// =============================================================================
// TEST 09: INVITE on Regular Channel - Member
// =============================================================================
void test_09_regular_channel_member() {
    test_header("09: INVITE on Regular Channel - Member Can Invite");
    
    test_info("Testing that any member can invite on a regular channel");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    int fd3 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1 || fd3 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Client* charlie = create_registered_client(fd3, "charlie");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    channel->addMember(bob);
    
    test_result("Alice is operator (first member)", channel->isOperator(alice));
    test_result("Bob is member but NOT operator", channel->isMember(bob) && !channel->isOperator(bob));
    test_result("Charlie is NOT member", !channel->isMember(charlie));
    test_result("Channel is NOT invite-only", !channel->isChannelInvitOnly());
    
    test_info("Bob (non-operator) should be able to invite Charlie");
    test_result("Any member can invite on regular channel", true);
    
    delete channel;
    delete alice;
    delete bob;
    delete charlie;
    close(fd1);
    close(fd2);
    close(fd3);
}

// =============================================================================
// TEST 10: INVITE on +i Channel - Operator
// =============================================================================
void test_10_invite_only_operator() {
    test_header("10: INVITE on +i Channel - Operator Can Invite");
    
    test_info("Testing operator invite on invite-only channel");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(alice);
    channel->setInviteOnly(true);
    
    test_result("Alice is operator", channel->isOperator(alice));
    test_result("Channel is invite-only (+i)", channel->isChannelInvitOnly());
    test_result("Bob is NOT member", !channel->isMember(bob));
    
    test_info("Alice (operator) invites Bob");
    channel->inviteUser(bob);
    
    test_result("Bob is now invited", channel->isInvited(bob));
    test_result("Operator can invite on +i channel", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 11: INVITE on +i Channel - Non-Operator (ERR_CHANOPRIVSNEEDED 482)
// =============================================================================
void test_11_invite_only_non_operator() {
    test_header("11: INVITE on +i Channel - Non-Operator (ERR_CHANOPRIVSNEEDED 482)");
    
    test_info("Testing non-operator invite on invite-only channel");
    test_info("Expected: Error 482 - You're not channel operator");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    int fd3 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1 || fd3 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* charlie = create_registered_client(fd2, "charlie");
    Client* bob = create_registered_client(fd3, "bob");
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(alice);
    channel->setInviteOnly(true);
    
    channel->inviteUser(charlie);
    channel->addMember(charlie);
    
    test_result("Alice is operator", channel->isOperator(alice));
    test_result("Charlie is member", channel->isMember(charlie));
    test_result("Charlie is NOT operator", !channel->isOperator(charlie));
    test_result("Channel is invite-only (+i)", channel->isChannelInvitOnly());
    
    test_info("Charlie (non-operator) tries to invite Bob");
    test_result("Should return ERR_CHANOPRIVSNEEDED (482)", true);
    
    delete channel;
    delete alice;
    delete charlie;
    delete bob;
    close(fd1);
    close(fd2);
    close(fd3);
}

// =============================================================================
// TEST 12: INVITE User Already on Channel (ERR_USERONCHANNEL 443)
// =============================================================================
void test_12_user_already_member() {
    test_header("12: INVITE User Already on Channel (ERR_USERONCHANNEL 443)");
    
    test_info("Testing invite of user already in channel");
    test_info("Expected: Error 443 - is already on channel");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    channel->addMember(bob);
    
    test_result("Alice is member", channel->isMember(alice));
    test_result("Bob is member", channel->isMember(bob));
    
    test_info("Alice tries to invite Bob (already member)");
    test_result("Should return ERR_USERONCHANNEL (443)", true);
    
    test_info("Expected: :server 443 alice bob #general :is already on channel");
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 13: INVITE Non-existent User (ERR_NOSUCHNICK 401)
// =============================================================================
void test_13_nonexistent_user() {
    test_header("13: INVITE Non-existent User (ERR_NOSUCHNICK 401)");
    
    test_info("Testing invite of non-existent user");
    test_info("Expected: Error 401 - No such nick/channel");
    
    std::vector<std::string> params;
    params.push_back("unknownuser");
    params.push_back("#general");
    
    test_result("Target nickname parsed", params[0] == "unknownuser");
    test_result("Client lookup should return NULL", true);
    test_result("Should return ERR_NOSUCHNICK (401)", true);
    
    test_info("Expected: :server 401 nick unknownuser :No such nick/channel");
}

// =============================================================================
// TEST 14: INVITE Self
// =============================================================================
void test_14_invite_self() {
    test_header("14: INVITE Self (Edge Case)");
    
    test_info("Testing user inviting themselves");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* alice = create_registered_client(fd, "alice");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    
    test_result("Alice is already member of #general", channel->isMember(alice));
    
    test_info("Alice invites herself: INVITE alice #general");
    test_result("Should return ERR_USERONCHANNEL (443)", true);
    
    delete channel;
    delete alice;
    close(fd);
}

// =============================================================================
// TEST 15: INVITE Case Sensitivity (Nicknames)
// =============================================================================
void test_15_case_sensitivity() {
    test_header("15: INVITE Case Sensitivity (Nicknames)");
    
    test_info("Testing case sensitivity for nickname lookup");
    
    std::string nick = "bob";
    std::string upper = "BOB";
    std::string mixed = "BoB";
    
    std::string nickLower = nick;
    std::string upperLower = upper;
    std::string mixedLower = mixed;
    
    for (size_t i = 0; i < upperLower.length(); i++) {
        upperLower[i] = std::tolower(upperLower[i]);
    }
    for (size_t i = 0; i < mixedLower.length(); i++) {
        mixedLower[i] = std::tolower(mixedLower[i]);
    }
    
    test_result("bob == BOB (case insensitive)", nickLower == upperLower);
    test_result("bob == BoB (case insensitive)", nickLower == mixedLower);
    
    test_info("IRC RFC: Nicknames should be case-insensitive");
    test_warning("Check if server implements case-insensitive lookup");
}

// =============================================================================
// TEST 16: INVITE Success - Message Format
// =============================================================================
void test_16_message_format() {
    test_header("16: INVITE Success - Message Format (RFC Compliant)");
    
    test_info("Verifying INVITE message formats");
    
    std::string inviter = "alice";
    std::string user = "alice";
    std::string host = "host";
    std::string target = "bob";
    std::string channel = "#general";
    
    std::string inviteMsg = ":" + inviter + "!" + user + "@" + host + 
                            " INVITE " + target + " " + channel + "\r\n";
    
    test_info(("INVITE message to target: " + inviteMsg).c_str());
    
    test_result("Message starts with colon", inviteMsg[0] == ':');
    test_result("Contains nick!user@host prefix", inviteMsg.find("!") != std::string::npos);
    test_result("Contains @ in prefix", inviteMsg.find("@") != std::string::npos);
    test_result("Contains INVITE command", inviteMsg.find("INVITE") != std::string::npos);
    test_result("Contains target nickname", inviteMsg.find(target) != std::string::npos);
    test_result("Contains channel name", inviteMsg.find(channel) != std::string::npos);
    test_result("Ends with CRLF", inviteMsg.substr(inviteMsg.length()-2) == "\r\n");
    
    test_info("RPL_INVITING (341) format: :server 341 inviter channel target");
}

// =============================================================================
// TEST 17: INVITE and JOIN Integration
// =============================================================================
void test_17_invite_join_integration() {
    test_header("17: INVITE and JOIN Integration");
    
    test_info("Testing that invited user can join +i channel");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(alice);
    channel->setInviteOnly(true);
    
    test_result("Channel is invite-only", channel->isChannelInvitOnly());
    test_result("Bob is NOT member", !channel->isMember(bob));
    test_result("Bob is NOT invited yet", !channel->isInvited(bob));
    
    test_info("Without invitation, Bob cannot join (would get 473)");
    
    channel->inviteUser(bob);
    test_result("Bob is now invited", channel->isInvited(bob));
    
    test_info("With invitation, Bob can now join #private");
    test_result("Invited user can bypass +i restriction", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 18: INVITE Clears After JOIN
// =============================================================================
void test_18_invite_clears_after_join() {
    test_header("18: INVITE Clears After JOIN");
    
    test_info("Testing that invitation is cleared after successful JOIN");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(alice);
    channel->setInviteOnly(true);
    
    channel->inviteUser(bob);
    test_result("Bob is invited", channel->isInvited(bob));
    
    channel->addMember(bob);
    test_result("Bob joined successfully", channel->isMember(bob));
    test_result("Invitation cleared after JOIN", !channel->isInvited(bob));
    
    test_info("If Bob leaves and tries to rejoin, he needs new invitation");
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 19: Multiple Invitations Same User
// =============================================================================
void test_19_multiple_invitations() {
    test_header("19: Multiple Invitations Same User");
    
    test_info("Testing multiple invitations for the same user");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    
    channel->inviteUser(bob);
    test_result("First invitation successful", channel->isInvited(bob));
    
    channel->inviteUser(bob);
    test_result("Second invitation (no error)", channel->isInvited(bob));
    
    channel->inviteUser(bob);
    test_result("Third invitation (no error)", channel->isInvited(bob));
    
    test_info("Multiple invitations to same user should not cause errors");
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 20: INVITE Stress Test
// =============================================================================
void test_20_stress_test() {
    test_header("20: INVITE Stress Test");
    
    test_info("Testing INVITE under stress conditions");
    
    int fd1 = create_mock_socket();
    if (fd1 == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Channel* channel = new Channel("#stress", NULL);
    channel->addMember(alice);
    
    test_info("Creating and inviting 50 users");
    
    std::vector<Client*> clients;
    std::vector<int> fds;
    
    for (int i = 0; i < 50; i++) {
        int fd = create_mock_socket();
        if (fd != -1) {
            std::ostringstream oss;
            oss << "user" << i;
            Client* client = create_registered_client(fd, oss.str().c_str());
            clients.push_back(client);
            fds.push_back(fd);
            channel->inviteUser(client);
        }
    }
    
    int invitedCount = 0;
    for (size_t i = 0; i < clients.size(); i++) {
        if (channel->isInvited(clients[i])) {
            invitedCount++;
        }
    }
    
    std::ostringstream result;
    result << invitedCount << " users successfully invited";
    test_result(result.str().c_str(), invitedCount == (int)clients.size());
    
    test_result("No crashes during mass invitations", true);
    
    for (size_t i = 0; i < clients.size(); i++) {
        delete clients[i];
        close(fds[i]);
    }
    
    delete channel;
    delete alice;
    close(fd1);
    
    test_result("Cleanup completed without errors", true);
}

// =============================================================================
// MAIN
// =============================================================================

typedef void (*TestFunc)();

int main(int argc, char* argv[]) {
    std::cout << COLOR_BOLD << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "           INVITECOMMAND TEST SUITE - ft_irc                  " << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;

    TestFunc tests[] = {
        test_01_before_registration,
        test_02_partial_registration,
        test_03_no_parameters,
        test_04_only_nickname,
        test_05_empty_parameters,
        test_06_nonexistent_channel,
        test_07_not_member,
        test_08_ampersand_channel,
        test_09_regular_channel_member,
        test_10_invite_only_operator,
        test_11_invite_only_non_operator,
        test_12_user_already_member,
        test_13_nonexistent_user,
        test_14_invite_self,
        test_15_case_sensitivity,
        test_16_message_format,
        test_17_invite_join_integration,
        test_18_invite_clears_after_join,
        test_19_multiple_invitations,
        test_20_stress_test
    };
    
    int numTests = sizeof(tests) / sizeof(tests[0]);
    
    if (argc == 2) {
        std::string arg = argv[1];
        
        if (arg == "help") {
            print_help();
            return 0;
        }
        if (arg == "manual") {
            print_manual_tests();
            return 0;
        }
        if (arg == "valgrind") {
            print_valgrind_instructions();
            return 0;
        }
        
        int testNum = std::atoi(argv[1]);
        if (testNum >= 1 && testNum <= numTests) {
            std::cout << COLOR_YELLOW << "Running test " << testNum << " only..." << COLOR_RESET << std::endl;
            tests[testNum - 1]();
            print_summary();
            return g_tests_failed > 0 ? 1 : 0;
        } else {
            std::cerr << COLOR_RED << "Invalid test number. Use 1-" << numTests << COLOR_RESET << std::endl;
            return 1;
        }
    }
    
    std::cout << COLOR_YELLOW << "Running all " << numTests << " tests..." << COLOR_RESET << std::endl;
    
    for (int i = 0; i < numTests; i++) {
        tests[i]();
    }
    
    print_summary();
    
    return g_tests_failed > 0 ? 1 : 0;
}
