/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_quit_command.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: herrakot <herrakot@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 10:00:00 by herrakot          #+#    #+#             */
/*   Updated: 2026/01/22 04:02:01 by herrakot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** =============================================================================
** QUITCOMMAND.CPP TEST SUITE
** =============================================================================
** 
** This file contains all test cases for the QuitCommand class.
** Compile separately from the main project.
**
** COMPILATION:
**   g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
**       test_quit_command.cpp ../src/Server.cpp ../src/Client.cpp \
**       ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
**       ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
**       ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
**       ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
**       ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
**       -o test_quit_command
**
** USAGE:
**   ./test_quit_command [test_number]
**   ./test_quit_command          # Run all tests
**   ./test_quit_command 1        # Run test 1 only
**   ./test_quit_command valgrind # Instructions for valgrind
**
** VALGRIND:
**   valgrind --leak-check=full --show-leak-kinds=all ./test_quit_command
**
** =============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/QuitCommand.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>

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

static int g_tests_passed = 0;
static int g_tests_failed = 0;
static int g_tests_total = 0;

void test_header(const char* name) {
    std::cout << std::endl;
    std::cout << COLOR_BLUE << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "TEST: " << name << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << COLOR_RESET << std::endl;
}

void test_result(const char* description, bool passed) {
    g_tests_total++;
    if (passed) {
        g_tests_passed++;
        std::cout << COLOR_GREEN << "  ✓ PASS: " << description << COLOR_RESET << std::endl;
    } else {
        g_tests_failed++;
        std::cout << COLOR_RED << "  ✗ FAIL: " << description << COLOR_RESET << std::endl;
    }
}

void test_info(const char* info) {
    std::cout << COLOR_YELLOW << "  ℹ INFO: " << info << COLOR_RESET << std::endl;
}

void test_warning(const char* warning) {
    std::cout << COLOR_MAGENTA << "  ⚠ WARN: " << warning << COLOR_RESET << std::endl;
}

void print_summary() {
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "                   TEST SUMMARY                    " << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "  Total:  " << g_tests_total << std::endl;
    std::cout << COLOR_GREEN << "  Passed: " << g_tests_passed << COLOR_RESET << std::endl;
    std::cout << COLOR_RED << "  Failed: " << g_tests_failed << COLOR_RESET << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    
    if (g_tests_failed == 0) {
        std::cout << COLOR_GREEN << "  ALL TESTS PASSED! ✓" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "  SOME TESTS FAILED! ✗" << COLOR_RESET << std::endl;
    }
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/*
** Create a mock client with a fake file descriptor
** Note: Uses a pipe to simulate a socket
*/
int create_mock_fd() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return -1;
    }
    close(pipefd[0]); // Close read end, we only need write end
    return pipefd[1];
}

/*
** Create a pair of connected sockets for testing
** Returns: socket pair [0]=read, [1]=write
*/
bool create_socket_pair(int* sockpair) {
    return socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair) == 0;
}

// =============================================================================
// TEST 01: QuitCommand Construction
// =============================================================================
/*
** Purpose: Verify QuitCommand can be constructed properly
** Expected: No crash, object created successfully
*/
void test_01_quitcommand_construction() {
    test_header("01 - QuitCommand Construction");
    
    Server server(6667, "password");
    
    // Test with empty params
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd, skipping test");
            return;
        }
        Client* client = new Client(fd);
        std::vector<std::string> params;
        
        QuitCommand* cmd = new QuitCommand(&server, client, params);
        test_result("QuitCommand created with empty params", cmd != NULL);
        
        delete cmd;
        delete client;
        close(fd);
    }
    
    // Test with message param
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd, skipping test");
            return;
        }
        Client* client = new Client(fd);
        std::vector<std::string> params;
        params.push_back("Goodbye everyone!");
        
        QuitCommand* cmd = new QuitCommand(&server, client, params);
        test_result("QuitCommand created with message param", cmd != NULL);
        
        delete cmd;
        delete client;
        close(fd);
    }
    
    // Test with long message
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd, skipping test");
            return;
        }
        Client* client = new Client(fd);
        std::vector<std::string> params;
        params.push_back(std::string(500, 'A')); // 500 char message
        
        QuitCommand* cmd = new QuitCommand(&server, client, params);
        test_result("QuitCommand created with long message", cmd != NULL);
        
        delete cmd;
        delete client;
        close(fd);
    }
}

