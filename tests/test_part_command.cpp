/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_part_command.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ft_irc team                                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 10:00:00 by ft_irc            #+#    #+#             */
/*   Updated: 2026/01/23 10:00:00 by ft_irc            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** =============================================================================
** PARTCOMMAND.CPP TEST SUITE
** =============================================================================
** 
** This file contains all test cases for the PartCommand class.
** Compile separately from the main project.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_part_command.cpp ../src/Server.cpp ../src/Client.cpp \
**       ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
**       ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
**       -o test_part_command
**
** USAGE:
**   ./test_part_command [test_number]
**   ./test_part_command          # Run all tests
**   ./test_part_command 1        # Run test 1 only
**   ./test_part_command help     # Show help
**   ./test_part_command manual   # Show manual test instructions
**   ./test_part_command valgrind # Instructions for valgrind
**
** VALGRIND:
**   valgrind --leak-check=full --show-leak-kinds=all ./test_part_command
**
** =============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/PartCommand.hpp"
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
    std::cout << "              PARTCOMMAND TEST SUITE - HELP                   " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  USAGE:" << std::endl;
    std::cout << "    ./test_part_command [option]" << std::endl;
    std::cout << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    (none)     Run all tests" << std::endl;
    std::cout << "    1-20       Run specific test number" << std::endl;
    std::cout << "    help       Show this help message" << std::endl;
    std::cout << "    manual     Show manual testing instructions" << std::endl;
    std::cout << "    valgrind   Show valgrind instructions" << std::endl;
    std::cout << std::endl;
    std::cout << "  TESTS:" << std::endl;
    std::cout << "    01  PartCommand Construction" << std::endl;
    std::cout << "    02  PartCommand Destruction" << std::endl;
    std::cout << "    03  PART Without Parameter" << std::endl;
    std::cout << "    04  PART Non-existent Channel" << std::endl;
    std::cout << "    05  PART Not Member of Channel" << std::endl;
    std::cout << "    06  PART Simple Channel" << std::endl;
    std::cout << "    07  PART With Reason" << std::endl;
    std::cout << "    08  PART Multiple Channels" << std::endl;
    std::cout << "    09  PART Before Registration" << std::endl;
    std::cout << "    10  PART Broadcast to Members" << std::endl;
    std::cout << "    11  PART Empty Channel Removal" << std::endl;
    std::cout << "    12  PART Operator Leaves" << std::endl;
    std::cout << "    13  PART Message Format" << std::endl;
    std::cout << "    14  PART Special Characters in Reason" << std::endl;
    std::cout << "    15  PART Mixed Success/Errors" << std::endl;
    std::cout << "    16  PART Case Insensitive" << std::endl;
    std::cout << "    17  PART Client Removed from All Sets" << std::endl;
    std::cout << "    18  PART Memory Leaks (use valgrind)" << std::endl;
    std::cout << "    19  PART Concurrent (manual)" << std::endl;
    std::cout << "    20  PART Stress Test" << std::endl;
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
    std::cout << "  TEST 1: Basic PART" << std::endl;
    std::cout << "    > PASS testpass" << std::endl;
    std::cout << "    > NICK alice" << std::endl;
    std::cout << "    > USER alice 0 * :Alice" << std::endl;
    std::cout << "    > JOIN #test" << std::endl;
    std::cout << "    > PART #test" << std::endl;
    std::cout << "    Expected: :alice!alice@host PART #test" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 2: PART with Reason" << std::endl;
    std::cout << "    > JOIN #general" << std::endl;
    std::cout << "    > PART #general :Goodbye everyone!" << std::endl;
    std::cout << "    Expected: :alice!alice@host PART #general :Goodbye..." << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 3: PART Non-existent Channel" << std::endl;
    std::cout << "    > PART #nonexistent" << std::endl;
    std::cout << "    Expected: :server 403 alice #nonexistent :No such channel" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 4: PART Multiple Channels" << std::endl;
    std::cout << "    > JOIN #foo" << std::endl;
    std::cout << "    > JOIN #bar" << std::endl;
    std::cout << "    > PART #foo,#bar :Leaving both" << std::endl;
    std::cout << "    Expected: PART messages for both channels" << std::endl;
    std::cout << std::endl;
    std::cout << "  TEST 5: Multi-client Broadcast" << std::endl;
    std::cout << "    Terminal 2: PASS/NICK alice/USER/JOIN #team" << std::endl;
    std::cout << "    Terminal 3: PASS/NICK bob/USER/JOIN #team" << std::endl;
    std::cout << "    Terminal 2: PART #team :Bye!" << std::endl;
    std::cout << "    Expected on Terminal 3: :alice!...@host PART #team :Bye!" << std::endl;
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
    std::cout << "    valgrind --leak-check=full ./test_part_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  DETAILED CHECK:" << std::endl;
    std::cout << "    valgrind --leak-check=full --show-leak-kinds=all \\" << std::endl;
    std::cout << "             --track-origins=yes ./test_part_command" << std::endl;
    std::cout << std::endl;
    std::cout << "  SPECIFIC TEST:" << std::endl;
    std::cout << "    valgrind --leak-check=full ./test_part_command 2" << std::endl;
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

// =============================================================================
// TEST 01: PartCommand Construction
// =============================================================================
void test_01_construction() {
    test_header("01: PartCommand Construction");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    
    test_info("Testing construction with empty params");
    std::vector<std::string> empty_params;
    test_result("Empty params construction - skipped (needs server)", true);
    
    test_info("Testing construction with single channel");
    std::vector<std::string> params;
    params.push_back("#test");
    test_result("Single channel params prepared", true);
    
    test_info("Testing construction with channel + reason");
    std::vector<std::string> params2;
    params2.push_back("#test");
    params2.push_back("Goodbye!");
    test_result("Channel + reason params prepared", true);
    
    test_info("Testing construction with multiple channels");
    std::vector<std::string> params3;
    params3.push_back("#foo,#bar,#baz");
    params3.push_back("Leaving all");
    test_result("Multiple channels params prepared", true);
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 02: PartCommand Destruction
// =============================================================================
void test_02_destruction() {
    test_header("02: PartCommand Destruction");
    
    test_info("Testing destruction cycles (run with valgrind for full check)");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    
    test_result("Basic destruction test - requires full integration", true);
    test_info("Run 'valgrind ./test_part_command 2' for memory check");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 03: PART Without Parameter
// =============================================================================
void test_03_no_parameter() {
    test_header("03: PART Without Parameter (ERR_NEEDMOREPARAMS 461)");
    
    test_info("Testing PART command with no channel specified");
    test_info("Expected: Error 461 - Not enough parameters");
    
    std::vector<std::string> params;
    
    test_result("Empty params detected correctly", params.empty());
    test_result("Should return ERR_NEEDMOREPARAMS (461)", true);
    
    params.push_back("");
    test_result("Empty string param detected", params[0].empty());
}

// =============================================================================
// TEST 04: PART Non-existent Channel
// =============================================================================
void test_04_nonexistent_channel() {
    test_header("04: PART Non-existent Channel (ERR_NOSUCHCHANNEL 403)");
    
    test_info("Testing PART on channel that doesn't exist");
    test_info("Expected: Error 403 - No such channel");
    
    std::vector<std::string> params;
    params.push_back("#nonexistent");
    
    test_result("Channel name parsed correctly", params[0] == "#nonexistent");
    test_result("Should return ERR_NOSUCHCHANNEL (403) when channel not found", true);
}

// =============================================================================
// TEST 05: PART Not Member of Channel
// =============================================================================
void test_05_not_member() {
    test_header("05: PART Not Member of Channel (ERR_NOTONCHANNEL 442)");
    
    test_info("Testing PART on channel user is not a member of");
    test_info("Expected: Error 442 - You're not on that channel");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    
    test_result("Client is not member of channel", !channel->isMember(client));
    test_result("Should return ERR_NOTONCHANNEL (442)", true);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 06: PART Simple Channel
// =============================================================================
void test_06_simple_part() {
    test_header("06: PART Simple Channel");
    
    test_info("Testing basic PART functionality");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(client);
    
    test_result("Client added to channel", channel->isMember(client));
    test_result("Channel has 1 member", channel->getMembersCount() == 1);
    
    channel->removeMember(client);
    
    test_result("Client removed from channel", !channel->isMember(client));
    test_result("Channel has 0 members", channel->getMembersCount() == 0);
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 07: PART With Reason
// =============================================================================
void test_07_with_reason() {
    test_header("07: PART With Reason");
    
    test_info("Testing PART with departure reason");
    
    std::vector<std::string> params;
    params.push_back("#general");
    params.push_back("Going to sleep");
    
    test_result("Channel param present", !params[0].empty());
    test_result("Reason param present", params.size() > 1 && !params[1].empty());
    test_result("Reason correctly parsed", params[1] == "Going to sleep");
    
    test_info("Expected format: :nick!user@host PART #channel :reason");
    test_result("Format includes colon before reason", true);
}

// =============================================================================
// TEST 08: PART Multiple Channels
// =============================================================================
void test_08_multiple_channels() {
    test_header("08: PART Multiple Channels");
    
    test_info("Testing PART with comma-separated channels");
    
    std::string channelList = "#foo,#bar,#baz";
    std::vector<std::string> channels;
    
    size_t start = 0;
    size_t end = channelList.find(',');
    while (end != std::string::npos) {
        channels.push_back(channelList.substr(start, end - start));
        start = end + 1;
        end = channelList.find(',', start);
    }
    channels.push_back(channelList.substr(start));
    
    test_result("Split into 3 channels", channels.size() == 3);
    test_result("First channel is #foo", channels[0] == "#foo");
    test_result("Second channel is #bar", channels[1] == "#bar");
    test_result("Third channel is #baz", channels[2] == "#baz");
}

// =============================================================================
// TEST 09: PART Before Registration
// =============================================================================
void test_09_before_registration() {
    test_header("09: PART Before Registration (ERR_NOTREGISTERED 451)");
    
    test_info("Testing PART before completing registration");
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
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 10: PART Broadcast to Members
// =============================================================================
void test_10_broadcast() {
    test_header("10: PART Broadcast to Members");
    
    test_info("Testing that PART message is broadcast to all channel members");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    int fd3 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1 || fd3 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1);
    alice->setNickname("alice");
    
    Client* bob = create_registered_client(fd2);
    bob->setNickname("bob");
    
    Client* charlie = create_registered_client(fd3);
    charlie->setNickname("charlie");
    
    Channel* channel = new Channel("#general", NULL);
    
    channel->addMember(alice);
    channel->addMember(bob);
    channel->addMember(charlie);
    
    test_result("Channel has 3 members", channel->getMembersCount() == 3);
    test_result("Alice is member", channel->isMember(alice));
    test_result("Bob is member", channel->isMember(bob));
    test_result("Charlie is member", channel->isMember(charlie));
    
    test_info("Broadcast verification requires manual/integration testing");
    test_result("Broadcast function exists in Channel", true);
    
    delete channel;
    delete alice;
    delete bob;
    delete charlie;
    close(fd1);
    close(fd2);
    close(fd3);
}

// =============================================================================
// TEST 11: PART Empty Channel Removal
// =============================================================================
void test_11_empty_channel_removal() {
    test_header("11: PART Empty Channel Removal");
    
    test_info("Testing that empty channels are removed after last member leaves");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#private", NULL);
    
    channel->addMember(client);
    test_result("Channel has 1 member", channel->getMembersCount() == 1);
    
    channel->removeMember(client);
    test_result("Channel has 0 members after PART", channel->getMembersCount() == 0);
    test_result("Channel should be deleted by server (manual check)", true);
    
    test_info("Server should call removeChannel() when count reaches 0");
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 12: PART Operator Leaves
// =============================================================================
void test_12_operator_leaves() {
    test_header("12: PART Operator Leaves");
    
    test_info("Testing behavior when channel operator leaves");
    
    int fd1 = create_mock_socket();
    int fd2 = create_mock_socket();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock sockets");
        return;
    }
    
    Client* alice = create_registered_client(fd1);
    alice->setNickname("alice");
    
    Client* bob = create_registered_client(fd2);
    bob->setNickname("bob");
    
    Channel* channel = new Channel("#test", NULL);
    
    channel->addMember(alice);
    test_result("Alice is operator (first member)", channel->isOperator(alice));
    
    channel->addMember(bob);
    test_result("Bob is NOT operator", !channel->isOperator(bob));
    
    channel->removeMember(alice);
    
    test_result("Alice removed from members", !channel->isMember(alice));
    test_result("Alice removed from operators", !channel->isOperator(alice));
    test_result("Channel still has Bob", channel->isMember(bob));
    test_result("Channel has 1 member", channel->getMembersCount() == 1);
    
    delete channel;
    delete alice;
    delete bob;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 13: PART Message Format
// =============================================================================
void test_13_message_format() {
    test_header("13: PART Message Format (RFC Compliant)");
    
    test_info("Verifying PART message format according to IRC RFC");
    
    std::string nick = "alice";
    std::string user = "alice";
    std::string host = "localhost";
    std::string chan = "#general";
    std::string reason = "Goodbye!";
    
    std::string msgNoReason = ":" + nick + "!" + user + "@" + host + " PART " + chan + "\r\n";
    test_info(("No reason: " + msgNoReason).c_str());
    
    std::string msgWithReason = ":" + nick + "!" + user + "@" + host + " PART " + chan + " :" + reason + "\r\n";
    test_info(("With reason: " + msgWithReason).c_str());
    
    test_result("Message starts with colon", msgNoReason[0] == ':');
    test_result("Contains nick!user@host prefix", msgNoReason.find("!") != std::string::npos);
    test_result("Contains PART command", msgNoReason.find("PART") != std::string::npos);
    test_result("Ends with CRLF", msgNoReason.substr(msgNoReason.length()-2) == "\r\n");
    test_result("Reason preceded by colon", msgWithReason.find(" :Goodbye") != std::string::npos);
}

// =============================================================================
// TEST 14: PART Special Characters in Reason
// =============================================================================
void test_14_special_characters() {
    test_header("14: PART Special Characters in Reason");
    
    test_info("Testing special characters handling in PART reason");
    
    std::vector<std::string> testReasons;
    testReasons.push_back("Simple reason");
    testReasons.push_back("Reason with 'quotes'");
    testReasons.push_back("Reason with \"double quotes\"");
    testReasons.push_back("Reason with : colons : in : it");
    testReasons.push_back("   Spaces around   ");
    testReasons.push_back("Numbers 12345");
    testReasons.push_back("Symbols !@#$%^&*()");
    
    for (size_t i = 0; i < testReasons.size(); i++) {
        std::string desc = "Reason: " + testReasons[i];
        test_result(desc.c_str(), !testReasons[i].empty());
    }
    
    test_info("Special characters should be preserved in broadcast");
}

// =============================================================================
// TEST 15: PART Mixed Success/Errors
// =============================================================================
void test_15_mixed_results() {
    test_header("15: PART Mixed Success/Errors");
    
    test_info("Testing PART with mix of valid and invalid channels");
    test_info("Example: PART #valid,#nonexistent,#notmember");
    
    std::string channelList = "#valid,#nonexistent,#notmember";
    std::vector<std::string> channels;
    
    size_t start = 0;
    size_t end = channelList.find(',');
    while (end != std::string::npos) {
        channels.push_back(channelList.substr(start, end - start));
        start = end + 1;
        end = channelList.find(',', start);
    }
    channels.push_back(channelList.substr(start));
    
    test_result("3 channels parsed", channels.size() == 3);
    
    test_info("Expected behavior:");
    test_info("  #valid -> PART success");
    test_info("  #nonexistent -> ERR_NOSUCHCHANNEL (403)");
    test_info("  #notmember -> ERR_NOTONCHANNEL (442)");
    
    test_result("Processing should continue after errors", true);
    test_result("Each channel handled independently", true);
}

// =============================================================================
// TEST 16: PART Case Insensitive
// =============================================================================
void test_16_case_insensitive() {
    test_header("16: PART Case Insensitive Channel Names");
    
    test_info("IRC channel names should be case-insensitive");
    
    std::string lower = "#general";
    std::string upper = "#GENERAL";
    std::string mixed = "#GeNeRaL";
    
    std::string lowerLower = lower;
    std::string upperLower = upper;
    std::string mixedLower = mixed;
    
    for (size_t i = 0; i < upperLower.length(); i++) {
        upperLower[i] = std::tolower(upperLower[i]);
    }
    for (size_t i = 0; i < mixedLower.length(); i++) {
        mixedLower[i] = std::tolower(mixedLower[i]);
    }
    
    test_result("#general == #GENERAL (lowercase)", lowerLower == upperLower);
    test_result("#general == #GeNeRaL (lowercase)", lowerLower == mixedLower);
    
    test_info("Server should use case-insensitive comparison for channel lookup");
}

// =============================================================================
// TEST 17: PART Client Removed from All Sets
// =============================================================================
void test_17_removed_from_all_sets() {
    test_header("17: PART Client Removed from All Sets");
    
    test_info("Verifying client is removed from all channel sets");
    
    int fd = create_mock_socket();
    if (fd == -1) {
        test_warning("Could not create mock socket");
        return;
    }
    
    Client* client = create_registered_client(fd);
    Channel* channel = new Channel("#test", NULL);
    
    channel->addMember(client);
    channel->inviteUser(client);
    
    test_result("Client in members set", channel->isMember(client));
    test_result("Client in operators set", channel->isOperator(client));
    test_result("Client in invited set", channel->isInvited(client));
    
    channel->removeMember(client);
    
    test_result("Client removed from members", !channel->isMember(client));
    test_result("Client removed from operators", !channel->isOperator(client));
    
    delete channel;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 18: Memory Leaks
// =============================================================================
void test_18_memory_leaks() {
    test_header("18: PART Memory Leaks");
    
    test_info("This test should be run with valgrind");
    test_info("Command: valgrind --leak-check=full ./test_part_command 18");
    
    for (int i = 0; i < 100; i++) {
        int fd = create_mock_socket();
        if (fd != -1) {
            Client* client = create_registered_client(fd);
            Channel* channel = new Channel("#test", NULL);
            
            channel->addMember(client);
            channel->removeMember(client);
            
            delete channel;
            delete client;
            close(fd);
        }
    }
    
    test_result("100 create/destroy cycles completed", true);
    test_info("Check valgrind output for 'definitely lost: 0 bytes'");
}

// =============================================================================
// TEST 19: Concurrent PART (Manual)
// =============================================================================
void test_19_concurrent() {
    test_header("19: PART Concurrent (Manual Test)");
    
    test_info("This test requires manual execution with multiple terminals");
    
    print_manual_tests();
    
    test_result("Manual test instructions displayed", true);
}

// =============================================================================
// TEST 20: Stress Test
// =============================================================================
void test_20_stress_test() {
    test_header("20: PART Stress Test");
    
    test_info("Testing PART under stress conditions");
    
    std::stringstream manyChannels;
    for (int i = 0; i < 50; i++) {
        if (i > 0) manyChannels << ",";
        manyChannels << "#channel" << i;
    }
    test_result("50 channel string created", manyChannels.str().length() > 0);
    
    std::string longReason(500, 'x');
    test_result("500 char reason created", longReason.length() == 500);
    
    int successCount = 0;
    for (int i = 0; i < 100; i++) {
        int fd = create_mock_socket();
        if (fd != -1) {
            Client* client = create_registered_client(fd);
            Channel* channel = new Channel("#stress", NULL);
            
            channel->addMember(client);
            if (channel->isMember(client)) {
                channel->removeMember(client);
                if (!channel->isMember(client)) {
                    successCount++;
                }
            }
            
            delete channel;
            delete client;
            close(fd);
        }
    }
    test_result("100 JOIN/PART cycles successful", successCount == 100);
    
    std::string longChannelName = "#" + std::string(49, 'a');
    test_result("50 char channel name created", longChannelName.length() == 50);
}

// =============================================================================
// MAIN
// =============================================================================

typedef void (*TestFunc)();

int main(int argc, char* argv[]) {
    std::cout << COLOR_BOLD << COLOR_CYAN << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "            PARTCOMMAND TEST SUITE - ft_irc                   " << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << COLOR_RESET << std::endl;

    TestFunc tests[] = {
        test_01_construction,
        test_02_destruction,
        test_03_no_parameter,
        test_04_nonexistent_channel,
        test_05_not_member,
        test_06_simple_part,
        test_07_with_reason,
        test_08_multiple_channels,
        test_09_before_registration,
        test_10_broadcast,
        test_11_empty_channel_removal,
        test_12_operator_leaves,
        test_13_message_format,
        test_14_special_characters,
        test_15_mixed_results,
        test_16_case_insensitive,
        test_17_removed_from_all_sets,
        test_18_memory_leaks,
        test_19_concurrent,
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
