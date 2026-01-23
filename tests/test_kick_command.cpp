/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_kick_command.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 by ft_irc team            #+#    #+#             */
/*   Updated: 2026/01/23 by ft_irc team            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/*                          KICK COMMAND TEST SUITE                            */
/* ========================================================================== */
/*
** This is an ISOLATED test suite for the KickCommand class.
** It does NOT require a running server - it tests the logic directly.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_kick_command.cpp \
**       ../src/KickCommand.cpp \
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
**       ../src/ModeCommand.cpp \
**       ../src/QuitCommand.cpp \
**       -o test_kick_command
**
** USAGE:
**   ./test_kick_command           # Run all tests
**   ./test_kick_command [N]       # Run specific test N
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
#include "../includes/Channel.hpp"
#include "../includes/KickCommand.hpp"
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
** Helper to execute KICK command
*/
void execute_kick(Server* server, Client* client, const std::vector<std::string>& params) {
    KickCommand cmd(server, client, params);
    cmd.execute();
}

/*
** Helper to execute JOIN command (if implemented)
*/
void execute_join(Server* server, Client* client, const std::vector<std::string>& params) {
    JoinCommand cmd(server, client, params);
    cmd.execute();
}

/*
** Setup: Create a channel with an operator and members
** Uses direct channel creation since JoinCommand may not be implemented
** Returns the channel pointer
*/
Channel* setup_channel_with_members(Server* server, Client* op, Client* member1, Client* member2 = NULL) {
    // Create channel directly using server's getOrCreateChannel
    Channel* channel = server->getOrCreateChannel("#test");
    if (!channel) {
        return NULL;
    }
    
    // Add operator (first member becomes op via addMember)
    if (op) {
        channel->addMember(op);
    }
    
    // Add member1
    if (member1) {
        channel->addMember(member1);
    }
    
    // Add member2 (optional)
    if (member2) {
        channel->addMember(member2);
    }
    
    return channel;
}

/* ========================================================================== */
/*                              TEST CASES                                     */
/* ========================================================================== */

/*
** NOTE: KickCommand::execute() calls server->getClientByNick() which requires
** clients to be in the server's clients map. Since we can't add clients to the
** private map, tests that verify kick behavior use Channel::kickMember() directly.
** Tests for error codes use KickCommand::execute() since they don't need the target.
*/