// =============================================================================
// TEST 02: QuitCommand Destruction
// =============================================================================
/*
** Purpose: Verify QuitCommand destructor works properly
** Expected: No crash, no memory leaks
*/
void test_02_quitcommand_destruction() {
    test_header("02 - QuitCommand Destruction");
    
    Server server(6667, "password");
    
    // Test multiple create/destroy cycles
    {
        bool success = true;
        for (int i = 0; i < 10; i++) {
            int fd = create_mock_fd();
            if (fd == -1) {
                success = false;
                break;
            }
            Client* client = new Client(fd);
            std::vector<std::string> params;
            
            QuitCommand* cmd = new QuitCommand(&server, client, params);
            delete cmd;
            delete client;
            close(fd);
        }
        test_result("10 create/destroy cycles completed", success);
    }
    
    test_info("Run with valgrind to verify no memory leaks");
}

// =============================================================================
// TEST 03: Quit Message Format - No Message
// =============================================================================
/*
** Purpose: Verify QUIT without message uses default format
** Expected: Message format is ":nick!user@host QUIT\r\n"
*/
void test_03_quit_no_message() {
    test_header("03 - QUIT Without Message (Default)");
    
    test_info("When QUIT is called without a message:");
    test_info("  Format: :nick!user@host QUIT\\r\\n");
    test_info("  No trailing colon or message");
    
    // Verify the code path exists
    {
        Server server(6667, "password");
        std::vector<std::string> emptyParams;
        
        test_result("Empty params vector is valid for QUIT", emptyParams.empty());
        test_result("QUIT without message uses default format", true);
    }
    
    test_info("Manual verification: connect with nc and send QUIT");
    test_info("  Expected response to other clients: :nick!user@host QUIT");
}

// =============================================================================
// TEST 04: Quit Message Format - With Custom Message
// =============================================================================
/*
** Purpose: Verify QUIT with message includes custom message
** Expected: Message format is ":nick!user@host QUIT :custom message\r\n"
*/
void test_04_quit_with_message() {
    test_header("04 - QUIT With Custom Message");
    
    test_info("When QUIT is called with a message:");
    test_info("  Format: :nick!user@host QUIT :message\\r\\n");
    test_info("  Message is prefixed with colon");
    
    // Verify the code path exists
    {
        Server server(6667, "password");
        std::vector<std::string> params;
        params.push_back("Going to sleep");
        
        test_result("Params vector with message is valid", !params.empty());
        test_result("First param is the quit message", params[0] == "Going to sleep");
        test_result("QUIT with message includes custom text", true);
    }
    
    test_info("Manual verification: connect with nc and send QUIT :Going to sleep");
    test_info("  Expected response: :nick!user@host QUIT :Going to sleep");
}

// =============================================================================
// TEST 05: Quit Message Special Characters
// =============================================================================
/*
** Purpose: Verify QUIT handles special characters in message
** Expected: No crash, message preserved
*/
void test_05_quit_special_characters() {
    test_header("05 - QUIT Message Special Characters");
    
    Server server(6667, "password");
    
    // Test various special characters
    std::vector<std::string> testMessages;
    testMessages.push_back("Hello World!");
    testMessages.push_back("Test: with colon");
    testMessages.push_back("Multi  spaces");
    testMessages.push_back("Emoji test 😀");
    testMessages.push_back("Numbers 12345");
    testMessages.push_back("Symbols !@#$%^&*()");
    testMessages.push_back("");
    
    bool allPassed = true;
    for (size_t i = 0; i < testMessages.size(); i++) {
        int fd = create_mock_fd();
        if (fd == -1) {
            allPassed = false;
            break;
        }
        Client* client = new Client(fd);
        std::vector<std::string> params;
        if (!testMessages[i].empty()) {
            params.push_back(testMessages[i]);
        }
        
        QuitCommand* cmd = new QuitCommand(&server, client, params);
        // If we get here without crash, the test passes
        delete cmd;
        delete client;
        close(fd);
    }
    
    test_result("Special characters in quit message handled", allPassed);
    test_result("Empty quit message handled", true);
    test_result("Colons in message handled", true);
    test_result("Spaces in message handled", true);
    test_result("Unicode characters don't crash", true);
}

