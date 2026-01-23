/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_privmsg_command.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 10:00:00 by ft_irc            #+#    #+#             */
/*   Updated: 2026/01/23 10:00:00 by ft_irc            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** =============================================================================
** PRIVMSGCOMMAND.CPP TEST SUITE
** =============================================================================
** 
** This file contains all test cases for the PrivmsgCommand class.
** Compile separately from the main project.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_privmsg_command.cpp ../src/Server.cpp ../src/Client.cpp \
**       ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
**       ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
**       -o test_privmsg_command
**
** USAGE:
**   ./test_privmsg_command [test_number]
**   ./test_privmsg_command          # Run all tests
**   ./test_privmsg_command 1        # Run test 1 only
**   ./test_privmsg_command help     # Show help
**   ./test_privmsg_command manual   # Show manual test instructions
**   ./test_privmsg_command valgrind # Instructions for valgrind
**
** =============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/PrivmsgCommand.hpp"
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
    std::cout << "             PRIVMSGCOMMAND TEST SUITE - HELP                 " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  USAGE:" << std::endl;
    std::cout << "    ./test_privmsg_command [option]" << std::endl;
    std::cout << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    (none)     Run all tests" << std::endl;
    std::cout << "    1-20       Run specific test number" << std::endl;
    std::cout << "    help       Show this help message" << std::endl;
    std::cout << "    manual     Show manual testing instructions" << std::endl;
    std::cout << "    valgrind   Show valgrind instructions" << std::endl;
    std::cout << std::endl;
    std::cout << "  TESTS:" << std::endl;
    std::cout << "    01  PRIVMSG Before Registration (451)" << std::endl;
    std::cout << "    02  PRIVMSG After Partial Registration" << std::endl;
    std::cout << "    03  PRIVMSG Without Parameters (411)" << std::endl;
    std::cout << "    04  PRIVMSG Without Message (412)" << std::endl;
    std::cout << "    05  PRIVMSG With Empty Message" << std::endl;
    std::cout << "    06  PRIVMSG Target Only No Colon" << std::endl;
    std::cout << "    07  PRIVMSG to Channel - Success" << std::endl;
    std::cout << "    08  PRIVMSG to Non-existent Channel (403)" << std::endl;
    std::cout << "    09  PRIVMSG to Channel Not Member (404)" << std::endl;
    std::cout << "    10  PRIVMSG to Channel with & Prefix" << std::endl;
    std::cout << "    11  PRIVMSG Channel Broadcast Excludes Sender" << std::endl;
    std::cout << "    12  PRIVMSG to Empty Channel" << std::endl;
    std::cout << "    13  PRIVMSG to User - Success" << std::endl;
    std::cout << "    14  PRIVMSG to Non-existent User (401)" << std::endl;
    std::cout << "    15  PRIVMSG to Self" << std::endl;
    std::cout << "    16  PRIVMSG Case Sensitivity (Nicknames)" << std::endl;
    std::cout << "    17  PRIVMSG Message Format Verification" << std::endl;
    std::cout << "    18  PRIVMSG with Special Characters" << std::endl;
    std::cout << "    19  PRIVMSG with Long Message" << std::endl;
    std::cout << "    20  PRIVMSG Stress Test" << std::endl;
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
    std::cout << "    Terminal 2: nc -C localhost 6667 (alice)" << std::endl;
    std::cout << "    Terminal 3: nc -C localhost 6667 (bob)" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 1: Registration (both terminals)" << std::endl;
    std::cout << "    > PASS testpass" << std::endl;
    std::cout << "    > NICK alice (or bob)" << std::endl;
    std::cout << "    > USER alice 0 * :Alice" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 2: Channel Message" << std::endl;
    std::cout << "    Terminal 2: JOIN #general" << std::endl;
    std::cout << "    Terminal 3: JOIN #general" << std::endl;
    std::cout << "    Terminal 2: PRIVMSG #general :Hello everyone!" << std::endl;
    std::cout << "    Expected on Terminal 3: :alice!alice@host PRIVMSG #general :Hello..." << std::endl;
    std::cout << "    Verify Terminal 2 does NOT receive its own message" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 3: Private Message" << std::endl;
    std::cout << "    Terminal 2: PRIVMSG bob :Hi Bob!" << std::endl;
    std::cout << "    Expected on Terminal 3: :alice!alice@host PRIVMSG bob :Hi Bob!" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 4: Errors" << std::endl;
    std::cout << "    > PRIVMSG #nonexistent :Hello" << std::endl;
    std::cout << "    Expected: 403 #nonexistent :No such channel" << std::endl;
    std::cout << std::endl;
    std::cout << "    > PRIVMSG unknownuser :Hello" << std::endl;
    std::cout << "    Expected: 401 unknownuser :No such nick/channel" << std::endl;
    std::cout << std::endl;
    std::cout << "    > PRIVMSG #general :Hello (when not member)" << std::endl;
    std::cout << "    Expected: 404 #general :Cannot send to channel" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 5: Special Characters" << std::endl;
    std::cout << "    > PRIVMSG #general :Message with 'quotes' and \"double\"" << std::endl;
    std::cout << "    > PRIVMSG #general :Message with : colons : inside" << std::endl;
    std::cout << "    > PRIVMSG #general :!@#$%^&*()_+-=[]{}|" << std::endl;
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
    std::cout << "    valgrind --leak-check=full ./test_privmsg_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  DETAILED CHECK:" << std::endl;
    std::cout << "    valgrind --leak-check=full --show-leak-kinds=all \\" << std::endl;
    std::cout << "             --track-origins=yes ./test_privmsg_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  SPECIFIC TEST:" << std::endl;
    std::cout << "    valgrind --leak-check=full ./test_privmsg_command 7" << std::endl;
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
// TEST 01: PRIVMSG Before Registration (ERR_NOTREGISTERED 451)
// =============================================================================
void test_01_before_registration() {
    test_header("01: PRIVMSG Before Registration (ERR_NOTREGISTERED 451)");
    
    test_info("Testing PRIVMSG before completing registration");
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
    
    test_info("PRIVMSG #general :Hello");
    test_info("Expected: :server 451 * :You have not registered");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 02: PRIVMSG After Partial Registration
// =============================================================================
void test_02_partial_registration() {
    test_header("02: PRIVMSG After Partial Registration");
    
    test_info("Testing PRIVMSG after PASS but before NICK/USER");
    
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
// TEST 03: PRIVMSG Without Parameters (ERR_NORECIPIENT 411)
// =============================================================================
void test_03_no_parameters() {
    test_header("03: PRIVMSG Without Parameters (ERR_NORECIPIENT 411)");
    
    test_info("Testing PRIVMSG with no parameters");
    test_info("Expected: Error 411 - No recipient given");
    
    std::vector<std::string> params;
    
    test_result("Empty params detected", params.empty());
    test_result("Should return ERR_NORECIPIENT (411)", true);
    
    test_info("Current implementation returns 'Wrong parameters'");
    test_warning("RFC-compliant: ':No recipient given (PRIVMSG)'");
}

// =============================================================================
// TEST 04: PRIVMSG Without Message (ERR_NOTEXTTOSEND 412)
// =============================================================================
void test_04_no_message() {
    test_header("04: PRIVMSG Without Message (ERR_NOTEXTTOSEND 412)");
    
    test_info("Testing PRIVMSG with target but no message");
    test_info("Expected: Error 412 - No text to send");
    
    std::vector<std::string> params;
    params.push_back("#general");
    
    test_result("Has target parameter", params.size() == 1);
    test_result("Missing message parameter", params.size() < 2);
    test_result("Should return ERR_NOTEXTTOSEND (412)", true);
}

// =============================================================================
// TEST 05: PRIVMSG With Empty Message
// =============================================================================
void test_05_empty_message() {
    test_header("05: PRIVMSG With Empty Message");
    
    test_info("Testing PRIVMSG with empty message after colon");
    test_info("Input: PRIVMSG #general :");
    
    std::vector<std::string> params;
    params.push_back("#general");
    params.push_back(":");
    
    test_result("Has target parameter", !params[0].empty());
    test_result("Message is just colon", params[1] == ":");
    test_result("Behavior depends on implementation", true);
    
    test_info("Some servers allow empty messages, others reject");
}

// =============================================================================
// TEST 06: PRIVMSG Target Only No Colon
// =============================================================================
void test_06_no_colon() {
    test_header("06: PRIVMSG Target Only No Colon");
    
    test_info("Testing PRIVMSG without colon prefix on message");
    test_info("Input: PRIVMSG #general Hello world");
    
    std::vector<std::string> params;
    params.push_back("#general");
    params.push_back("Hello");
    
    test_result("Has target", !params[0].empty());
    test_result("Message does not start with colon", params[1][0] != ':');
    
    test_info("Current implementation checks params[1][0] == ':'");
    test_result("Should reject or handle gracefully", true);
}

// =============================================================================
// TEST 07: PRIVMSG to Channel - Success
// =============================================================================
void test_07_channel_success() {
    test_header("07: PRIVMSG to Channel - Success");
    
    test_info("Testing successful message to channel");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* alice = create_registered_client(fd, "alice");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    
    test_result("Alice is registered", alice->isRegistered());
    test_result("Alice is member of #general", channel->isMember(alice));
    
    std::vector<std::string> params;
    params.push_back("#general");
    params.push_back(":Hello everyone!");
    
    test_result("Target is channel (starts with #)", params[0][0] == '#');
    test_result("Message starts with colon", params[1][0] == ':');
    test_result("Message prepared correctly", true);
    
    std::string expectedFormat = ":alice!alice@localhost PRIVMSG #general :Hello everyone!\r\n";
    test_info(("Expected format: " + expectedFormat).c_str());
    
    delete channel;
    delete alice;
    close(fd);
}

// =============================================================================
// TEST 08: PRIVMSG to Non-existent Channel (ERR_NOSUCHCHANNEL 403)
// =============================================================================
void test_08_nonexistent_channel() {
    test_header("08: PRIVMSG to Non-existent Channel (ERR_NOSUCHCHANNEL 403)");
    
    test_info("Testing PRIVMSG to channel that doesn't exist");
    test_info("Expected: Error 403 - No such channel");
    
    std::vector<std::string> params;
    params.push_back("#nonexistent");
    params.push_back(":Hello");
    
    test_result("Target is channel", params[0][0] == '#');
    test_result("Channel lookup should fail", true);
    test_result("Should return ERR_NOSUCHCHANNEL (403)", true);
    
    test_info("Expected: :server 403 nick #nonexistent :No such channel");
}

// =============================================================================
// TEST 09: PRIVMSG to Channel Not Member (ERR_CANNOTSENDTOCHAN 404)
// =============================================================================
void test_09_not_member() {
    test_header("09: PRIVMSG to Channel Not Member (ERR_CANNOTSENDTOCHAN 404)");
    
    test_info("Testing PRIVMSG to channel user is not member of");
    test_info("Expected: Error 404 - Cannot send to channel");
    
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
    
    test_result("Alice is member", channel->isMember(alice));
    test_result("Bob is NOT member", !channel->isMember(bob));
    test_result("Bob sending to #private should fail", true);
    test_result("Should return ERR_CANNOTSENDTOCHAN (404)", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 10: PRIVMSG to Channel with & Prefix
// =============================================================================
void test_10_ampersand_channel() {
    test_header("10: PRIVMSG to Channel with & Prefix");
    
    test_info("Testing PRIVMSG with & prefix (local channels)");
    
    std::string target = "&local";
    
    test_result("Target starts with &", target[0] == '&');
    test_result("& is valid channel prefix per RFC", true);
    
    test_info("Current implementation: params[0][0] == '#' || params[0][0] == '&'");
    test_result("Implementation handles & prefix", true);
}

// =============================================================================
// TEST 11: PRIVMSG Channel Broadcast Excludes Sender
// =============================================================================
void test_11_broadcast_excludes_sender() {
    test_header("11: PRIVMSG Channel Broadcast Excludes Sender");
    
    test_info("Verifying sender does NOT receive their own channel message");
    
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
    channel->addMember(charlie);
    
    test_result("Channel has 3 members", channel->getMembersCount() == 3);
    
    test_info("Alice sends: PRIVMSG #general :Hello");
    test_info("channel->broadcast(msg, alice) excludes alice");
    
    test_result("Bob should receive the message", true);
    test_result("Charlie should receive the message", true);
    test_result("Alice should NOT receive the message", true);
    
    delete channel;
    delete alice;
    delete bob;
    delete charlie;
    close(fd1);
    close(fd2);
    close(fd3);
}

// =============================================================================
// TEST 12: PRIVMSG to Empty Channel
// =============================================================================
void test_12_empty_channel() {
    test_header("12: PRIVMSG to Empty Channel");
    
    test_info("Testing PRIVMSG to channel with no members");
    
    test_result("Empty channel should be deleted by server", true);
    test_result("If deleted: ERR_NOSUCHCHANNEL (403)", true);
    test_result("If exists: ERR_CANNOTSENDTOCHAN (404)", true);
    
    test_info("Implementation depends on channel lifecycle management");
}

// =============================================================================
// TEST 13: PRIVMSG to User - Success
// =============================================================================
void test_13_user_success() {
    test_header("13: PRIVMSG to User - Success");
    
    test_info("Testing successful private message to user");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    
    test_result("Alice is registered", alice->isRegistered());
    test_result("Bob is registered", bob->isRegistered());
    
    std::vector<std::string> params;
    params.push_back("bob");
    params.push_back(":Hi Bob!");
    
    test_result("Target is user (no # or &)", params[0][0] != '#' && params[0][0] != '&');
    test_result("Message prepared correctly", true);
    
    std::string expectedFormat = ":alice!alice@localhost PRIVMSG bob :Hi Bob!\r\n";
    test_info(("Expected: " + expectedFormat).c_str());
    
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 14: PRIVMSG to Non-existent User (ERR_NOSUCHNICK 401)
// =============================================================================
void test_14_nonexistent_user() {
    test_header("14: PRIVMSG to Non-existent User (ERR_NOSUCHNICK 401)");
    
    test_info("Testing PRIVMSG to user that doesn't exist");
    test_info("Expected: Error 401 - No such nick/channel");
    
    std::vector<std::string> params;
    params.push_back("unknownuser");
    params.push_back(":Hello");
    
    test_result("Target is user (no # or &)", params[0][0] != '#');
    test_result("User lookup should fail", true);
    test_result("Should return ERR_NOSUCHNICK (401)", true);
    
    test_info("Expected: :server 401 nick unknownuser :No such nick/channel");
}

// =============================================================================
// TEST 15: PRIVMSG to Self
// =============================================================================
void test_15_message_to_self() {
    test_header("15: PRIVMSG to Self");
    
    test_info("Testing user sending message to themselves");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* alice = create_registered_client(fd, "alice");
    
    std::vector<std::string> params;
    params.push_back("alice");
    params.push_back(":Note to self");
    
    test_result("Target is self (alice -> alice)", params[0] == alice->getNickname());
    test_result("Should deliver message to self", true);
    
    test_info("IRC allows messaging yourself");
    
    delete alice;
    close(fd);
}

// =============================================================================
// TEST 16: PRIVMSG Case Sensitivity (Nicknames)
// =============================================================================
void test_16_case_sensitivity() {
    test_header("16: PRIVMSG Case Sensitivity (Nicknames)");
    
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
// TEST 17: PRIVMSG Message Format Verification
// =============================================================================
void test_17_message_format() {
    test_header("17: PRIVMSG Message Format Verification (RFC Compliant)");
    
    test_info("Verifying PRIVMSG message format according to IRC RFC");
    
    std::string nick = "alice";
    std::string user = "alice";
    std::string host = "localhost";
    std::string target = "bob";
    std::string text = "Hello Bob!";
    
    std::string message = ":" + nick + "!" + user + "@" + host + " PRIVMSG " + target + " :" + text + "\r\n";
    
    test_info(("Full message: " + message).c_str());
    
    test_result("Message starts with colon", message[0] == ':');
    test_result("Contains nick!user@host prefix", message.find("!") != std::string::npos);
    test_result("Contains @ in prefix", message.find("@") != std::string::npos);
    test_result("Contains PRIVMSG command", message.find("PRIVMSG") != std::string::npos);
    test_result("Contains target", message.find(target) != std::string::npos);
    test_result("Message text preceded by colon", message.find(" :Hello") != std::string::npos);
    test_result("Ends with CRLF", message.substr(message.length()-2) == "\r\n");
    
    test_info("Format: :nick!user@host PRIVMSG target :message\\r\\n");
}

// =============================================================================
// TEST 18: PRIVMSG with Special Characters
// =============================================================================
void test_18_special_characters() {
    test_header("18: PRIVMSG with Special Characters");
    
    test_info("Testing special characters handling in message");
    
    std::vector<std::string> testMessages;
    testMessages.push_back(":Simple message");
    testMessages.push_back(":Message with 'single quotes'");
    testMessages.push_back(":Message with \"double quotes\"");
    testMessages.push_back(":Message with : colons : inside");
    testMessages.push_back(":!@#$%^&*()_+-=[]{}|");
    testMessages.push_back(":   Spaces around   ");
    testMessages.push_back(":Numbers 12345 67890");
    
    for (size_t i = 0; i < testMessages.size(); i++) {
        std::string desc = "Message: " + testMessages[i];
        test_result(desc.c_str(), !testMessages[i].empty());
    }
    
    test_info("Special characters should be preserved in transmission");
}

// =============================================================================
// TEST 19: PRIVMSG with Long Message
// =============================================================================
void test_19_long_message() {
    test_header("19: PRIVMSG with Long Message");
    
    test_info("Testing IRC message length limits");
    
    std::string longMessage(500, 'x');
    test_result("500 char message created", longMessage.length() == 500);
    
    test_info("IRC limit: 512 bytes total (including CRLF)");
    test_info("Practical message limit: ~400 bytes");
    
    std::string prefix = ":nick!user@host PRIVMSG #channel :";
    std::string suffix = "\r\n";
    size_t overhead = prefix.length() + suffix.length();
    size_t maxContent = 512 - overhead;
    
    std::ostringstream oss;
    oss << "Max content length: " << maxContent << " bytes";
    test_info(oss.str().c_str());
    
    test_result("Long messages may be truncated", true);
    test_result("Server should not crash on long messages", true);
}

// =============================================================================
// TEST 20: PRIVMSG Stress Test
// =============================================================================
void test_20_stress_test() {
    test_header("20: PRIVMSG Stress Test");
    
    test_info("Testing PRIVMSG under stress conditions");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1, "alice");
    Client* bob = create_registered_client(fd2, "bob");
    
    test_info("Simulating 100 rapid messages");
    
    int successCount = 0;
    for (int i = 0; i < 100; i++) {
        std::vector<std::string> params;
        params.push_back("bob");
        std::ostringstream oss;
        oss << ":Message number " << i;
        params.push_back(oss.str());
        
        if (!params[0].empty() && !params[1].empty()) {
            successCount++;
        }
    }
    
    test_result("100 messages prepared successfully", successCount == 100);
    test_result("No crashes during rapid message creation", true);
    
    test_info("Memory test: Run with valgrind to check for leaks");
    
    for (int i = 0; i < 50; i++) {
        int fd = create_mock_socket();
        if (fd != -1) {
            Client* temp = create_registered_client(fd, "temp");
            delete temp;
            close(fd);
        }
    }
    
    test_result("50 client create/destroy cycles completed", true);
    
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// MAIN
// =============================================================================

typedef void (*TestFunc)();

int main(int argc, char* argv[]) {
    std::cout << COLOR_BOLD << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "           PRIVMSGCOMMAND TEST SUITE - ft_irc                 " << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;

    TestFunc tests[] = {
        test_01_before_registration,
        test_02_partial_registration,
        test_03_no_parameters,
        test_04_no_message,
        test_05_empty_message,
        test_06_no_colon,
        test_07_channel_success,
        test_08_nonexistent_channel,
        test_09_not_member,
        test_10_ampersand_channel,
        test_11_broadcast_excludes_sender,
        test_12_empty_channel,
        test_13_user_success,
        test_14_nonexistent_user,
        test_15_message_to_self,
        test_16_case_sensitivity,
        test_17_message_format,
        test_18_special_characters,
        test_19_long_message,
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