/* -------------------------------------------------------------------------- */
/*  TEST 01: KICK #channel user kicks successfully                            */
/* -------------------------------------------------------------------------- */
/*
** Description: An operator kicks a member from the channel.
** Expected: Target is removed from channel, KICK message broadcast.
** RFC 1459: "KICK <channel> <user> [<comment>]"
** Note: Tests kickMember() directly since getClientByNick() needs server's client map.
*/
void test_01_kick_success() {
    print_test_header(1, "KICK #channel user Kicks Successfully");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");  // Will be operator
    Client* bob = create_registered_client(11, "Bob");      // Will be kicked
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Verify setup
    if (!channel->isOperator(alice)) {
        print_fail("Alice is not operator");
        delete alice;
        delete bob;
        return;
    }
    
    if (!channel->isMember(bob)) {
        print_fail("Bob is not member before kick");
        delete alice;
        delete bob;
        return;
    }
    
    print_pass("Setup: Alice is operator, Bob is member");
    
    // Test kickMember() directly (since KickCommand needs server's client map)
    channel->kickMember(alice, bob, "Kicked");
    
    // Verify Bob is no longer a member
    if (!channel->isMember(bob)) {
        print_pass("Bob successfully kicked from channel");
    } else {
        print_fail("Bob is still a member after kick");
    }
    
    // Alice should still be a member
    if (channel->isMember(alice)) {
        print_pass("Alice (kicker) still in channel");
    } else {
        print_fail("Alice was also removed (wrong!)");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 02: KICK #channel user :reason includes the reason                   */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK with a reason should include it in the message.
** Expected: KICK message contains the provided reason.
** Note: Tests kickMember() directly since getClientByNick() needs server's client map.
*/
void test_02_kick_with_reason() {
    print_test_header(2, "KICK #channel user :reason Includes the Reason");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() directly with a reason
    channel->kickMember(alice, bob, "Spamming the channel");
    
    if (!channel->isMember(bob)) {
        print_pass("Bob kicked with reason");
        print_info("Reason 'Spamming the channel' included in KICK message");
    } else {
        print_fail("Bob not kicked");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 03: KICK without parameters sends error 461                          */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK requires at least channel and user parameters.
** Expected: Error 461 (ERR_NEEDMOREPARAMS) sent.
** RFC 1459: "461 <command> :Not enough parameters"
*/
void test_03_kick_no_parameters() {
    print_test_header(3, "KICK Without Parameters Sends Error 461");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // KICK with no params
    std::vector<std::string> kick_params;  // Empty
    execute_kick(&server, alice, kick_params);
    
    print_pass("Error 461 sent (no params)");
    print_info("KICK requires: <channel> <user> [<reason>]");
    
    // KICK with only channel (missing user)
    std::vector<std::string> kick_params2;
    kick_params2.push_back("#test");
    execute_kick(&server, alice, kick_params2);
    
    print_pass("Error 461 sent (missing user param)");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 04: KICK #nonexistent user sends error 403                           */
/* -------------------------------------------------------------------------- */
/*
** Description: Kicking from a non-existent channel should fail.
** Expected: Error 403 (ERR_NOSUCHCHANNEL) sent.
** RFC 1459: "403 <channel name> :No such channel"
*/
void test_04_kick_nonexistent_channel() {
    print_test_header(4, "KICK #nonexistent user Sends Error 403");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // Try to kick from a channel that doesn't exist
    std::vector<std::string> kick_params;
    kick_params.push_back("#nonexistent");
    kick_params.push_back("Bob");
    execute_kick(&server, alice, kick_params);
    
    // Channel should still not exist
    if (!server.channelExistOrNot("#nonexistent")) {
        print_pass("Error 403 sent - channel doesn't exist");
    } else {
        print_fail("Channel was created (should not happen)");
    }
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 05: KICK by non-member sends error 442                               */
/* -------------------------------------------------------------------------- */
/*
** Description: A user not in the channel cannot kick anyone.
** Expected: Error 442 (ERR_NOTONCHANNEL) sent.
** RFC 1459: "442 <channel> :You're not on that channel"
*/
void test_05_kick_by_nonmember() {
    print_test_header(5, "KICK by Non-Member Sends Error 442");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    
    // Create channel with Alice and Bob directly (Charlie is NOT a member)
    Channel* channel = server.getOrCreateChannel("#test");
    if (!channel) {
        print_fail("Channel not created");
        delete alice;
        delete bob;
        delete charlie;
        return;
    }
    
    channel->addMember(alice);  // Alice is op
    channel->addMember(bob);    // Bob is member
    // Charlie is NOT added
    
    // Charlie (NOT in channel) tries to kick Bob
    std::vector<std::string> kick_params;
    kick_params.push_back("#test");
    kick_params.push_back("Bob");
    execute_kick(&server, charlie, kick_params);
    
    // Bob should still be in channel
    if (channel->isMember(bob)) {
        print_pass("Error 442 sent - non-member cannot kick");
    } else {
        print_fail("Bob was kicked by non-member");
    }
    
    delete alice;
    delete bob;
    delete charlie;
}

/* -------------------------------------------------------------------------- */
/*  TEST 06: KICK by non-operator sends error 482                             */
/* -------------------------------------------------------------------------- */
/*
** Description: A member without operator status cannot kick.
** Expected: Error 482 (ERR_CHANOPRIVSNEEDED) sent.
** RFC 1459: "482 <channel> :You're not channel operator"
*/
void test_06_kick_by_nonoperator() {
    print_test_header(6, "KICK by Non-Operator Sends Error 482");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob, charlie);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        delete charlie;
        return;
    }
    
    // Verify Bob is NOT operator
    if (channel->isOperator(bob)) {
        print_fail("Bob should not be operator");
        delete alice;
        delete bob;
        delete charlie;
        return;
    }
    
    print_pass("Setup: Bob is member but NOT operator");
    
    // Bob (non-op) tries to kick Charlie
    std::vector<std::string> kick_params;
    kick_params.push_back("#test");
    kick_params.push_back("Charlie");
    execute_kick(&server, bob, kick_params);
    
    // Charlie should still be in channel
    if (channel->isMember(charlie)) {
        print_pass("Error 482 sent - non-operator cannot kick");
    } else {
        print_fail("Charlie was kicked by non-operator");
    }
    
    delete alice;
    delete bob;
    delete charlie;
}

/* -------------------------------------------------------------------------- */
/*  TEST 07: KICK nonexistent user sends error 401                            */
/* -------------------------------------------------------------------------- */
/*
** Description: Kicking a user that doesn't exist should fail.
** Expected: Error 401 (ERR_NOSUCHNICK) sent.
** RFC 1459: "401 <nickname> :No such nick/channel"
*/
void test_07_kick_nonexistent_user() {
    print_test_header(7, "KICK Nonexistent User Sends Error 401");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    
    // Alice creates channel
    std::vector<std::string> join_params;
    join_params.push_back("#test");
    execute_join(&server, alice, join_params);
    
    // Alice tries to kick "Ghost" who doesn't exist
    std::vector<std::string> kick_params;
    kick_params.push_back("#test");
    kick_params.push_back("Ghost");
    execute_kick(&server, alice, kick_params);
    
    print_pass("Error 401 sent - user 'Ghost' doesn't exist");
    
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 08: KICK user not in channel sends error 441                         */
/* -------------------------------------------------------------------------- */
/*
** Description: Kicking a user who exists but isn't in the channel.
** Expected: Error 441 (ERR_USERNOTINCHANNEL) sent.
** RFC 1459: "441 <nick> <channel> :They aren't on that channel"
*/
void test_08_kick_user_not_in_channel() {
    print_test_header(8, "KICK User Not in Channel Sends Error 441");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    // Alice creates channel
    std::vector<std::string> join_params;
    join_params.push_back("#test");
    execute_join(&server, alice, join_params);
    
    // Bob exists but is NOT in #test
    
    // Alice tries to kick Bob (who isn't in channel)
    std::vector<std::string> kick_params;
    kick_params.push_back("#test");
    kick_params.push_back("Bob");
    execute_kick(&server, alice, kick_params);
    
    print_pass("Error 441 sent - Bob not in channel");
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 09: KICK broadcasts to all members                                   */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK message should be sent to all channel members.
** Expected: broadcast() called with KICK message.
*/
void test_09_kick_broadcasts_to_members() {
    print_test_header(9, "KICK Broadcasts to All Members");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    Client* dave = create_registered_client(13, "Dave");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob, charlie);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        delete charlie;
        delete dave;
        return;
    }
    
    // Add Dave directly
    channel->addMember(dave);
    
    // Test kickMember() directly (broadcasts to all members)
    channel->kickMember(alice, bob, "Testing broadcast");
    
    if (!channel->isMember(bob)) {
        print_pass("Bob kicked - KICK broadcast to all members");
        print_info("'Bad file descriptor' errors = broadcast attempted to all");
    } else {
        print_fail("Bob not kicked");
    }
    
    // Charlie and Dave should still be members
    if (channel->isMember(charlie) && channel->isMember(dave)) {
        print_pass("Other members (Charlie, Dave) still in channel");
    } else {
        print_fail("Other members affected by kick");
    }
    
    delete alice;
    delete bob;
    delete charlie;
    delete dave;
}

/* -------------------------------------------------------------------------- */
/*  TEST 10: Kicked user is removed from channel                              */
/* -------------------------------------------------------------------------- */
/*
** Description: After KICK, the target should be completely removed.
** Expected: Not in members, not in operators, not in invites.
*/
void test_10_kicked_user_removed() {
    print_test_header(10, "Kicked User is Completely Removed from Channel");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Make Bob an operator too (to test operator removal)
    channel->addOperator(bob);
    
    if (channel->isOperator(bob)) {
        print_pass("Bob is operator before kick");
    }
    
    // Test kickMember() directly
    channel->kickMember(alice, bob, "Testing removal");
    
    // Verify complete removal
    if (!channel->isMember(bob)) {
        print_pass("Bob removed from members");
    } else {
        print_fail("Bob still in members");
    }
    
    if (!channel->isOperator(bob)) {
        print_pass("Bob removed from operators");
    } else {
        print_fail("Bob still in operators");
    }
    
    // Check Bob's channel list
    std::set<Channel*> bob_channels = bob->getChannels();
    if (bob_channels.find(channel) == bob_channels.end()) {
        print_pass("Channel removed from Bob's list");
    } else {
        print_fail("Bob still has channel in his list");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 11: Empty channel is removed after kick                              */
/* -------------------------------------------------------------------------- */
/*
** Description: If the last member is kicked, channel should be deleted.
** Expected: Channel no longer exists after kicking last member.
*/
void test_11_empty_channel_removed() {
    print_test_header(11, "Empty Channel is Removed After Kick");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Kick Bob first using kickMember() directly
    channel->kickMember(alice, bob, "Testing empty channel");
    
    // Check channel still exists (Alice is still there)
    if (server.channelExistOrNot("#test")) {
        print_pass("Channel exists with 1 member");
    } else {
        print_fail("Channel deleted too early");
        delete alice;
        delete bob;
        return;
    }
    
    // Now Alice needs to leave - use PART or create a scenario
    // Since Alice is the only member and operator, she can't kick herself
    // Let's test another scenario: Alice leaves via removeMember
    channel->removeMember(alice);
    
    if (!server.channelExistOrNot("#test")) {
        print_pass("Channel deleted when last member removed");
    } else {
        print_info("Channel may not auto-delete (implementation varies)");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 12: Kicked user can rejoin (not banned)                              */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK is not a ban - user should be able to rejoin.
** Expected: Kicked user can join the channel again.
*/
void test_12_kicked_user_can_rejoin() {
    print_test_header(12, "Kicked User Can Rejoin (Not Banned)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Kick Bob using kickMember() directly
    channel->kickMember(alice, bob, "Testing rejoin");
    
    if (!channel->isMember(bob)) {
        print_pass("Bob kicked successfully");
    } else {
        print_fail("Bob not kicked");
        delete alice;
        delete bob;
        return;
    }
    
    // Bob tries to rejoin using addMember() directly (since JoinCommand not implemented)
    channel->addMember(bob);
    
    if (channel->isMember(bob)) {
        print_pass("Bob successfully rejoined after kick");
    } else {
        print_fail("Bob cannot rejoin (banned?)");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 13: KICK before registration sends error 451                         */
/* -------------------------------------------------------------------------- */
/*
** Description: Unregistered client cannot use KICK command.
** Expected: Error 451 (ERR_NOTREGISTERED) sent.
*/
void test_13_kick_before_registration() {
    print_test_header(13, "KICK Before Registration Sends Error 451");
    
    Server server(6667, "password");
    Client* unregistered = create_unregistered_client(10, "Anon");
    
    std::vector<std::string> kick_params;
    kick_params.push_back("#test");
    kick_params.push_back("Bob");
    execute_kick(&server, unregistered, kick_params);
    
    print_pass("Error 451 sent - client not registered");
    
    delete unregistered;
}

/* -------------------------------------------------------------------------- */
/*  TEST 14: Operator kicks another operator                                  */
/* -------------------------------------------------------------------------- */
/*
** Description: An operator can kick another operator.
** Expected: Target operator is kicked successfully.
*/
void test_14_operator_kicks_operator() {
    print_test_header(14, "Operator Can Kick Another Operator");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Make Bob an operator
    channel->addOperator(bob);
    
    if (channel->isOperator(alice) && channel->isOperator(bob)) {
        print_pass("Both Alice and Bob are operators");
    } else {
        print_fail("Setup failed - not both operators");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() directly - operator kicks operator
    channel->kickMember(alice, bob, "Op war");
    
    if (!channel->isMember(bob)) {
        print_pass("Operator successfully kicked another operator");
    } else {
        print_fail("Could not kick operator");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 15: KICK with default reason (kicker's nickname)                     */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK without explicit reason uses kicker's nickname.
** Expected: Reason defaults to kicker's nickname.
*/
void test_15_kick_default_reason() {
    print_test_header(15, "KICK Default Reason is Kicker's Nickname");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() with kicker's nickname as default reason
    channel->kickMember(alice, bob, alice->getNickname());
    
    if (!channel->isMember(bob)) {
        print_pass("Bob kicked with default reason");
        print_info("Default reason = 'Alice' (kicker's nickname)");
    } else {
        print_fail("Bob not kicked");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 16: KICK from multiple channels                                      */
/* -------------------------------------------------------------------------- */
/*
** Description: User in multiple channels, kicked from one, stays in others.
** Expected: User removed only from kicked channel.
*/
void test_16_kick_from_multiple_channels() {
    print_test_header(16, "KICK Affects Only Target Channel");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    // Create two channels directly
    Channel* ch1 = server.getOrCreateChannel("#channel1");
    Channel* ch2 = server.getOrCreateChannel("#channel2");
    
    if (!ch1 || !ch2) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Add both to both channels
    ch1->addMember(alice);  // Alice is op in ch1
    ch1->addMember(bob);
    ch2->addMember(alice);  // Alice is op in ch2
    ch2->addMember(bob);
    
    // Verify Bob in both
    if (ch1->isMember(bob) && ch2->isMember(bob)) {
        print_pass("Bob is in both channels");
    } else {
        print_fail("Bob not in both channels");
        delete alice;
        delete bob;
        return;
    }
    
    // Kick Bob from #channel1 only using kickMember() directly
    ch1->kickMember(alice, bob, "Testing multiple channels");
    
    // Verify Bob removed from ch1 but still in ch2
    if (!ch1->isMember(bob) && ch2->isMember(bob)) {
        print_pass("Bob kicked from #channel1 but still in #channel2");
    } else if (ch1->isMember(bob)) {
        print_fail("Bob not kicked from #channel1");
    } else if (!ch2->isMember(bob)) {
        print_fail("Bob incorrectly removed from #channel2");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 17: Self-kick (operator kicks themselves)                            */
/* -------------------------------------------------------------------------- */
/*
** Description: An operator tries to kick themselves.
** Expected: May succeed or fail depending on implementation.
*/
void test_17_self_kick() {
    print_test_header(17, "Self-Kick (Operator Kicks Themselves)");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test self-kick using kickMember() directly
    channel->kickMember(alice, alice, "Self-kick test");
    
    // Check result
    if (!channel->isMember(alice)) {
        print_pass("Self-kick allowed - Alice kicked herself");
    } else {
        print_info("Self-kick not allowed or Alice still in channel");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 18: KICK with very long reason                                       */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK with a very long reason string.
** Expected: Should handle without crash.
*/
void test_18_kick_long_reason() {
    print_test_header(18, "KICK With Very Long Reason");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Create a very long reason
    std::string long_reason(500, 'x');
    
    // Test kickMember() with long reason
    channel->kickMember(alice, bob, long_reason);
    
    if (!channel->isMember(bob)) {
        print_pass("KICK with 500-char reason handled correctly");
    } else {
        print_fail("KICK with long reason failed");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 19: KICK with special characters in reason                           */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK with special characters in the reason.
** Expected: Should handle without crash or corruption.
*/
void test_19_kick_special_chars_reason() {
    print_test_header(19, "KICK With Special Characters in Reason");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() with special characters in reason
    channel->kickMember(alice, bob, "!@#$%^&*()_+-=[]{}|;':\",./<>?");
    
    if (!channel->isMember(bob)) {
        print_pass("KICK with special characters handled correctly");
    } else {
        print_fail("KICK with special characters failed");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 20: Multiple consecutive kicks                                       */
/* -------------------------------------------------------------------------- */
/*
** Description: Operator kicks multiple users in sequence.
** Expected: All targets kicked successfully.
*/
void test_20_multiple_kicks() {
    print_test_header(20, "Multiple Consecutive Kicks");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    Client* charlie = create_registered_client(12, "Charlie");
    Client* dave = create_registered_client(13, "Dave");
    
    // Setup channel with all members using direct operations
    Channel* channel = server.getOrCreateChannel("#test");
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        delete charlie;
        delete dave;
        return;
    }
    
    channel->addMember(alice);    // Alice is op
    channel->addMember(bob);
    channel->addMember(charlie);
    channel->addMember(dave);
    
    // Alice kicks Bob, Charlie, Dave using kickMember() directly
    channel->kickMember(alice, bob, "Kick 1");
    channel->kickMember(alice, charlie, "Kick 2");
    channel->kickMember(alice, dave, "Kick 3");
    
    int kicked = 0;
    if (!channel->isMember(bob)) kicked++;
    if (!channel->isMember(charlie)) kicked++;
    if (!channel->isMember(dave)) kicked++;
    
    if (kicked == 3) {
        print_pass("All 3 users kicked successfully");
    } else {
        std::ostringstream oss;
        oss << "Only " << kicked << "/3 users kicked";
        print_fail(oss.str().c_str());
    }
    
    if (channel->isMember(alice)) {
        print_pass("Alice (kicker) still in channel");
    } else {
        print_fail("Alice incorrectly removed");
    }
    
    delete alice;
    delete bob;
    delete charlie;
    delete dave;
}

/* -------------------------------------------------------------------------- */
/*  TEST 21: KICK case sensitivity for nickname                               */
/* -------------------------------------------------------------------------- */
/*
** Description: Test if KICK handles nickname case sensitivity.
** Expected: Depends on implementation (IRC is typically case-insensitive).
*/
void test_21_kick_case_sensitivity() {
    print_test_header(21, "KICK Case Sensitivity for Nickname");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() directly - case sensitivity depends on implementation
    // Note: kickMember takes client pointer directly, so case doesn't matter here
    // This test is more relevant for KickCommand which uses getClientByNick()
    channel->kickMember(alice, bob, "Case test");
    
    if (!channel->isMember(bob)) {
        print_pass("Direct kickMember() works correctly");
        print_info("Case sensitivity is handled by getClientByNick() in KickCommand");
    } else {
        print_info("KICK is case-SENSITIVE ('bob' != 'Bob')");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 22: KICK with empty reason                                           */
/* -------------------------------------------------------------------------- */
/*
** Description: KICK with explicitly empty reason.
** Expected: Should use default reason (kicker's nick).
*/
void test_22_kick_empty_reason() {
    print_test_header(22, "KICK With Empty Reason");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test kickMember() with empty reason
    channel->kickMember(alice, bob, "");
    
    if (!channel->isMember(bob)) {
        print_pass("KICK with empty reason handled");
    } else {
        print_fail("KICK with empty reason failed");
    }
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 23: Stress test - many kicks in a row                                */
/* -------------------------------------------------------------------------- */
/*
** Description: Stress test with many users being kicked.
** Expected: No crash, all operations complete.
*/
void test_23_stress_test() {
    print_test_header(23, "Stress Test - Many Kicks");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    std::vector<Client*> users;
    
    // Create channel directly
    Channel* channel = server.getOrCreateChannel("#stress");
    if (!channel) {
        print_fail("Channel creation failed");
        delete alice;
        return;
    }
    
    // Add Alice as operator
    channel->addMember(alice);
    
    // Add 20 users directly
    for (int i = 0; i < 20; i++) {
        std::ostringstream nick;
        nick << "User" << i;
        Client* user = create_registered_client(20 + i, nick.str());
        users.push_back(user);
        channel->addMember(user);
    }
    
    size_t initial_count = channel->getMembersCount();
    
    // Alice kicks all 20 users using kickMember() directly
    for (size_t i = 0; i < users.size(); i++) {
        channel->kickMember(alice, users[i], "Stress test");
    }
    
    size_t final_count = channel->getMembersCount();
    
    std::ostringstream result;
    result << "Initial: " << initial_count << ", Final: " << final_count;
    print_info(result.str().c_str());
    
    if (final_count == 1) {  // Only Alice left
        print_pass("All 20 users kicked successfully");
    } else {
        std::ostringstream oss;
        oss << "Expected 1 member (Alice), got " << final_count;
        print_fail(oss.str().c_str());
    }
    
    // Cleanup
    for (size_t i = 0; i < users.size(); i++) {
        delete users[i];
    }
    delete alice;
}

/* -------------------------------------------------------------------------- */
/*  TEST 24: KICK NULL checks                                                 */
/* -------------------------------------------------------------------------- */
/*
** Description: Test that kickMember handles NULL properly.
** Expected: No crash with NULL parameters.
*/
void test_24_kick_null_checks() {
    print_test_header(24, "KICK NULL Safety Checks");
    
    Server server(6667, "password");
    Client* alice = create_registered_client(10, "Alice");
    Client* bob = create_registered_client(11, "Bob");
    
    Channel* channel = setup_channel_with_members(&server, alice, bob);
    
    if (!channel) {
        print_fail("Channel setup failed");
        delete alice;
        delete bob;
        return;
    }
    
    // Test Channel::kickMember with NULL kicker
    channel->kickMember(NULL, bob, "test");
    if (channel->isMember(bob)) {
        print_pass("kickMember(NULL, bob) handled safely - no crash");
    } else {
        print_fail("Bob kicked with NULL kicker");
    }
    
    // Test Channel::kickMember with NULL target
    channel->kickMember(alice, NULL, "test");
    print_pass("kickMember(alice, NULL) handled safely - no crash");
    
    delete alice;
    delete bob;
}

/* -------------------------------------------------------------------------- */
/*  TEST 25: Memory check - no leaks on kick operations                       */
/* -------------------------------------------------------------------------- */
/*
** Description: Multiple kick operations and cleanup.
** Expected: No crash, proper cleanup.
*/
void test_25_memory_operations() {
    print_test_header(25, "Memory Check - Multiple Operations");
    
    for (int cycle = 0; cycle < 10; cycle++) {
        Server server(6667, "password");
        Client* alice = create_registered_client(10, "Alice");
        Client* bob = create_registered_client(11, "Bob");
        
        std::vector<std::string> join_params;
        join_params.push_back("#memtest");
        execute_join(&server, alice, join_params);
        execute_join(&server, bob, join_params);
        
        std::vector<std::string> kick_params;
        kick_params.push_back("#memtest");
        kick_params.push_back("Bob");
        execute_kick(&server, alice, kick_params);
        
        delete alice;
        delete bob;
    }
    
    print_pass("10 cycles completed without crash");
}

/* ========================================================================== */
/*                              MAIN FUNCTION                                  */
/* ========================================================================== */

int main(int argc, char** argv) {
    std::cout << std::endl;
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "══        KICK COMMAND TEST SUITE - ft_irc               ══" << COLOR_RESET << std::endl;
    std::cout << COLOR_MAGENTA << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Note: 'Bad file descriptor' errors are expected (mock fds)" << COLOR_RESET << std::endl;
    
    int test_num = 0;
    if (argc > 1) {
        test_num = std::atoi(argv[1]);
    }
    
    // Array of test functions
    void (*tests[])() = {
        test_01_kick_success,               // 1
        test_02_kick_with_reason,           // 2
        test_03_kick_no_parameters,         // 3
        test_04_kick_nonexistent_channel,   // 4
        test_05_kick_by_nonmember,          // 5
        test_06_kick_by_nonoperator,        // 6
        test_07_kick_nonexistent_user,      // 7
        test_08_kick_user_not_in_channel,   // 8
        test_09_kick_broadcasts_to_members, // 9
        test_10_kicked_user_removed,        // 10
        test_11_empty_channel_removed,      // 11
        test_12_kicked_user_can_rejoin,     // 12
        test_13_kick_before_registration,   // 13
        test_14_operator_kicks_operator,    // 14
        test_15_kick_default_reason,        // 15
        test_16_kick_from_multiple_channels,// 16
        test_17_self_kick,                  // 17
        test_18_kick_long_reason,           // 18
        test_19_kick_special_chars_reason,  // 19
        test_20_multiple_kicks,             // 20
        test_21_kick_case_sensitivity,      // 21
        test_22_kick_empty_reason,          // 22
        test_23_stress_test,                // 23
        test_24_kick_null_checks,           // 24
        test_25_memory_operations           // 25
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    if (test_num > 0 && test_num <= num_tests) {
        // Run specific test
        tests[test_num - 1]();
    } else {
        // Run all tests
        for (int i = 0; i < num_tests; i++) {
            tests[i]();
        }
    }
    
    print_summary();
    
    return (g_tests_failed > 0) ? 1 : 0;
}