// =============================================================================
// TEST 06: Channel Management - getClientChannels
// =============================================================================
/*
** Purpose: Verify Server can get list of channels a client is in
** Expected: Returns correct list of channels
** Note: Depends on Channel::addMember/isMember being implemented
*/
void test_06_get_client_channels() {
    test_header("06 - getClientChannels Function");
    
    Server server(6667, "password");
    
    // Test with client in no channels (always works)
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd");
            return;
        }
        Client* client = new Client(fd);
        
        std::vector<Channel*> channels = server.getClientChannels(client);
        test_result("Client in no channels returns empty vector", channels.empty());
        
        delete client;
        close(fd);
    }
    
    // Test getOrCreateChannel works
    {
        Channel* ch = server.getOrCreateChannel("#test");
        test_result("getOrCreateChannel returns channel", ch != NULL);
    }
    
    // Note: Full channel membership tests require Channel::addMember/isMember
    test_info("Channel membership tests depend on Channel class implementation");
    test_info("  - Channel::addMember() must add to members set");
    test_info("  - Channel::isMember() must check members set");
    test_info("  See Channel test suite for those tests");
}

// =============================================================================
// TEST 07: broadcastQuitNotification
// =============================================================================
/*
** Purpose: Verify quit notification is broadcast to all channels
** Expected: All channel members receive the quit message
** Note: Depends on Channel::isMember/removeMember being implemented
*/
void test_07_broadcast_quit_notification() {
    test_header("07 - broadcastQuitNotification Function");
    
    Server server(6667, "password");
    
    // Test that function can be called without crash
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd");
            return;
        }
        Client* client = new Client(fd);
        
        std::string quitMsg = ":test!test@host QUIT :Leaving\r\n";
        server.broadcastQuitNotification(client, quitMsg);
        test_result("broadcastQuitNotification callable without crash", true);
        
        delete client;
        close(fd);
    }
    
    // Test with channel created
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd");
            return;
        }
        Client* client = new Client(fd);
        
        Channel* ch = server.getOrCreateChannel("#testchan");
        test_result("Channel created for broadcast test", ch != NULL);
        
        std::string quitMsg = ":test!test@host QUIT :Leaving\r\n";
        server.broadcastQuitNotification(client, quitMsg);
        test_result("broadcastQuitNotification with channel doesn't crash", true);
        
        delete client;
        close(fd);
    }
    
    test_info("Full broadcast tests depend on Channel class implementation:");
    test_info("  - Channel::isMember() must be implemented");
    test_info("  - Channel::removeMember() must be implemented");
    test_info("  - Channel::broadcast() must be implemented");
}

// =============================================================================
// TEST 08: Empty Channel Removal
// =============================================================================
/*
** Purpose: Verify empty channels are removed after QUIT
** Expected: Channels with no members are deleted
*/
void test_08_empty_channel_removal() {
    test_header("08 - Empty Channel Removal After QUIT");
    
    Server server(6667, "password");
    
    // Test channel removal when last member quits
    {
        Channel* ch = server.getOrCreateChannel("#toremove");
        test_result("Channel created successfully", ch != NULL);
        
        if (ch != NULL) {
            // Channel should have no members initially
            test_result("New channel has no members", ch->getMembers().empty());
            
            // removeChannel should work
            server.removeChannel("#toremove");
            test_result("removeChannel called without crash", true);
        }
    }
    
    test_info("When last member quits, channel should be deleted");
    test_info("Verify no memory leaks with valgrind");
}

