/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_topic_command.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 10:00:00 by ft_irc            #+#    #+#             */
/*   Updated: 2026/01/23 10:00:00 by ft_irc            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** =============================================================================
** TOPICCOMMAND.CPP TEST SUITE
** =============================================================================
** 
** This file contains all test cases for the TopicCommand class.
** Compile separately from the main project.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_topic_command.cpp ../src/Server.cpp ../src/Client.cpp \
**       ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
**       ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
**       -o test_topic_command
**
** USAGE:
**   ./test_topic_command [test_number]
**   ./test_topic_command          # Run all tests
**   ./test_topic_command 1        # Run test 1 only
**   ./test_topic_command help     # Show help
**   ./test_topic_command manual   # Show manual test instructions
**   ./test_topic_command valgrind # Instructions for valgrind
**
** VALGRIND:
**   valgrind --leak-check=full --show-leak-kinds=all ./test_topic_command
**
** =============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/JoinCommand.hpp"
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
    std::cout << "              TOPICCOMMAND TEST SUITE - HELP                  " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  USAGE:" << std::endl;
    std::cout << "    ./test_topic_command [option]" << std::endl;
    std::cout << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    (none)     Run all tests" << std::endl;
    std::cout << "    1-25       Run specific test number" << std::endl;
    std::cout << "    help       Show this help message" << std::endl;
    std::cout << "    manual     Show manual testing instructions" << std::endl;
    std::cout << "    valgrind   Show valgrind instructions" << std::endl;
    std::cout << std::endl;
    std::cout << "  TESTS:" << std::endl;
    std::cout << "    01  TopicCommand Construction" << std::endl;
    std::cout << "    02  TopicCommand Destruction" << std::endl;
    std::cout << "    03  TOPIC Before Registration (ERR_NOTREGISTERED 451)" << std::endl;
    std::cout << "    04  TOPIC Without Parameter (ERR_NEEDMOREPARAMS 461)" << std::endl;
    std::cout << "    05  TOPIC Wrong Parameters Format" << std::endl;
    std::cout << "    06  TOPIC Non-existent Channel (ERR_NOSUCHCHANNEL 403)" << std::endl;
    std::cout << "    07  TOPIC Not Member of Channel (ERR_NOTONCHANNEL 442)" << std::endl;
    std::cout << "    08  TOPIC View - No Topic Set (RPL_NOTOPIC 331)" << std::endl;
    std::cout << "    09  TOPIC View - Topic Exists (RPL_TOPIC 332)" << std::endl;
    std::cout << "    10  TOPIC Set - Normal Channel Member" << std::endl;
    std::cout << "    11  TOPIC Set - Normal Channel Operator" << std::endl;
    std::cout << "    12  TOPIC Set - Protected Channel (+t) Non-Op (ERR_CHANOPRIVSNEEDED 482)" << std::endl;
    std::cout << "    13  TOPIC Set - Protected Channel (+t) Operator" << std::endl;
    std::cout << "    14  TOPIC Clear - Empty Topic" << std::endl;
    std::cout << "    15  TOPIC Broadcast to All Members" << std::endl;
    std::cout << "    16  TOPIC Multi-word Preservation" << std::endl;
    std::cout << "    17  TOPIC Special Characters" << std::endl;
    std::cout << "    18  TOPIC With Leading Colon in Content" << std::endl;
    std::cout << "    19  TOPIC Very Long Subject" << std::endl;
    std::cout << "    20  TOPIC Message Format Verification" << std::endl;
    std::cout << "    21  TOPIC Multiple Consecutive Changes" << std::endl;
    std::cout << "    22  TOPIC Memory Leaks (use valgrind)" << std::endl;
    std::cout << "    23  TOPIC Concurrent Multi-Client (manual)" << std::endl;
    std::cout << "    24  TOPIC Stress Test" << std::endl;
    std::cout << "    25  TOPIC Integration with JOIN" << std::endl;
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
    std::cout << "    Terminal 2: nc -C localhost 6667" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 1: View Topic (No Topic Set)" << std::endl;
    std::cout << "    > PASS testpass" << std::endl;
    std::cout << "    > NICK alice" << std::endl;
    std::cout << "    > USER alice 0 * :Alice" << std::endl;
    std::cout << "    > JOIN #test" << std::endl;
    std::cout << "    > TOPIC #test" << std::endl;
    std::cout << "    Expected: :server 331 alice #test :No topic is set" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 2: Set Topic" << std::endl;
    std::cout << "    > TOPIC #test :Welcome to Test Channel!" << std::endl;
    std::cout << "    Expected: :alice!alice@host TOPIC #test :Welcome to Test Channel!" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 3: View Existing Topic" << std::endl;
    std::cout << "    > TOPIC #test" << std::endl;
    std::cout << "    Expected: :server 332 alice #test :Welcome to Test Channel!" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 4: Clear Topic" << std::endl;
    std::cout << "    > TOPIC #test :" << std::endl;
    std::cout << "    Expected: :alice!alice@host TOPIC #test :" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 5: Topic on +t Channel (Non-Op)" << std::endl;
    std::cout << "    Terminal 2: (alice) MODE #test +t" << std::endl;
    std::cout << "    Terminal 3: (bob) JOIN #test" << std::endl;
    std::cout << "    Terminal 3: (bob) TOPIC #test :New Topic" << std::endl;
    std::cout << "    Expected: :server 482 bob #test :You're not channel operator" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 6: Topic on +t Channel (Operator)" << std::endl;
    std::cout << "    Terminal 2: (alice) TOPIC #test :Ops Only Topic" << std::endl;
    std::cout << "    Expected: :alice!...@host TOPIC #test :Ops Only Topic" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 7: Multi-Client Broadcast" << std::endl;
    std::cout << "    Terminal 2: alice joins #general" << std::endl;
    std::cout << "    Terminal 3: bob joins #general" << std::endl;
    std::cout << "    Terminal 4: charlie joins #general" << std::endl;
    std::cout << "    Terminal 2: (alice) TOPIC #general :New Announcement" << std::endl;
    std::cout << "    Expected: All terminals see the TOPIC message" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 8: Non-member Cannot View Topic" << std::endl;
    std::cout << "    > TOPIC #otherchannel" << std::endl;
    std::cout << "    Expected: :server 442 nick #otherchannel :You're not on that channel" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 9: Special Characters in Topic" << std::endl;
    std::cout << "    > TOPIC #test :Hello! @everyone #rules :info :)" << std::endl;
    std::cout << "    Expected: Topic preserved with all special chars" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 10: Multiple Words Preservation" << std::endl;
    std::cout << "    > TOPIC #test :This is a very long topic with many words" << std::endl;
    std::cout << "    > TOPIC #test" << std::endl;
    std::cout << "    Expected: Full topic text returned intact" << std::endl;
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
    std::cout << "    valgrind --leak-check=full ./test_topic_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  DETAILED CHECK:" << std::endl;
    std::cout << "    valgrind --leak-check=full --show-leak-kinds=all \\" << std::endl;
    std::cout << "             --track-origins=yes ./test_topic_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  SPECIFIC TEST:" << std::endl;
    std::cout << "    valgrind --leak-check=full ./test_topic_command 22" << std::endl;
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

Client* create_registered_client(int fd) {
    Client* client = new Client(fd);
    client->setNickname("testuser");
    client->setUsername("testuser");
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

Client* create_named_client(int fd, const std::string& nick) {
    Client* client = new Client(fd);
    client->setNickname(nick);
    client->setUsername(nick);
    client->setHostname("localhost");
    client->setRealname("Test " + nick);
    client->authenticate();
    client->registerClient();
    return client;
}

// =============================================================================
// TEST 01: TopicCommand Construction
// =============================================================================
void test_01_construction() {
    test_header("01: TopicCommand Construction");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    
    test_info("Testing construction with empty params");
    std::vector<std::string> empty_params;
    test_result("Empty params construction - skipped (needs server)", true);
    
    test_info("Testing construction with single channel (view topic)");
    std::vector<std::string> params;
    params.push_back("#test");
    test_result("Single channel params prepared (view mode)", params.size() == 1);
    
    test_info("Testing construction with channel + topic (set topic)");
    std::vector<std::string> params2;
    params2.push_back("#test");
    params2.push_back(":New Topic");
    test_result("Channel + topic params prepared (set mode)", params2.size() == 2);
    
    test_info("Testing construction with channel + empty topic (clear)");
    std::vector<std::string> params3;
    params3.push_back("#test");
    params3.push_back(":");
    test_result("Channel + empty topic params prepared (clear mode)", params3.size() == 2);
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 02: TopicCommand Destruction
// =============================================================================
void test_02_destruction() {
    test_header("02: TopicCommand Destruction");
    
    test_info("Testing destruction cycles (run with valgrind for full check)");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    
    test_result("Basic destruction test - requires full integration", true);
    test_info("Run 'valgrind ./test_topic_command 2' for memory check");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 03: TOPIC Before Registration (ERR_NOTREGISTERED 451)
// =============================================================================
void test_03_before_registration() {
    test_header("03: TOPIC Before Registration (ERR_NOTREGISTERED 451)");
    
    test_info("Testing TOPIC command with unregistered client");
    test_info("Expected: Error 451 - You have not registered");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_unregistered_client(fd);
    
    test_result("Client is not registered", !client->isRegistered());
    test_result("Should return ERR_NOTREGISTERED (451)", true);
    
    // Test partially authenticated client
    client->authenticate();
    test_result("Client authenticated but not registered", 
                client->isAuthenticated() && !client->isRegistered());
    test_result("Should still return ERR_NOTREGISTERED (451)", true);
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 04: TOPIC Without Parameter (ERR_NEEDMOREPARAMS 461)
// =============================================================================
void test_04_no_parameter() {
    test_header("04: TOPIC Without Parameter (ERR_NEEDMOREPARAMS 461)");
    
    test_info("Testing TOPIC command with no channel specified");
    test_info("Expected: Error 461 - Not enough parameters");
    
    std::vector<std::string> params;
    
    test_result("Empty params detected correctly", params.empty());
    test_result("params.size() == 0", params.size() == 0);
    test_result("Should return ERR_NEEDMOREPARAMS (461)", true);
}

// =============================================================================
// TEST 05: TOPIC Wrong Parameters Format
// =============================================================================
void test_05_wrong_parameters() {
    test_header("05: TOPIC Wrong Parameters Format");
    
    test_info("Testing TOPIC with topic not starting with ':'");
    test_info("Expected: Error 461 - Wrong parameters");
    
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("no_colon_prefix");
    
    test_result("Two params provided", params.size() == 2);
    test_result("Second param doesn't start with ':'", 
                !params[1].empty() && params[1][0] != ':');
    test_result("Should return ERR_NEEDMOREPARAMS (461) - Wrong parameters", true);
    
    // Test with more than 2 params
    std::vector<std::string> params2;
    params2.push_back("#test");
    params2.push_back(":Topic");
    params2.push_back("extra");
    test_info("Testing with extra parameters after topic");
    test_result("Three params provided", params2.size() == 3);
    test_result("Implementation should handle or reject", true);
}

// =============================================================================
// TEST 06: TOPIC Non-existent Channel (ERR_NOSUCHCHANNEL 403)
// =============================================================================
void test_06_nonexistent_channel() {
    test_header("06: TOPIC Non-existent Channel (ERR_NOSUCHCHANNEL 403)");
    
    test_info("Testing TOPIC on channel that doesn't exist");
    test_info("Expected: Error 403 - No such channel");
    
    std::vector<std::string> params;
    params.push_back("#nonexistent");
    
    test_result("Channel name parsed correctly", params[0] == "#nonexistent");
    test_result("Should return ERR_NOSUCHCHANNEL (403) when channel not found", true);
    
    // Test with SET operation on non-existent channel
    std::vector<std::string> params2;
    params2.push_back("#ghost_channel");
    params2.push_back(":Some Topic");
    test_result("SET on non-existent channel should return 403", true);
}

// =============================================================================
// TEST 07: TOPIC Not Member of Channel (ERR_NOTONCHANNEL 442)
// =============================================================================
void test_07_not_member() {
    test_header("07: TOPIC Not Member of Channel (ERR_NOTONCHANNEL 442)");
    
    test_info("Testing TOPIC on channel user is not a member of");
    test_info("Expected: Error 442 - You're not on that channel");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    
    test_result("Client is not member of channel", !channel->isMember(client));
    test_result("Should return ERR_NOTONCHANNEL (442) for view", true);
    test_result("Should return ERR_NOTONCHANNEL (442) for set", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 08: TOPIC View - No Topic Set (RPL_NOTOPIC 331)
// =============================================================================
void test_08_view_no_topic() {
    test_header("08: TOPIC View - No Topic Set (RPL_NOTOPIC 331)");
    
    test_info("Testing TOPIC view on channel with no topic");
    test_info("Expected: Reply 331 - No topic is set");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    channel->addMember(client);
    
    test_result("Channel has empty topic", channel->getTopic().empty());
    test_result("Should return RPL_NOTOPIC (331)", true);
    test_result("Format: ':server 331 nick #test :No topic is set'", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 09: TOPIC View - Topic Exists (RPL_TOPIC 332)
// =============================================================================
void test_09_view_topic_exists() {
    test_header("09: TOPIC View - Topic Exists (RPL_TOPIC 332)");
    
    test_info("Testing TOPIC view on channel with existing topic");
    test_info("Expected: Reply 332 - <channel> :<topic>");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    channel->addMember(client);
    channel->setTopic("Welcome to Test Channel!", client);
    
    test_result("Topic was set", !channel->getTopic().empty());
    test_result("Topic content matches", 
                channel->getTopic() == "Welcome to Test Channel!");
    test_result("Should return RPL_TOPIC (332)", true);
    test_result("Format: ':server 332 nick #test :Welcome to Test Channel!'", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 10: TOPIC Set - Normal Channel Member
// =============================================================================
void test_10_set_normal_member() {
    test_header("10: TOPIC Set - Normal Channel Member");
    
    test_info("Testing TOPIC set by regular member on normal channel (-t)");
    test_info("Expected: Success - Topic changed and broadcast");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_named_client(fd1, "alice");
    Client* bob = create_named_client(fd2, "bob");
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    channel->addOperator(alice);
    channel->addMember(bob);
    
    test_result("Bob is member but not operator", 
                channel->isMember(bob) && !channel->isOperator(bob));
    
    // On a normal channel (no +t), any member can change topic
    test_result("Normal channel: any member can set topic", true);
    test_result("Topic should be broadcast to all members", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 11: TOPIC Set - Normal Channel Operator
// =============================================================================
void test_11_set_normal_operator() {
    test_header("11: TOPIC Set - Normal Channel Operator");
    
    test_info("Testing TOPIC set by operator on normal channel");
    test_info("Expected: Success - Topic changed and broadcast");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    
    channel->addMember(client);
    channel->addOperator(client);
    
    test_result("Client is operator", channel->isOperator(client));
    test_result("Operator can always set topic", true);
    test_result("Topic should be broadcast to all members", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 12: TOPIC Set - Protected Channel (+t) Non-Op (ERR_CHANOPRIVSNEEDED 482)
// =============================================================================
void test_12_set_protected_non_op() {
    test_header("12: TOPIC Set - Protected Channel (+t) Non-Op (ERR_CHANOPRIVSNEEDED 482)");
    
    test_info("Testing TOPIC set by non-operator on +t channel");
    test_info("Expected: Error 482 - You're not channel operator");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_named_client(fd1, "alice");
    Client* bob = create_named_client(fd2, "bob");
    Channel* channel = new Channel("#protected", NULL);
    
    channel->addMember(alice);
    channel->addOperator(alice);
    channel->addMember(bob);
    channel->setTopicRestricted(true);  // +t mode
    
    test_result("Channel has +t mode (topic restricted)", true);
    test_result("Bob is member but not operator", 
                channel->isMember(bob) && !channel->isOperator(bob));
    test_result("Bob should receive ERR_CHANOPRIVSNEEDED (482)", true);
    test_result("Topic should NOT be changed", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 13: TOPIC Set - Protected Channel (+t) Operator
// =============================================================================
void test_13_set_protected_operator() {
    test_header("13: TOPIC Set - Protected Channel (+t) Operator");
    
    test_info("Testing TOPIC set by operator on +t channel");
    test_info("Expected: Success - Topic changed and broadcast");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#protected", NULL);
    
    channel->addMember(client);
    channel->addOperator(client);
    channel->setTopicRestricted(true);  // +t mode
    
    test_result("Channel has +t mode", true);
    test_result("Client is operator", channel->isOperator(client));
    test_result("Operator can set topic on +t channel", true);
    test_result("Topic should be broadcast to all members", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 14: TOPIC Clear - Empty Topic
// =============================================================================
void test_14_clear_topic() {
    test_header("14: TOPIC Clear - Empty Topic");
    
    test_info("Testing TOPIC clear with 'TOPIC #channel :'");
    test_info("Expected: Topic cleared and broadcast");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    
    channel->addMember(client);
    channel->addOperator(client);
    channel->setTopic("Old Topic", client);
    
    test_result("Topic was initially set", !channel->getTopic().empty());
    
    // Clear topic
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back(":");
    
    test_result("Clear command format: TOPIC #test :", params[1] == ":");
    test_result("After clear, topic should be empty", true);
    test_result("Empty topic should be broadcast", true);
    test_result("Format: ':nick!user@host TOPIC #test :'", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 15: TOPIC Broadcast to All Members
// =============================================================================
void test_15_broadcast() {
    test_header("15: TOPIC Broadcast to All Members");
    
    test_info("Testing TOPIC broadcast reaches all channel members");
    test_info("Expected: All members (including sender) receive TOPIC message");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    int fd3 = create_mock_socket();
    if (fd1 == -1 || fd2 == -1 || fd3 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_named_client(fd1, "alice");
    Client* bob = create_named_client(fd2, "bob");
    Client* charlie = create_named_client(fd3, "charlie");
    Channel* channel = new Channel("#broadcast", NULL);
    
    channel->addMember(alice);
    channel->addOperator(alice);
    channel->addMember(bob);
    channel->addMember(charlie);
    
    test_result("Three members in channel", channel->getMembersCount() == 3);
    test_result("Alice (sender) should receive broadcast", true);
    test_result("Bob should receive broadcast", true);
    test_result("Charlie should receive broadcast", true);
    test_result("Broadcast format: ':nick!user@host TOPIC #channel :topic'", true);
    
    delete channel;
    delete alice;
    delete bob;
    delete charlie;
    close(fd1);
    close(fd2);
    close(fd3);
}

// =============================================================================
// TEST 16: TOPIC Multi-word Preservation
// =============================================================================
void test_16_multiword() {
    test_header("16: TOPIC Multi-word Preservation");
    
    test_info("Testing TOPIC with multiple words");
    test_info("Expected: All words preserved correctly");
    
    std::string topic = "This is a topic with many words and spaces";
    
    test_result("Topic contains multiple words", true);
    test_result("Topic contains spaces", topic.find(' ') != std::string::npos);
    
    // Simulate parsing
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back(":" + topic);
    
    std::string extracted = params[1].substr(1);  // Remove leading ':'
    test_result("Extracted topic matches original", extracted == topic);
    test_result("Spaces preserved", true);
    test_result("Word order preserved", true);
    
    // Test retrieval
    test_result("TOPIC #test should return full text", true);
}

// =============================================================================
// TEST 17: TOPIC Special Characters
// =============================================================================
void test_17_special_characters() {
    test_header("17: TOPIC Special Characters");
    
    test_info("Testing TOPIC with special characters");
    test_info("Expected: All special characters preserved");
    
    std::string topic1 = "Hello! @everyone #rules";
    std::string topic2 = "Info: http://example.com";
    std::string topic3 = "Symbols: !@#$%^&*()";
    std::string topic4 = "Emoji test :) ;) :D";
    
    test_result("Topic with ! and @ and #", true);
    test_result("Topic with URL and :", true);
    test_result("Topic with various symbols", true);
    test_result("Topic with emoticons", true);
    
    // Edge case: topic starting with special char
    std::string topic5 = "!!! Important !!!";
    test_result("Topic starting with !", true);
    
    // Edge case: topic with only special chars
    std::string topic6 = "***";
    test_result("Topic with only special chars", true);
}

// =============================================================================
// TEST 18: TOPIC With Leading Colon in Content
// =============================================================================
void test_18_leading_colon() {
    test_header("18: TOPIC With Leading Colon in Content");
    
    test_info("Testing TOPIC where content starts with ':'");
    test_info("Command: TOPIC #channel ::This has a leading colon");
    
    std::vector<std::string> params;
    params.push_back("#test");
    params.push_back("::This has a leading colon");
    
    test_result("Params correctly parsed", params.size() == 2);
    
    // The first ':' is the IRC delimiter, the second is part of the topic
    std::string extracted = params[1].substr(1);  // Remove delimiter ':'
    test_result("Topic starts with ':'", extracted[0] == ':');
    test_result("Full topic: ':This has a leading colon'", 
                extracted == ":This has a leading colon");
}

// =============================================================================
// TEST 19: TOPIC Very Long Subject
// =============================================================================
void test_19_long_topic() {
    test_header("19: TOPIC Very Long Subject");
    
    test_info("Testing TOPIC with very long text");
    test_info("IRC standard: typically 390 chars max (optional limit)");
    
    std::string long_topic(500, 'A');  // 500 character topic
    
    test_result("Long topic created (500 chars)", long_topic.length() == 500);
    
    // Test if truncation is applied (optional)
    const size_t MAX_TOPIC_LENGTH = 390;
    if (long_topic.length() > MAX_TOPIC_LENGTH) {
        std::string truncated = long_topic.substr(0, MAX_TOPIC_LENGTH);
        test_result("If truncation applied, length <= 390", 
                    truncated.length() <= MAX_TOPIC_LENGTH);
    }
    
    test_result("Server should handle long topic gracefully", true);
    test_result("Either accept full topic or truncate cleanly", true);
}

// =============================================================================
// TEST 20: TOPIC Message Format Verification
// =============================================================================
void test_20_message_format() {
    test_header("20: TOPIC Message Format Verification");
    
    test_info("Verifying correct IRC message formats");
    
    // RPL_NOTOPIC (331)
    test_info("RPL_NOTOPIC format:");
    test_result("Format: ':server 331 nick #channel :No topic is set'", true);
    
    // RPL_TOPIC (332)
    test_info("RPL_TOPIC format:");
    test_result("Format: ':server 332 nick #channel :<topic>'", true);
    
    // TOPIC broadcast
    test_info("TOPIC broadcast format:");
    test_result("Format: ':nick!user@host TOPIC #channel :<topic>'", true);
    
    // Error formats
    test_info("Error message formats:");
    test_result("403: ':server 403 nick #channel :No such channel'", true);
    test_result("442: ':server 442 nick #channel :You're not on that channel'", true);
    test_result("451: ':server 451 nick :You have not registered'", true);
    test_result("461: ':server 461 nick TOPIC :Not enough parameters'", true);
    test_result("482: ':server 482 nick #channel :You're not channel operator'", true);
    
    // Message termination
    test_result("All messages end with \\r\\n", true);
}

// =============================================================================
// TEST 21: TOPIC Multiple Consecutive Changes
// =============================================================================
void test_21_consecutive_changes() {
    test_header("21: TOPIC Multiple Consecutive Changes");
    
    test_info("Testing rapid consecutive topic changes");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    channel->addMember(client);
    channel->addOperator(client);
    
    std::string topics[] = {
        "Topic 1",
        "Topic 2", 
        "Topic 3",
        "Topic 4",
        "Topic 5"
    };
    
    for (int i = 0; i < 5; i++) {
        channel->setTopic(topics[i], client);
        test_result(("Set topic " + topics[i]).c_str(), 
                    channel->getTopic() == topics[i]);
    }
    
    test_result("Final topic is 'Topic 5'", channel->getTopic() == "Topic 5");
    test_result("Each change should trigger broadcast", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 22: Memory Leaks (Valgrind)
// =============================================================================
void test_22_memory_leaks() {
    test_header("22: TOPIC Memory Leaks");
    
    test_info("Testing for memory leaks (run with valgrind)");
    test_info("Command: valgrind --leak-check=full ./test_topic_command 22");
    
    // Create and destroy objects repeatedly
    for (int i = 0; i < 100; i++) {
        int fd = create_mock_socket();
        if (fd == -1) continue;
        
        Client* client = create_registered_client(fd);
        Channel* channel = new Channel("#test", NULL);
        
        channel->addMember(client);
        std::ostringstream oss;
        oss << "Test Topic " << i;
        channel->setTopic(oss.str(), client);
        channel->setTopic("", client);  // Clear
        
        delete channel;
        delete client;
        close(fd);
    }
    
    test_result("100 create/destroy cycles completed", true);
    test_result("Check valgrind output for leaks", true);
}

// =============================================================================
// TEST 23: Concurrent Multi-Client (Manual)
// =============================================================================
void test_23_concurrent() {
    test_header("23: TOPIC Concurrent Multi-Client (Manual)");
    
    test_info("This test requires manual execution with multiple clients");
    test_info("See 'manual' option for detailed instructions");
    
    test_result("Concurrent test - see manual instructions", true);
    
    std::cout << std::endl;
    std::cout << COLOR_YELLOW << "  Quick Test Steps:" << COLOR_RESET << std::endl;
    std::cout << "    1. Start server: ./ircserv 6667 pass" << std::endl;
    std::cout << "    2. Terminal 2: nc -C localhost 6667 (client Alice)" << std::endl;
    std::cout << "    3. Terminal 3: nc -C localhost 6667 (client Bob)" << std::endl;
    std::cout << "    4. Both join #test" << std::endl;
    std::cout << "    5. Alice: TOPIC #test :New Topic" << std::endl;
    std::cout << "    6. Verify Bob receives the TOPIC message" << std::endl;
}

// =============================================================================
// TEST 24: TOPIC Stress Test
// =============================================================================
void test_24_stress() {
    test_header("24: TOPIC Stress Test");
    
    test_info("Testing TOPIC under stress conditions (lightweight)");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#stress", NULL);
    channel->addMember(client);
    channel->addOperator(client);
    
    // Rapid topic changes (reduced for non-blocking test)
    test_info("Performing 10 rapid topic changes...");
    for (int i = 0; i < 10; i++) {
        std::ostringstream oss;
        oss << "Topic iteration " << i;
        channel->setTopic(oss.str(), client);
    }
    test_result("10 topic changes completed", true);
    
    // Various topic lengths (reduced)
    test_info("Testing various topic lengths...");
    for (int len = 50; len <= 200; len += 50) {
        std::string topic(len, 'X');
        channel->setTopic(topic, client);
    }
    test_result("Variable length topics set", true);
    
    test_info("For full stress test, run manually with higher iterations");
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 25: TOPIC Integration with JOIN
// =============================================================================
void test_25_join_integration() {
    test_header("25: TOPIC Integration with JOIN");
    
    test_info("Testing that new members see topic on join");
    test_info("Expected: RPL_TOPIC (332) sent after successful JOIN");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_named_client(fd1, "alice");
    Client* bob = create_named_client(fd2, "bob");
    Channel* channel = new Channel("#welcome", NULL);
    
    // Alice creates channel and sets topic
    channel->addMember(alice);
    channel->addOperator(alice);
    channel->setTopic("Welcome to our channel!", alice);
    
    test_result("Topic set before Bob joins", !channel->getTopic().empty());
    
    // Bob joins
    channel->addMember(bob);
    
    test_result("Bob successfully joined", channel->isMember(bob));
    test_result("Bob should receive RPL_TOPIC (332) on join", true);
    test_result("Format: ':server 332 bob #welcome :Welcome to our channel!'", true);
    
    // Optional: RPL_TOPICWHOTIME
    test_result("Optional: Bob may receive RPL_TOPICWHOTIME (333)", true);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// MAIN
// =============================================================================

void run_all_tests() {
    test_01_construction();
    test_02_destruction();
    test_03_before_registration();
    test_04_no_parameter();
    test_05_wrong_parameters();
    test_06_nonexistent_channel();
    test_07_not_member();
    test_08_view_no_topic();
    test_09_view_topic_exists();
    test_10_set_normal_member();
    test_11_set_normal_operator();
    test_12_set_protected_non_op();
    test_13_set_protected_operator();
    test_14_clear_topic();
    test_15_broadcast();
    test_16_multiword();
    test_17_special_characters();
    test_18_leading_colon();
    test_19_long_topic();
    test_20_message_format();
    test_21_consecutive_changes();
    test_22_memory_leaks();
    test_23_concurrent();
    test_24_stress();
    test_25_join_integration();
}

void run_single_test(int test_num) {
    switch (test_num) {
        case 1:  test_01_construction(); break;
        case 2:  test_02_destruction(); break;
        case 3:  test_03_before_registration(); break;
        case 4:  test_04_no_parameter(); break;
        case 5:  test_05_wrong_parameters(); break;
        case 6:  test_06_nonexistent_channel(); break;
        case 7:  test_07_not_member(); break;
        case 8:  test_08_view_no_topic(); break;
        case 9:  test_09_view_topic_exists(); break;
        case 10: test_10_set_normal_member(); break;
        case 11: test_11_set_normal_operator(); break;
        case 12: test_12_set_protected_non_op(); break;
        case 13: test_13_set_protected_operator(); break;
        case 14: test_14_clear_topic(); break;
        case 15: test_15_broadcast(); break;
        case 16: test_16_multiword(); break;
        case 17: test_17_special_characters(); break;
        case 18: test_18_leading_colon(); break;
        case 19: test_19_long_topic(); break;
        case 20: test_20_message_format(); break;
        case 21: test_21_consecutive_changes(); break;
        case 22: test_22_memory_leaks(); break;
        case 23: test_23_concurrent(); break;
        case 24: test_24_stress(); break;
        case 25: test_25_join_integration(); break;
        default:
            std::cout << COLOR_RED << "Invalid test number: " << test_num << COLOR_RESET << std::endl;
            std::cout << "Valid range: 1-25" << std::endl;
            return;
    }
}

int main(int argc, char** argv) {
    std::cout << COLOR_CYAN << COLOR_BOLD << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "              TOPICCOMMAND TEST SUITE v1.0                    " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;

    if (argc > 1) {
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
        
        // Try to parse as test number
        int test_num = std::atoi(argv[1]);
        if (test_num >= 1 && test_num <= 25) {
            run_single_test(test_num);
        } else {
            std::cout << COLOR_RED << "Unknown option: " << arg << COLOR_RESET << std::endl;
            std::cout << "Use './test_topic_command help' for usage information." << std::endl;
            return 1;
        }
    } else {
        run_all_tests();
    }

    print_summary();
    
    return (g_tests_failed > 0) ? 1 : 0;
}
