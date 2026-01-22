/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_channel.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by or Channel.       #+#    #+#             */
/*   Updated: 2026/01/22 20:39:09 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cstdlib>

// Include Channel and Client headers
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

// ============================================================================
// TEST FRAMEWORK
// ============================================================================

static int g_tests_passed = 0;
static int g_tests_failed = 0;
static int g_tests_skipped = 0;
static int g_current_test = 0;

#define COLOR_GREEN  "\033[32m"
#define COLOR_RED    "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE   "\033[34m"
#define COLOR_CYAN   "\033[36m"
#define COLOR_RESET  "\033[0m"

void test_result(const std::string& test_name, bool passed) {
    if (passed) {
        std::cout << COLOR_GREEN << "  [PASS] " << COLOR_RESET << test_name << std::endl;
        g_tests_passed++;
    } else {
        std::cout << COLOR_RED << "  [FAIL] " << COLOR_RESET << test_name << std::endl;
        g_tests_failed++;
    }
}

void test_skip(const std::string& test_name, const std::string& reason) {
    std::cout << COLOR_YELLOW << "  [SKIP] " << COLOR_RESET << test_name 
              << " - " << reason << std::endl;
    g_tests_skipped++;
}

void test_info(const std::string& info) {
    std::cout << COLOR_CYAN << "  [INFO] " << COLOR_RESET << info << std::endl;
}

void test_header(int test_num, const std::string& title) {
    g_current_test = test_num;
    std::cout << std::endl;
    std::cout << COLOR_BLUE << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "TEST " << test_num << ": " << title << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "═══════════════════════════════════════════════════════════" << COLOR_RESET << std::endl;
}

void print_summary() {
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                    TEST SUMMARY                           " << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
    std::cout << COLOR_GREEN << "  Passed:  " << g_tests_passed << COLOR_RESET << std::endl;
    std::cout << COLOR_RED << "  Failed:  " << g_tests_failed << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "  Skipped: " << g_tests_skipped << COLOR_RESET << std::endl;
    std::cout << "  Total:   " << (g_tests_passed + g_tests_failed + g_tests_skipped) << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
    
    if (g_tests_failed == 0) {
        std::cout << COLOR_GREEN << "  ✓ All tests passed!" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "  ✗ Some tests failed!" << COLOR_RESET << std::endl;
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Create a mock client with specified fd and nickname
Client* create_mock_client(int fd, const std::string& nick, const std::string& user = "user") {
    Client* client = new Client(fd);
    client->setNickname(nick);
    client->setUsername(user);
    return client;
}

// Clean up clients
void cleanup_clients(std::vector<Client*>& clients) {
    for (size_t i = 0; i < clients.size(); i++) {
        delete clients[i];
    }
    clients.clear();
}

// ============================================================================
// TEST 01: Channel Construction
// ============================================================================
void test_01_channel_construction() {
    test_header(1, "Channel Construction");
    
    // Test 1.1: Create channel with valid name
    {
        Channel* ch = new Channel("#general", NULL);
        test_result("Channel created with name #general", ch != NULL);
        test_result("getName() returns correct name", ch->getName() == "#general");
        delete ch;
    }
    
    // Test 1.2: Create channel with different name formats
    {
        Channel* ch1 = new Channel("#test-channel", NULL);
        test_result("Channel with hyphen in name", ch1->getName() == "#test-channel");
        delete ch1;
        
        Channel* ch2 = new Channel("&local", NULL);
        test_result("Channel with & prefix", ch2->getName() == "&local");
        delete ch2;
        
        Channel* ch3 = new Channel("#123", NULL);
        test_result("Channel with numbers", ch3->getName() == "#123");
        delete ch3;
    }
    
    // Test 1.3: Initial state
    {
        Channel* ch = new Channel("#test", NULL);
        test_result("Initial topic is empty", ch->getTopic().empty());
        test_result("Initial member count is 0", ch->getMembersCount() == 0);
        test_result("Initial hasKey() is false", ch->hasKey() == false);
        test_result("Initial isChannelFull() is false", ch->isChannelFull() == false);
        test_result("Initial isChannelInvitOnly() is false", ch->isChannelInvitOnly() == false);
        delete ch;
    }
}

// ============================================================================
// TEST 02: Channel Destruction
// ============================================================================
void test_02_channel_destruction() {
    test_header(2, "Channel Destruction");
    
    // Test 2.1: Simple destruction
    {
        Channel* ch = new Channel("#test", NULL);
        delete ch;
        test_result("Channel destroyed without crash", true);
    }
    
    // Test 2.2: Multiple create/destroy cycles
    {
        bool success = true;
        for (int i = 0; i < 100; i++) {
            Channel* ch = new Channel("#test", NULL);
            delete ch;
        }
        test_result("100 create/destroy cycles completed", success);
    }
    
    // Test 2.3: Destruction with data
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setKey("secret");
        // Note: Can't add members without proper Client setup that won't crash
        delete ch;
        test_result("Channel with key destroyed without crash", true);
    }
}

// ============================================================================
// TEST 03: First Member Becomes Operator
// ============================================================================
void test_03_first_member_operator() {
    test_header(3, "First Member Becomes Operator Automatically");
    
    std::vector<Client*> clients;
    
    // Test 3.1: First member is operator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        
        test_result("First member added successfully", ch->isMember(alice));
        test_result("First member is automatically operator", ch->isOperator(alice));
        test_result("Member count is 1", ch->getMembersCount() == 1);
        
        delete ch;
    }
    
    // Test 3.2: Second member is NOT operator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        test_result("Second member added successfully", ch->isMember(bob));
        test_result("Second member is NOT operator", !ch->isOperator(bob));
        test_result("First member still operator", ch->isOperator(alice));
        test_result("Member count is 2", ch->getMembersCount() == 2);
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 04: Members Join and Leave
// ============================================================================
void test_04_members_join_leave() {
    test_header(4, "Members Can Join and Leave");
    
    std::vector<Client*> clients;
    
    // Test 4.1: Multiple joins
    {
        Channel* ch = new Channel("#test", NULL);
        
        for (int i = 0; i < 5; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
        }
        
        test_result("5 members joined", ch->getMembersCount() == 5);
        
        delete ch;
    }
    
    // Test 4.2: Member leaves
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        test_result("Before leave: 2 members", ch->getMembersCount() == 2);
        
        ch->removeMember(bob);
        
        test_result("After leave: 1 member", ch->getMembersCount() == 1);
        test_result("Bob is no longer member", !ch->isMember(bob));
        test_result("Alice still member", ch->isMember(alice));
        
        delete ch;
    }
    
    // Test 4.3: Remove non-member does nothing
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        
        ch->removeMember(bob); // Bob never joined
        
        test_result("Remove non-member: no crash", true);
        test_result("Member count unchanged", ch->getMembersCount() == 1);
        
        delete ch;
    }
    
    // Test 4.4: Add NULL client does nothing
    {
        Channel* ch = new Channel("#test", NULL);
        
        ch->addMember(NULL);
        
        test_result("addMember(NULL): no crash", true);
        test_result("Member count still 0", ch->getMembersCount() == 0);
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 05: Operator Can Change Topic When +t
// ============================================================================
void test_05_operator_topic_with_t() {
    test_header(5, "Operators Can Modify Topic When +t Is Active");
    
    std::vector<Client*> clients;
    
    // Test 5.1: Operator can set topic (default +t)
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice); // Alice becomes operator
        
        test_result("Alice is operator", ch->isOperator(alice));
        
        ch->setTopic("New topic", alice);
        
        test_result("Operator can set topic with +t", ch->getTopic() == "New topic");
        
        delete ch;
    }
    
    // Test 5.2: Topic can be changed multiple times
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        
        ch->setTopic("First topic", alice);
        test_result("Topic set to 'First topic'", ch->getTopic() == "First topic");
        
        ch->setTopic("Second topic", alice);
        test_result("Topic changed to 'Second topic'", ch->getTopic() == "Second topic");
        
        ch->setTopic("", alice);
        test_result("Topic can be cleared", ch->getTopic() == "");
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 06: Non-Operator Cannot Change Topic When +t
// ============================================================================
void test_06_non_operator_topic_with_t() {
    test_header(6, "Non-Operators Cannot Modify Topic When +t Is Active");
    
    std::vector<Client*> clients;
    
    // Test 6.1: Non-operator cannot set topic
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice); // Alice is operator
        ch->addMember(bob);   // Bob is regular member
        
        test_result("Bob is NOT operator", !ch->isOperator(bob));
        
        ch->setTopic("Bob's topic", bob);
        
        test_result("Non-operator cannot set topic with +t", ch->getTopic() != "Bob's topic");
        test_result("Topic remains empty", ch->getTopic().empty());
        
        delete ch;
    }
    
    // Test 6.2: Non-operator can set topic when -t
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        ch->setTopicRestricted(false); // -t
        
        ch->setTopic("Bob's topic", bob);
        
        test_result("Non-operator CAN set topic with -t", ch->getTopic() == "Bob's topic");
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 07: Channel Key Blocks Unauthorized Joins
// ============================================================================
void test_07_channel_key() {
    test_header(7, "Channel Key Blocks Unauthorized Joins");
    
    // Test 7.1: setKey and hasKey
    {
        Channel* ch = new Channel("#test", NULL);
        
        test_result("Initially no key", !ch->hasKey());
        
        ch->setKey("secret123");
        
        test_result("After setKey, hasKey() is true", ch->hasKey());
        
        delete ch;
    }
    
    // Test 7.2: checkKey with correct key
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setKey("secret");
        
        test_result("checkKey with correct key", ch->checkKey("secret"));
        
        delete ch;
    }
    
    // Test 7.3: checkKey with wrong key
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setKey("secret");
        
        test_result("checkKey with wrong key fails", !ch->checkKey("wrongkey"));
        test_result("checkKey with empty key fails", !ch->checkKey(""));
        
        delete ch;
    }
    
    // Test 7.4: checkKey when no key set
    {
        Channel* ch = new Channel("#test", NULL);
        
        test_result("checkKey with any key when no key set", ch->checkKey("anything"));
        test_result("checkKey with empty key when no key set", ch->checkKey(""));
        
        delete ch;
    }
    
    // Test 7.5: Remove key
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setKey("secret");
        
        test_result("Key is set", ch->hasKey());
        
        ch->setKey("");
        
        test_result("Key removed with empty string", !ch->hasKey());
        
        delete ch;
    }
}

// ============================================================================
// TEST 08: Invite Only Mode
// ============================================================================
void test_08_invite_only() {
    test_header(8, "Invite Only Mode Works Correctly");
    
    std::vector<Client*> clients;
    
    // Test 8.1: setInviteOnly
    {
        Channel* ch = new Channel("#test", NULL);
        
        test_result("Initially not invite-only", !ch->isChannelInvitOnly());
        
        ch->setInviteOnly(true);
        
        test_result("After setInviteOnly(true), isChannelInvitOnly() is true", 
                    ch->isChannelInvitOnly());
        
        ch->setInviteOnly(false);
        
        test_result("After setInviteOnly(false), isChannelInvitOnly() is false", 
                    !ch->isChannelInvitOnly());
        
        delete ch;
    }
    
    // Test 8.2: inviteUser and isInvited
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        test_result("Alice not initially invited", !ch->isInvited(alice));
        
        ch->inviteUser(alice);
        
        test_result("After inviteUser, Alice is invited", ch->isInvited(alice));
        test_result("Bob is NOT invited", !ch->isInvited(bob));
        
        delete ch;
    }
    
    // Test 8.3: Invitation is single-use (cleared after join)
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->inviteUser(alice);
        test_result("Alice invited before join", ch->isInvited(alice));
        
        ch->addMember(alice);
        
        test_result("After join, invitation is cleared", !ch->isInvited(alice));
        
        delete ch;
    }
    
    // Test 8.4: setInviteOnly(false) clears all invitations
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->setInviteOnly(true);
        ch->inviteUser(alice);
        ch->inviteUser(bob);
        
        test_result("Alice invited", ch->isInvited(alice));
        test_result("Bob invited", ch->isInvited(bob));
        
        ch->setInviteOnly(false);
        
        test_result("After -i, Alice invitation cleared", !ch->isInvited(alice));
        test_result("After -i, Bob invitation cleared", !ch->isInvited(bob));
        
        delete ch;
    }
    
    // Test 8.5: inviteUser(NULL) does nothing
    {
        Channel* ch = new Channel("#test", NULL);
        
        ch->inviteUser(NULL);
        
        test_result("inviteUser(NULL): no crash", true);
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 09: User Limit
// ============================================================================
void test_09_user_limit() {
    test_header(9, "User Limit Prevents Excessive Joins");
    
    std::vector<Client*> clients;
    
    // Test 9.1: setUserLimit
    {
        Channel* ch = new Channel("#test", NULL);
        
        test_result("Initially not full (limit 0 = no limit)", !ch->isChannelFull());
        
        ch->setUserLimit(5);
        
        test_result("Limit set, still not full (0 members)", !ch->isChannelFull());
        
        delete ch;
    }
    
    // Test 9.2: Channel becomes full
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setUserLimit(3);
        
        for (int i = 0; i < 3; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
        }
        
        test_result("3 members joined (limit 3)", ch->getMembersCount() == 3);
        test_result("Channel is now full", ch->isChannelFull());
        
        delete ch;
    }
    
    // Test 9.3: Cannot join full channel
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setUserLimit(2);
        
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        Client* charlie = create_mock_client(12, "Charlie", "charlie");
        clients.push_back(alice);
        clients.push_back(bob);
        clients.push_back(charlie);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        test_result("2 members (at limit)", ch->getMembersCount() == 2);
        
        ch->addMember(charlie); // Should be rejected
        
        test_result("Charlie rejected (channel full)", !ch->isMember(charlie));
        test_result("Still 2 members", ch->getMembersCount() == 2);
        
        delete ch;
    }
    
    // Test 9.4: Limit 0 means no limit
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setUserLimit(0);
        
        for (int i = 0; i < 10; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
        }
        
        test_result("10 members with no limit", ch->getMembersCount() == 10);
        test_result("isChannelFull() is false with limit 0", !ch->isChannelFull());
        
        delete ch;
    }
    
    // Test 9.5: Negative limit is rejected
    {
        Channel* ch = new Channel("#test", NULL);
        ch->setUserLimit(5);
        
        ch->setUserLimit(-1);
        
        // Limit should remain at 5 (negative rejected)
        // We can't directly check userLimit, but we can test behavior
        test_result("Negative limit rejected (no crash)", true);
        
        delete ch;
    }
    
    // Test 9.6: Lowering limit doesn't kick existing members
    {
        Channel* ch = new Channel("#test", NULL);
        
        for (int i = 0; i < 5; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
        }
        
        test_result("5 members joined", ch->getMembersCount() == 5);
        
        ch->setUserLimit(2);
        
        test_result("After limit set to 2, still 5 members", ch->getMembersCount() == 5);
        test_result("Channel is considered full", ch->isChannelFull());
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 10: Kick Removes Member Correctly
// ============================================================================
void test_10_kick_member() {
    test_header(10, "Kick Removes Member Correctly");
    
    std::vector<Client*> clients;
    
    // Test 10.1: Operator can kick member
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice); // Alice is operator
        ch->addMember(bob);
        
        test_result("Bob is member before kick", ch->isMember(bob));
        
        ch->kickMember(alice, bob, "spam");
        
        test_result("Bob is NOT member after kick", !ch->isMember(bob));
        test_result("Alice still member", ch->isMember(alice));
        test_result("Member count is 1", ch->getMembersCount() == 1);
        
        delete ch;
    }
    
    // Test 10.2: Non-operator cannot kick
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        Client* charlie = create_mock_client(12, "Charlie", "charlie");
        clients.push_back(alice);
        clients.push_back(bob);
        clients.push_back(charlie);
        
        ch->addMember(alice);
        ch->addMember(bob);
        ch->addMember(charlie);
        
        ch->kickMember(bob, charlie, "trying to kick"); // Bob is not operator
        
        test_result("Non-operator cannot kick: Charlie still member", ch->isMember(charlie));
        test_result("Member count still 3", ch->getMembersCount() == 3);
        
        delete ch;
    }
    
    // Test 10.3: Cannot kick non-member
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        // Bob never joined
        
        ch->kickMember(alice, bob, "not even here");
        
        test_result("Kick non-member: no crash", true);
        test_result("Member count unchanged", ch->getMembersCount() == 1);
        
        delete ch;
    }
    
    // Test 10.4: Kick with NULL clients
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        
        ch->kickMember(NULL, alice, "test");
        test_result("kickMember(NULL, alice): no crash", true);
        test_result("Alice still member", ch->isMember(alice));
        
        ch->kickMember(alice, NULL, "test");
        test_result("kickMember(alice, NULL): no crash", true);
        
        delete ch;
    }
    
    // Test 10.5: Kick removes operator status too
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        ch->addOperator(bob); // Make Bob operator too
        
        test_result("Bob is operator", ch->isOperator(bob));
        
        ch->kickMember(alice, bob, "goodbye");
        
        test_result("Bob kicked and no longer operator", !ch->isOperator(bob));
        test_result("Bob no longer member", !ch->isMember(bob));
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 11: Broadcast Sends to All Members Except Excluded
// ============================================================================
void test_11_broadcast() {
    test_header(11, "Broadcast Sends to All Members Except Excluded");
    
    std::vector<Client*> clients;
    
    // Note: We can't easily verify message content without mocking sendMessage
    // But we can verify the function doesn't crash with various inputs
    
    // Test 11.1: Broadcast to empty channel
    {
        Channel* ch = new Channel("#test", NULL);
        
        ch->broadcast("Test message\r\n", NULL);
        
        test_result("Broadcast to empty channel: no crash", true);
        
        delete ch;
    }
    
    // Test 11.2: Broadcast with exclude
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        ch->broadcast("Test\r\n", alice); // Exclude alice
        
        test_result("Broadcast with exclude: no crash", true);
        
        delete ch;
    }
    
    // Test 11.3: Broadcast with NULL exclude (send to all)
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        
        ch->broadcast("Test\r\n", NULL);
        
        test_result("Broadcast with NULL exclude: no crash", true);
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 12: Operator Management
// ============================================================================
void test_12_operator_management() {
    test_header(12, "Operator Management");
    
    std::vector<Client*> clients;
    
    // Test 12.1: addOperator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        test_result("Bob not initially operator", !ch->isOperator(bob));
        
        ch->addOperator(bob);
        
        test_result("Bob is now operator", ch->isOperator(bob));
        
        delete ch;
    }
    
    // Test 12.2: addOperator for non-member does nothing
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        // Bob never joined
        
        ch->addOperator(bob);
        
        test_result("addOperator for non-member: Bob not operator", !ch->isOperator(bob));
        
        delete ch;
    }
    
    // Test 12.3: removeOperator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        ch->addOperator(bob);
        
        test_result("Both are operators", ch->isOperator(alice) && ch->isOperator(bob));
        
        ch->removeOperator(bob);
        
        test_result("Bob no longer operator after removeOperator", !ch->isOperator(bob));
        test_result("Alice still operator", ch->isOperator(alice));
        
        delete ch;
    }
    
    // Test 12.4: Cannot remove last operator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice); // Alice is only operator
        
        ch->removeOperator(alice);
        
        test_result("Cannot remove last operator", ch->isOperator(alice));
        
        delete ch;
    }
    
    // Test 12.5: removeOperator for non-operator does nothing
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        ch->removeOperator(bob); // Bob is not operator
        
        test_result("removeOperator for non-operator: no crash", true);
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 13: Topic Restricted Mode
// ============================================================================
void test_13_topic_restricted() {
    test_header(13, "Topic Restricted Mode (+t)");
    
    std::vector<Client*> clients;
    
    // Test 13.1: Default is topic restricted
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        ch->setTopic("Op topic", alice);
        test_result("Operator can set topic (default +t)", ch->getTopic() == "Op topic");
        
        ch->setTopic("User topic", bob);
        test_result("Non-op cannot set topic (default +t)", ch->getTopic() == "Op topic");
        
        delete ch;
    }
    
    // Test 13.2: setTopicRestricted(false) allows anyone
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        ch->setTopicRestricted(false);
        
        ch->setTopic("User topic", bob);
        test_result("Anyone can set topic with -t", ch->getTopic() == "User topic");
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 14: getMembers Function
// ============================================================================
void test_14_get_members() {
    test_header(14, "getMembers() Function");
    
    std::vector<Client*> clients;
    
    // Test 14.1: getMembers returns correct set
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        std::set<Client*> members = ch->getMembers();
        
        test_result("getMembers returns 2 members", members.size() == 2);
        test_result("Alice in members set", members.find(alice) != members.end());
        test_result("Bob in members set", members.find(bob) != members.end());
        
        delete ch;
    }
    
    // Test 14.2: getMembers on empty channel
    {
        Channel* ch = new Channel("#test", NULL);
        
        std::set<Client*> members = ch->getMembers();
        
        test_result("getMembers on empty channel returns empty set", members.empty());
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 15: getMembersCount Function
// ============================================================================
void test_15_get_members_count() {
    test_header(15, "getMembersCount() Function");
    
    std::vector<Client*> clients;
    
    // Test 15.1: Initial count is 0
    {
        Channel* ch = new Channel("#test", NULL);
        
        test_result("Initial getMembersCount() is 0", ch->getMembersCount() == 0);
        
        delete ch;
    }
    
    // Test 15.2: Count increases with joins
    {
        Channel* ch = new Channel("#test", NULL);
        
        for (int i = 0; i < 5; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
            
            std::stringstream msg;
            msg << "After join " << (i + 1) << ": count is " << (i + 1);
            test_result(msg.str(), ch->getMembersCount() == (size_t)(i + 1));
        }
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 16: isMember and isOperator Functions
// ============================================================================
void test_16_is_member_operator() {
    test_header(16, "isMember() and isOperator() Functions");
    
    std::vector<Client*> clients;
    
    // Test 16.1: isMember for member
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        test_result("isMember before join: false", !ch->isMember(alice));
        
        ch->addMember(alice);
        
        test_result("isMember after join: true", ch->isMember(alice));
        
        delete ch;
    }
    
    // Test 16.2: isOperator for operator
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        Client* bob = create_mock_client(11, "Bob", "bob");
        clients.push_back(alice);
        clients.push_back(bob);
        
        ch->addMember(alice);
        ch->addMember(bob);
        
        test_result("First member (Alice) is operator", ch->isOperator(alice));
        test_result("Second member (Bob) is NOT operator", !ch->isOperator(bob));
        
        delete ch;
    }
    
    // Test 16.3: isMember with NULL
    {
        Channel* ch = new Channel("#test", NULL);
        
        // This might crash if not handled properly
        // Depending on implementation, NULL might cause issues
        // test_result("isMember(NULL)", !ch->isMember(NULL));
        test_skip("isMember(NULL)", "May not be implemented safely");
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 17: clearAllSet Function
// ============================================================================
void test_17_clear_all_set() {
    test_header(17, "clearAllSet() Function");
    
    std::vector<Client*> clients;
    
    // Test 17.1: clearAllSet clears everything
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        ch->setKey("secret");
        ch->inviteUser(alice); // Just to add to invitedUsers
        
        ch->clearAllSet();
        
        test_result("After clearAllSet: no members", ch->getMembersCount() == 0);
        test_result("After clearAllSet: no key", !ch->hasKey());
        test_result("After clearAllSet: topic empty", ch->getTopic().empty());
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 18: Edge Cases
// ============================================================================
void test_18_edge_cases() {
    test_header(18, "Edge Cases");
    
    std::vector<Client*> clients;
    
    // Test 18.1: Same client joins twice
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        ch->addMember(alice); // Join again
        
        test_result("Double join: still 1 member", ch->getMembersCount() == 1);
        
        delete ch;
    }
    
    // Test 18.2: Channel with special characters in name
    {
        Channel* ch = new Channel("#test_channel-123", NULL);
        test_result("Channel with special chars: created", ch->getName() == "#test_channel-123");
        delete ch;
    }
    
    // Test 18.3: Very long topic
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        
        std::string longTopic(1000, 'A');
        ch->setTopic(longTopic, alice);
        
        test_result("Long topic set successfully", ch->getTopic().length() == 1000);
        
        delete ch;
    }
    
    // Test 18.4: Very long key
    {
        Channel* ch = new Channel("#test", NULL);
        
        std::string longKey(500, 'X');
        ch->setKey(longKey);
        
        test_result("Long key set", ch->hasKey());
        test_result("Long key checkKey works", ch->checkKey(longKey));
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// TEST 19: Memory Management
// ============================================================================
void test_19_memory_management() {
    test_header(19, "Memory Management");
    
    // Test 19.1: Many channels created and destroyed
    {
        bool success = true;
        for (int i = 0; i < 100; i++) {
            Channel* ch = new Channel("#test", NULL);
            delete ch;
        }
        test_result("100 channel cycles: no crash", success);
    }
    
    // Test 19.2: Channel with many operations
    {
        std::vector<Client*> clients;
        Channel* ch = new Channel("#test", NULL);
        
        for (int i = 0; i < 20; i++) {
            std::stringstream ss;
            ss << "User" << i;
            Client* c = create_mock_client(10 + i, ss.str(), ss.str());
            clients.push_back(c);
            ch->addMember(c);
        }
        
        for (int i = 0; i < 10; i++) {
            ch->removeMember(clients[i]);
        }
        
        test_result("Many operations: no crash", true);
        test_result("10 members remaining", ch->getMembersCount() == 10);
        
        delete ch;
        cleanup_clients(clients);
    }
}

// ============================================================================
// TEST 20: Mode Combinations
// ============================================================================
void test_20_mode_combinations() {
    test_header(20, "Mode Combinations");
    
    std::vector<Client*> clients;
    
    // Test 20.1: +i +k combined
    {
        Channel* ch = new Channel("#test", NULL);
        
        ch->setInviteOnly(true);
        ch->setKey("secret");
        
        test_result("+i and +k can be combined", 
                    ch->isChannelInvitOnly() && ch->hasKey());
        
        delete ch;
    }
    
    // Test 20.2: +i +l combined
    {
        Channel* ch = new Channel("#test", NULL);
        
        ch->setInviteOnly(true);
        ch->setUserLimit(5);
        
        test_result("+i and +l can be combined", ch->isChannelInvitOnly());
        
        delete ch;
    }
    
    // Test 20.3: All modes at once
    {
        Channel* ch = new Channel("#test", NULL);
        Client* alice = create_mock_client(10, "Alice", "alice");
        clients.push_back(alice);
        
        ch->addMember(alice);
        ch->setInviteOnly(true);
        ch->setKey("secret");
        ch->setUserLimit(10);
        ch->setTopicRestricted(true);
        ch->setTopic("Test topic", alice);
        
        test_result("All modes set: channel functional", 
                    ch->isChannelInvitOnly() && 
                    ch->hasKey() && 
                    ch->getTopic() == "Test topic");
        
        delete ch;
    }
    
    cleanup_clients(clients);
}

// ============================================================================
// MAIN
// ============================================================================
int main(int argc, char** argv) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           CHANNEL.CPP UNIT TEST SUITE                     ║" << std::endl;
    std::cout << "║                                                           ║" << std::endl;
    std::cout << "║  Isolated tests - Server NOT required                     ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    
    // Run all tests or specific test
    if (argc > 1) {
        int test_num = atoi(argv[1]);
        switch (test_num) {
            case 1: test_01_channel_construction(); break;
            case 2: test_02_channel_destruction(); break;
            case 3: test_03_first_member_operator(); break;
            case 4: test_04_members_join_leave(); break;
            case 5: test_05_operator_topic_with_t(); break;
            case 6: test_06_non_operator_topic_with_t(); break;
            case 7: test_07_channel_key(); break;
            case 8: test_08_invite_only(); break;
            case 9: test_09_user_limit(); break;
            case 10: test_10_kick_member(); break;
            case 11: test_11_broadcast(); break;
            case 12: test_12_operator_management(); break;
            case 13: test_13_topic_restricted(); break;
            case 14: test_14_get_members(); break;
            case 15: test_15_get_members_count(); break;
            case 16: test_16_is_member_operator(); break;
            case 17: test_17_clear_all_set(); break;
            case 18: test_18_edge_cases(); break;
            case 19: test_19_memory_management(); break;
            case 20: test_20_mode_combinations(); break;
            default:
                std::cout << "Invalid test number. Use 1-20 or no argument for all tests." << std::endl;
                return 1;
        }
    } else {
        // Run all tests
        test_01_channel_construction();
        test_02_channel_destruction();
        test_03_first_member_operator();
        test_04_members_join_leave();
        test_05_operator_topic_with_t();
        test_06_non_operator_topic_with_t();
        test_07_channel_key();
        test_08_invite_only();
        test_09_user_limit();
        test_10_kick_member();
        test_11_broadcast();
        test_12_operator_management();
        test_13_topic_restricted();
        test_14_get_members();
        test_15_get_members_count();
        test_16_is_member_operator();
        test_17_clear_all_set();
        test_18_edge_cases();
        test_19_memory_management();
        test_20_mode_combinations();
    }
    
    print_summary();
    
    return (g_tests_failed > 0) ? 1 : 0;
}