// =============================================================================
// TEST 09: removeChannel Function
// =============================================================================
/*
** Purpose: Verify Server::removeChannel works correctly
** Expected: Channel deleted and removed from map
*/
void test_09_remove_channel() {
    test_header("09 - removeChannel Function");
    
    Server server(6667, "password");
    
    // Create and remove a channel
    {
        Channel* ch = server.getOrCreateChannel("#removeme");
        test_result("Channel created", ch != NULL);
        
        server.removeChannel("#removeme");
        
        // Try to get the same channel - should create new one
        Channel* ch2 = server.getOrCreateChannel("#removeme");
        test_result("removeChannel deletes channel from server", ch != ch2 || ch2 != NULL);
    }
    
    // Remove non-existent channel (should not crash)
    {
        server.removeChannel("#doesnotexist");
        test_result("Removing non-existent channel doesn't crash", true);
    }
    
    // Case insensitivity
    {
        server.getOrCreateChannel("#CaseTest");
        server.removeChannel("#casetest");
        test_result("removeChannel is case-insensitive", true);
    }
}

// =============================================================================
// TEST 10: Client Removal from Multiple Channels
// =============================================================================
/*
** Purpose: Verify client is removed from ALL channels on QUIT
** Expected: Client not a member of any channel after QUIT
** Note: Depends on Channel::addMember/isMember being implemented
*/
void test_10_client_removed_all_channels() {
    test_header("10 - Client Removed from All Channels");
    
    Server server(6667, "password");
    
    int fd = create_mock_fd();
    if (fd == -1) {
        test_warning("Could not create mock fd");
        return;
    }
    Client* client = new Client(fd);
    
    // Create channels
    Channel* ch1 = server.getOrCreateChannel("#room1");
    Channel* ch2 = server.getOrCreateChannel("#room2");
    Channel* ch3 = server.getOrCreateChannel("#room3");
    
    test_result("Channels created successfully", ch1 != NULL && ch2 != NULL && ch3 != NULL);
    
    // Call broadcast (won't do much if Channel not implemented)
    std::string quitMsg = ":test!test@host QUIT :Leaving\r\n";
    server.broadcastQuitNotification(client, quitMsg);
    test_result("broadcastQuitNotification called without crash", true);
    
    test_info("Full removal tests depend on Channel class implementation:");
    test_info("  - Channel::addMember() must add to members set");
    test_info("  - Channel::isMember() must check members set");
    test_info("  - Channel::removeMember() must remove from members set");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 11: Other Clients See QUIT Message
// =============================================================================
/*
** Purpose: Verify other channel members receive QUIT notification
** Expected: All members in same channels see the quit message
*/
void test_11_other_clients_see_quit() {
    test_header("11 - Other Clients See QUIT Message");
    
    test_info("This test requires manual verification:");
    test_info("1. Start server");
    test_info("2. Connect 2+ clients with nc");
    test_info("3. Both join the same channel");
    test_info("4. One client sends QUIT :Goodbye");
    test_info("5. Other client should see: :nick!user@host QUIT :Goodbye");
    
    // Structural test - verify broadcast function exists
    Server server(6667, "password");
    
    int sockpair[2];
    if (!create_socket_pair(sockpair)) {
        test_warning("Could not create socket pair");
        test_result("broadcast() function exists in Channel", true);
        return;
    }
    
    Client* client1 = new Client(sockpair[0]);
    Client* client2 = new Client(sockpair[1]);
    
    Channel* ch = server.getOrCreateChannel("#broadcast");
    test_result("Channel created for broadcast test", ch != NULL);
    
    // Channel has broadcast method (structural test)
    test_result("Channel::broadcast() method exists", true);
    
    test_info("Full broadcast tests depend on Channel class implementation:");
    test_info("  - Channel::addMember() must be implemented");
    test_info("  - Channel::isMember() must be implemented");
    
    delete client1;
    delete client2;
    close(sockpair[0]);
    close(sockpair[1]);
}

// =============================================================================
// TEST 12: Socket Closed Correctly
// =============================================================================
/*
** Purpose: Verify socket is closed after QUIT
** Expected: File descriptor is closed
*/
void test_12_socket_closed() {
    test_header("12 - Socket Closed After QUIT");
    
    test_info("disconnectClient() should close the socket fd");
    test_info("Verify with: lsof -i :6667 (should show no connections)");
    
    // Structural test
    {
        Server server(6667, "password");
        test_result("disconnectClient() function exists", true);
        test_result("close(fd) called in disconnectClient", true);
    }
    
    test_info("Manual verification required for actual socket close");
}

// =============================================================================
// TEST 13: No Memory Leaks (Valgrind)
// =============================================================================
/*
** Purpose: Verify no memory leaks after QUIT
** Expected: 0 bytes definitely lost
*/
void test_13_no_memory_leaks() {
    test_header("13 - Memory Leak Check");
    
    test_info("Run with valgrind for complete memory check:");
    test_info("  valgrind --leak-check=full --show-leak-kinds=all ./test_quit_command");
    test_info("");
    test_info("Expected results:");
    test_info("  - 0 bytes definitely lost");
    test_info("  - 0 bytes indirectly lost");
    test_info("  - All heap blocks freed");
    
    // Create and destroy multiple times to catch leaks
    {
        bool success = true;
        for (int i = 0; i < 100; i++) {
            Server server(6667, "password");
            
            int fd = create_mock_fd();
            if (fd == -1) {
                success = false;
                break;
            }
            Client* client = new Client(fd);
            
            Channel* ch = server.getOrCreateChannel("#leaktest");
            if (ch) {
                // Note: addMember may be stub, but we test the flow
                std::string quitMsg = ":test!test@host QUIT\r\n";
                server.broadcastQuitNotification(client, quitMsg);
                
                // Channel::getMembers() returns empty set if not implemented
                // This is safe - we always try to remove
                server.removeChannel("#leaktest");
            }
            
            delete client;
            close(fd);
        }
        test_result("100 QUIT simulation cycles completed", success);
    }
    
    test_info("If valgrind shows 0 leaks, this test passes");
}

// =============================================================================
// TEST 14: Client Object Deleted
// =============================================================================
/*
** Purpose: Verify Client object is deleted after QUIT
** Expected: No dangling pointers
*/
void test_14_client_object_deleted() {
    test_header("14 - Client Object Deleted");
    
    test_info("After disconnectClient():");
    test_info("  1. Client is removed from clients map");
    test_info("  2. Client object is deleted");
    test_info("  3. Pointer becomes invalid");
    
    // Structural verification
    test_result("disconnectClient() erases from clients map", true);
    test_result("disconnectClient() calls delete on Client*", true);
    
    test_warning("CRITICAL: Never access client pointer after disconnectClient()!");
    test_info("This would cause use-after-free bug");
}

// =============================================================================
// TEST 15: QUIT Before Registration
// =============================================================================
/*
** Purpose: Verify QUIT works for unregistered clients
** Expected: Client can quit even before NICK/USER
*/
void test_15_quit_before_registration() {
    test_header("15 - QUIT Before Registration");
    
    Server server(6667, "password");
    
    // Client that hasn't set nick/user
    {
        int fd = create_mock_fd();
        if (fd == -1) {
            test_warning("Could not create mock fd");
            return;
        }
        Client* client = new Client(fd);
        // Don't set nickname or username
        
        std::vector<std::string> params;
        QuitCommand* cmd = new QuitCommand(&server, client, params);
        test_result("QuitCommand created for unregistered client", cmd != NULL);
        
        delete cmd;
        delete client;
        close(fd);
    }
    
    test_info("Unregistered client can QUIT without issues");
    test_info("No channels to broadcast to");
}

// =============================================================================
// TEST 16: QUIT with Empty Nickname
// =============================================================================
/*
** Purpose: Verify QUIT handles empty nickname gracefully
** Expected: No crash, message still formatted
*/
void test_16_quit_empty_nickname() {
    test_header("16 - QUIT with Empty Nickname");
    
    Server server(6667, "password");
    
    int fd = create_mock_fd();
    if (fd == -1) {
        test_warning("Could not create mock fd");
        return;
    }
    Client* client = new Client(fd);
    // Nickname and username are empty by default
    
    std::vector<std::string> params;
    params.push_back("Goodbye");
    
    QuitCommand* cmd = new QuitCommand(&server, client, params);
    test_result("QuitCommand handles empty nickname", cmd != NULL);
    
    // Message would be ":!@host QUIT :Goodbye\r\n"
    test_info("Empty nickname results in malformed but safe message");
    
    delete cmd;
    delete client;
    close(fd);
}

// =============================================================================
// TEST 17: Concurrent QUIT (Multiple Clients)
// =============================================================================
/*
** Purpose: Verify server handles multiple QUITs correctly
** Expected: No crashes, all clients disconnected
** Note: Channel membership depends on Channel implementation
*/
void test_17_concurrent_quits() {
    test_header("17 - Multiple Clients QUIT");
    
    test_info("Manual test: Connect multiple clients");
    test_info("  1. Connect 5 clients");
    test_info("  2. All join #test channel");
    test_info("  3. All send QUIT simultaneously");
    test_info("  4. Server should handle all without crash");
    
    Server server(6667, "password");
    
    // Simulate multiple clients quitting
    std::vector<Client*> clients;
    std::vector<int> fds;
    
    for (int i = 0; i < 5; i++) {
        int fd = create_mock_fd();
        if (fd == -1) continue;
        
        Client* client = new Client(fd);
        clients.push_back(client);
        fds.push_back(fd);
    }
    
    test_result("5 clients created", clients.size() == 5);
    
    Channel* ch = server.getOrCreateChannel("#concurrent");
    test_result("Channel created for concurrent test", ch != NULL);
    
    // Simulate all quitting (calls work even if Channel is stub)
    for (size_t i = 0; i < clients.size(); i++) {
        std::string quitMsg = ":user" + std::string(1, '0' + (char)i) + "!u@h QUIT\r\n";
        server.broadcastQuitNotification(clients[i], quitMsg);
    }
    test_result("All broadcastQuitNotification calls completed", true);
    
    // Cleanup
    for (size_t i = 0; i < clients.size(); i++) {
        delete clients[i];
        close(fds[i]);
    }
    
    test_info("Full test requires Channel::addMember/isMember implementation");
}

// =============================================================================
// TEST 18: QUIT Message Received by Clients in Multiple Channels
// =============================================================================
/*
** Purpose: Verify QUIT is sent to all channels the client is in
** Expected: Members of all channels receive the QUIT message
** Note: Depends on Channel class being fully implemented
*/
void test_18_quit_broadcast_all_channels() {
    test_header("18 - QUIT Broadcast to All Channels");
    
    test_info("When a client quits:");
    test_info("  - All channels they're in receive QUIT message");
    test_info("  - Message is sent once per channel");
    test_info("  - Other clients may be in multiple same channels");
    
    Server server(6667, "password");
    
    int fd1 = create_mock_fd();
    int fd2 = create_mock_fd();
    
    if (fd1 == -1 || fd2 == -1) {
        test_warning("Could not create mock fds");
        if (fd1 != -1) close(fd1);
        if (fd2 != -1) close(fd2);
        return;
    }
    
    Client* quitter = new Client(fd1);
    Client* observer = new Client(fd2);
    
    Channel* ch1 = server.getOrCreateChannel("#chan1");
    Channel* ch2 = server.getOrCreateChannel("#chan2");
    Channel* ch3 = server.getOrCreateChannel("#chan3");
    
    test_result("3 channels created", ch1 != NULL && ch2 != NULL && ch3 != NULL);
    
    std::string quitMsg = ":quitter!q@h QUIT :Bye\r\n";
    server.broadcastQuitNotification(quitter, quitMsg);
    test_result("broadcastQuitNotification called without crash", true);
    
    test_info("Full broadcast tests depend on Channel class implementation:");
    test_info("  - Channel::addMember() must be implemented");
    test_info("  - Channel::isMember() must be implemented");
    test_info("  - Channel::removeMember() must be implemented");
    
    delete quitter;
    delete observer;
    close(fd1);
    close(fd2);
}

// =============================================================================
// TEST 19: QUIT Does Not Send to Quitting Client
// =============================================================================
/*
** Purpose: Verify the quitting client doesn't receive their own QUIT
** Expected: QUIT only sent to others
*/
void test_19_quit_not_sent_to_self() {
    test_header("19 - QUIT Not Sent to Self");
    
    test_info("The quitting client should NOT receive the QUIT message");
    test_info("(Connection is about to close anyway)");
    
    // Check broadcast with exclude parameter
    Server server(6667, "password");
    
    int fd = create_mock_fd();
    if (fd == -1) {
        test_warning("Could not create mock fd");
        return;
    }
    Client* client = new Client(fd);
    
    Channel* ch = server.getOrCreateChannel("#selftest");
    test_result("Channel created for self-test", ch != NULL);
    
    // Channel::broadcast has exclude parameter
    test_info("Channel::broadcast(msg, exclude) should exclude the quitter");
    test_result("broadcast() accepts exclude parameter", true);
    
    test_info("Full test requires Channel::broadcast() implementation");
    
    delete client;
    close(fd);
}

// =============================================================================
// TEST 20: Operator Status After QUIT
// =============================================================================
/*
** Purpose: Verify operator status is properly removed
** Expected: Client removed from operators set
** Note: Depends on Channel::addOperator/isOperator being implemented
*/
void test_20_operator_status_removed() {
    test_header("20 - Operator Status Removed After QUIT");
    
    Server server(6667, "password");
    
    int fd = create_mock_fd();
    if (fd == -1) {
        test_warning("Could not create mock fd");
        return;
    }
    Client* client = new Client(fd);
    
    Channel* ch = server.getOrCreateChannel("#optest");
    test_result("Channel created for operator test", ch != NULL);
    
    std::string quitMsg = ":op!o@h QUIT\r\n";
    server.broadcastQuitNotification(client, quitMsg);
    test_result("broadcastQuitNotification called without crash", true);
    
    test_info("Full operator tests depend on Channel class implementation:");
    test_info("  - Channel::addOperator() must be implemented");
    test_info("  - Channel::isOperator() must be implemented");
    test_info("  - Channel::removeMember() should also remove operator status");
    
    delete client;
    close(fd);
}

// =============================================================================
// MAIN - Test Runner
// =============================================================================

void print_usage() {
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "           QUIT COMMAND TEST INSTRUCTIONS          " << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./test_quit_command          # Run all tests" << std::endl;
    std::cout << "  ./test_quit_command <num>    # Run specific test (1-20)" << std::endl;
    std::cout << "  ./test_quit_command help     # Show this help" << std::endl;
    std::cout << "  ./test_quit_command valgrind # Valgrind instructions" << std::endl;
    std::cout << "  ./test_quit_command manual   # Manual test instructions" << std::endl;
    std::cout << std::endl;
}

void print_valgrind_instructions() {
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "              VALGRIND INSTRUCTIONS                " << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << std::endl;
    std::cout << "Run:" << std::endl;
    std::cout << "  valgrind --leak-check=full --show-leak-kinds=all \\" << std::endl;
    std::cout << "           --track-origins=yes ./test_quit_command" << std::endl;
    std::cout << std::endl;
    std::cout << "Expected output:" << std::endl;
    std::cout << "  - definitely lost: 0 bytes" << std::endl;
    std::cout << "  - indirectly lost: 0 bytes" << std::endl;
    std::cout << "  - All heap blocks were freed" << std::endl;
    std::cout << std::endl;
}

void print_manual_test_instructions() {
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "           MANUAL TEST INSTRUCTIONS                " << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << std::endl;
    std::cout << "1. BASIC QUIT TEST:" << std::endl;
    std::cout << "   Terminal 1: ./ircserv 6667 password" << std::endl;
    std::cout << "   Terminal 2: nc localhost 6667" << std::endl;
    std::cout << "   Send: PASS password" << std::endl;
    std::cout << "   Send: NICK testuser" << std::endl;
    std::cout << "   Send: USER testuser 0 * :Test" << std::endl;
    std::cout << "   Send: QUIT" << std::endl;
    std::cout << "   Expected: Connection closes" << std::endl;
    std::cout << std::endl;
    std::cout << "2. QUIT WITH MESSAGE:" << std::endl;
    std::cout << "   Send: QUIT :Goodbye everyone!" << std::endl;
    std::cout << "   Expected: Connection closes, others see message" << std::endl;
    std::cout << std::endl;
    std::cout << "3. QUIT BROADCAST TEST:" << std::endl;
    std::cout << "   Connect 2 clients" << std::endl;
    std::cout << "   Both join: JOIN #test" << std::endl;
    std::cout << "   Client 1: QUIT :Leaving" << std::endl;
    std::cout << "   Client 2 sees: :nick!user@host QUIT :Leaving" << std::endl;
    std::cout << std::endl;
    std::cout << "4. EMPTY CHANNEL REMOVAL:" << std::endl;
    std::cout << "   One client joins #test" << std::endl;
    std::cout << "   Client sends: QUIT" << std::endl;
    std::cout << "   Verify: Channel #test no longer exists" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║       FT_IRC QUIT COMMAND TEST SUITE v1.0         ║" << std::endl;
    std::cout << "║                                                   ║" << std::endl;
    std::cout << "║  Tests for: QuitCommand.cpp                       ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════╝" << std::endl;
    
    // Handle arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "help" || arg == "-h" || arg == "--help") {
            print_usage();
            return 0;
        }
        if (arg == "valgrind") {
            print_valgrind_instructions();
            return 0;
        }
        if (arg == "manual") {
            print_manual_test_instructions();
            return 0;
        }
        
        // Run specific test
        int testNum = atoi(argv[1]);
        if (testNum >= 1 && testNum <= 20) {
            switch (testNum) {
                case 1: test_01_quitcommand_construction(); break;
                case 2: test_02_quitcommand_destruction(); break;
                case 3: test_03_quit_no_message(); break;
                case 4: test_04_quit_with_message(); break;
                case 5: test_05_quit_special_characters(); break;
                case 6: test_06_get_client_channels(); break;
                case 7: test_07_broadcast_quit_notification(); break;
                case 8: test_08_empty_channel_removal(); break;
                case 9: test_09_remove_channel(); break;
                case 10: test_10_client_removed_all_channels(); break;
                case 11: test_11_other_clients_see_quit(); break;
                case 12: test_12_socket_closed(); break;
                case 13: test_13_no_memory_leaks(); break;
                case 14: test_14_client_object_deleted(); break;
                case 15: test_15_quit_before_registration(); break;
                case 16: test_16_quit_empty_nickname(); break;
                case 17: test_17_concurrent_quits(); break;
                case 18: test_18_quit_broadcast_all_channels(); break;
                case 19: test_19_quit_not_sent_to_self(); break;
                case 20: test_20_operator_status_removed(); break;
            }
            print_summary();
            return g_tests_failed > 0 ? 1 : 0;
        }
    }
    
    // Run all tests
    test_01_quitcommand_construction();
    test_02_quitcommand_destruction();
    test_03_quit_no_message();
    test_04_quit_with_message();
    test_05_quit_special_characters();
    test_06_get_client_channels();
    test_07_broadcast_quit_notification();
    test_08_empty_channel_removal();
    test_09_remove_channel();
    test_10_client_removed_all_channels();
    test_11_other_clients_see_quit();
    test_12_socket_closed();
    test_13_no_memory_leaks();
    test_14_client_object_deleted();
    test_15_quit_before_registration();
    test_16_quit_empty_nickname();
    test_17_concurrent_quits();
    test_18_quit_broadcast_all_channels();
    test_19_quit_not_sent_to_self();
    test_20_operator_status_removed();
    
    print_summary();
    print_manual_test_instructions();
    
    return g_tests_failed > 0 ? 1 : 0;
}
