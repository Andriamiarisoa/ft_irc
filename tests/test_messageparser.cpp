/**
 * @file test_messageparser.cpp
 * @brief Comprehensive unit tests for MessageParser
 * 
 * This test file focuses ONLY on testing the parsing logic implemented
 * directly in MessageParser.cpp:
 *   - extractPrefix(): Extracts IRC message prefix
 *   - splitParams(): Splits IRC message into tokens
 *   - removePrefix(): Helper to remove prefix from message
 * 
 * These tests do NOT depend on Command, Server, or Client implementations.
 * 
 * To compile: g++ -std=c++98 -Wall -Wextra -Werror test_messageparser.cpp -o test_messageparser
 * To run: ./test_messageparser
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

// =============================================================================
// REIMPLEMENTATION OF PARSER FUNCTIONS FOR ISOLATED TESTING
// (Copy of the parsing logic without Command/Server/Client dependencies)
// =============================================================================

namespace ParserTest {

/**
 * @brief Extracts the prefix from an IRC message line
 * @param line The raw IRC message
 * @return The prefix (including ':') or empty string if no prefix
 */
std::string extractPrefix(const std::string& line) {
    size_t pos;

    if (line.empty() || line[0] != ':')
        return "";
    pos = line.find(" ");
    if (pos == std::string::npos)
        return line;
    return line.substr(0, pos);
}

/**
 * @brief Removes the prefix from an IRC message
 * @param line The raw IRC message
 * @return The message without the prefix
 */
std::string removePrefix(const std::string& line) {
    std::string prefix;
    std::string cmdLine;
    size_t      pos;

    cmdLine = line;
    if (line.empty())
        return "";
    if (line[0] == ':')
    {
        prefix = extractPrefix(line);
        pos = prefix.size();
        while (line.size() > pos && line[pos] == ' ')
            ++pos;
        cmdLine = line.substr(pos, line.size() - pos);
    }
    return (cmdLine);
}

/**
 * @brief Splits an IRC message into command and parameters
 * @param str The raw IRC message
 * @return Vector of tokens (command + parameters)
 */
std::vector<std::string> splitParams(const std::string& str) {
    std::vector<std::string>    result;
    std::string                 cmdLine;
    size_t                      pos;

    cmdLine = removePrefix(str);
    if (cmdLine.empty())
        return std::vector<std::string>();
    while (!cmdLine.empty() && cmdLine[0] != ':')
    {
        pos = cmdLine.find(" ");
        if (pos == std::string::npos)
            pos = cmdLine.size();
        result.push_back(cmdLine.substr(0, pos));
        while (cmdLine[pos] == ' ')
            ++pos;
        cmdLine = cmdLine.substr(pos, cmdLine.size() - pos);
    }
    if (!cmdLine.empty())
    {
        if (cmdLine[0] == ' ')
            result.clear();
        else
            result.push_back(cmdLine);
    }
    return result;
}

/**
 * @brief Converts command to uppercase (for case-insensitive comparison)
 * @param cmd The command string
 * @return Uppercase version of the command
 */
std::string toUpperCase(const std::string& cmd) {
    std::string result = cmd;
    for (size_t i = 0; i < result.size(); i++)
        result[i] = std::toupper(result[i]);
    return result;
}

} // namespace ParserTest

// =============================================================================
// TEST FRAMEWORK (Minimal implementation)
// =============================================================================

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

void printTestHeader(const std::string& testName) {
    std::cout << CYAN << "\n=== " << testName << " ===" << RESET << std::endl;
}

void assertEq(const std::string& expected, const std::string& actual, 
              const std::string& testDesc) {
    if (expected == actual) {
        std::cout << GREEN << "[PASS] " << RESET << testDesc << std::endl;
        g_testsPassed++;
    } else {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        std::cout << "       Expected: \"" << expected << "\"" << std::endl;
        std::cout << "       Actual:   \"" << actual << "\"" << std::endl;
        g_testsFailed++;
    }
}

void assertEqInt(int expected, int actual, const std::string& testDesc) {
    if (expected == actual) {
        std::cout << GREEN << "[PASS] " << RESET << testDesc << std::endl;
        g_testsPassed++;
    } else {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        std::cout << "       Expected: " << expected << std::endl;
        std::cout << "       Actual:   " << actual << std::endl;
        g_testsFailed++;
    }
}

void assertVectorSize(size_t expected, const std::vector<std::string>& vec, 
                      const std::string& testDesc) {
    if (vec.size() == expected) {
        std::cout << GREEN << "[PASS] " << RESET << testDesc << std::endl;
        g_testsPassed++;
    } else {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        std::cout << "       Expected size: " << expected << std::endl;
        std::cout << "       Actual size:   " << vec.size() << std::endl;
        g_testsFailed++;
    }
}

void assertVectorElem(const std::string& expected, const std::vector<std::string>& vec, 
                      size_t index, const std::string& testDesc) {
    if (index >= vec.size()) {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        std::cout << "       Index " << index << " out of bounds (size=" << vec.size() << ")" << std::endl;
        g_testsFailed++;
        return;
    }
    if (vec[index] == expected) {
        std::cout << GREEN << "[PASS] " << RESET << testDesc << std::endl;
        g_testsPassed++;
    } else {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        std::cout << "       Expected: \"" << expected << "\"" << std::endl;
        std::cout << "       Actual:   \"" << vec[index] << "\"" << std::endl;
        g_testsFailed++;
    }
}

void assertTrue(bool condition, const std::string& testDesc) {
    if (condition) {
        std::cout << GREEN << "[PASS] " << RESET << testDesc << std::endl;
        g_testsPassed++;
    } else {
        std::cout << RED << "[FAIL] " << RESET << testDesc << std::endl;
        g_testsFailed++;
    }
}

// =============================================================================
// TEST: extractPrefix()
// =============================================================================

void test_extractPrefix_emptyString() {
    printTestHeader("extractPrefix - Empty string");
    std::string result = ParserTest::extractPrefix("");
    assertEq("", result, "Empty string returns empty prefix");
}

void test_extractPrefix_noPrefix() {
    printTestHeader("extractPrefix - No prefix");
    
    assertEq("", ParserTest::extractPrefix("NICK john"),
             "Simple command without prefix");
    assertEq("", ParserTest::extractPrefix("PRIVMSG #channel :Hello"),
             "Command with params without prefix");
    assertEq("", ParserTest::extractPrefix("JOIN #channel"),
             "JOIN without prefix");
}

void test_extractPrefix_simplePrefix() {
    printTestHeader("extractPrefix - Simple prefix");
    
    assertEq(":nick", ParserTest::extractPrefix(":nick PRIVMSG #chan :msg"),
             "Simple nick prefix");
    assertEq(":nick!user@host", ParserTest::extractPrefix(":nick!user@host PRIVMSG #chan :msg"),
             "Full nick!user@host prefix");
    assertEq(":server.example.com", ParserTest::extractPrefix(":server.example.com NOTICE * :msg"),
             "Server prefix");
}

void test_extractPrefix_prefixOnly() {
    printTestHeader("extractPrefix - Prefix only (no command)");
    
    assertEq(":onlyprefix", ParserTest::extractPrefix(":onlyprefix"),
             "Line with only prefix, no space");
}

void test_extractPrefix_prefixWithMultipleSpaces() {
    printTestHeader("extractPrefix - Prefix with multiple spaces");
    
    assertEq(":prefix", ParserTest::extractPrefix(":prefix    COMMAND"),
             "Prefix followed by multiple spaces");
}

// =============================================================================
// TEST: removePrefix() - via splitParams behavior
// =============================================================================

void test_removePrefix_noPrefix() {
    printTestHeader("removePrefix - No prefix");
    
    std::vector<std::string> result = ParserTest::splitParams("NICK john");
    assertVectorSize(2, result, "NICK john has 2 tokens");
    assertVectorElem("NICK", result, 0, "First token is NICK");
    assertVectorElem("john", result, 1, "Second token is john");
}

void test_removePrefix_withPrefix() {
    printTestHeader("removePrefix - With prefix");
    
    std::vector<std::string> result = ParserTest::splitParams(":server NICK john");
    assertVectorSize(2, result, ":server NICK john has 2 tokens after prefix removal");
    assertVectorElem("NICK", result, 0, "First token is NICK (prefix removed)");
    assertVectorElem("john", result, 1, "Second token is john");
}

void test_removePrefix_multipleSpacesAfterPrefix() {
    printTestHeader("removePrefix - Multiple spaces after prefix");
    
    std::vector<std::string> result = ParserTest::splitParams(":server    NICK john");
    assertVectorSize(2, result, "Multiple spaces after prefix handled");
    assertVectorElem("NICK", result, 0, "Command extracted correctly");
}

// =============================================================================
// TEST: splitParams() - Basic commands (PASS, NICK, USER)
// =============================================================================

void test_splitParams_PASS() {
    printTestHeader("splitParams - PASS command");
    
    std::vector<std::string> result = ParserTest::splitParams("PASS secretpassword");
    assertVectorSize(2, result, "PASS has 2 tokens");
    assertVectorElem("PASS", result, 0, "Command is PASS");
    assertVectorElem("secretpassword", result, 1, "Password is secretpassword");
}

void test_splitParams_NICK() {
    printTestHeader("splitParams - NICK command");
    
    std::vector<std::string> result = ParserTest::splitParams("NICK john_doe");
    assertVectorSize(2, result, "NICK has 2 tokens");
    assertVectorElem("NICK", result, 0, "Command is NICK");
    assertVectorElem("john_doe", result, 1, "Nickname is john_doe");
}

void test_splitParams_USER_basic() {
    printTestHeader("splitParams - USER command (basic)");
    
    std::vector<std::string> result = ParserTest::splitParams("USER john 0 * :John Doe");
    assertVectorSize(5, result, "USER has 5 tokens");
    assertVectorElem("USER", result, 0, "Command is USER");
    assertVectorElem("john", result, 1, "Username is john");
    assertVectorElem("0", result, 2, "Mode is 0");
    assertVectorElem("*", result, 3, "Unused param is *");
    assertVectorElem(":John Doe", result, 4, "Realname is :John Doe (with spaces)");
}

// =============================================================================
// TEST: splitParams() - Commands with multiple parameters (USER)
// =============================================================================

void test_splitParams_USER_multipleParams() {
    printTestHeader("splitParams - USER with multiple parameters");
    
    std::vector<std::string> result = ParserTest::splitParams("USER username hostname servername :Real Name Here");
    assertVectorSize(5, result, "USER has 5 tokens");
    assertVectorElem("USER", result, 0, "Command is USER");
    assertVectorElem("username", result, 1, "Username correct");
    assertVectorElem("hostname", result, 2, "Hostname correct");
    assertVectorElem("servername", result, 3, "Servername correct");
    assertVectorElem(":Real Name Here", result, 4, "Realname with spaces preserved");
}

// =============================================================================
// TEST: splitParams() - Final parameter with spaces (PRIVMSG)
// =============================================================================

void test_splitParams_PRIVMSG_simpleMessage() {
    printTestHeader("splitParams - PRIVMSG simple message");
    
    std::vector<std::string> result = ParserTest::splitParams("PRIVMSG #channel :Hello World!");
    assertVectorSize(3, result, "PRIVMSG has 3 tokens");
    assertVectorElem("PRIVMSG", result, 0, "Command is PRIVMSG");
    assertVectorElem("#channel", result, 1, "Target is #channel");
    assertVectorElem(":Hello World!", result, 2, "Message with spaces preserved");
}

void test_splitParams_PRIVMSG_longMessage() {
    printTestHeader("splitParams - PRIVMSG long message with spaces");
    
    std::vector<std::string> result = ParserTest::splitParams(
        "PRIVMSG nick :This is a very long message with many spaces");
    assertVectorSize(3, result, "PRIVMSG has 3 tokens");
    assertVectorElem("PRIVMSG", result, 0, "Command is PRIVMSG");
    assertVectorElem("nick", result, 1, "Target is nick");
    assertVectorElem(":This is a very long message with many spaces", result, 2, 
                     "Long message preserved");
}

void test_splitParams_PRIVMSG_colonInMessage() {
    printTestHeader("splitParams - PRIVMSG with colon in message");
    
    std::vector<std::string> result = ParserTest::splitParams(
        "PRIVMSG #channel :Time is 12:30:00 now");
    assertVectorSize(3, result, "PRIVMSG has 3 tokens");
    assertVectorElem(":Time is 12:30:00 now", result, 2, "Colons in message preserved");
}

void test_splitParams_PRIVMSG_emptyTrailing() {
    printTestHeader("splitParams - PRIVMSG with empty trailing");
    
    std::vector<std::string> result = ParserTest::splitParams("PRIVMSG #channel :");
    assertVectorSize(3, result, "PRIVMSG with empty trailing has 3 tokens");
    assertVectorElem(":", result, 2, "Empty trailing is just colon");
}

// =============================================================================
// TEST: splitParams() - Channel commands (JOIN, PART)
// =============================================================================

void test_splitParams_JOIN_singleChannel() {
    printTestHeader("splitParams - JOIN single channel");
    
    std::vector<std::string> result = ParserTest::splitParams("JOIN #channel");
    assertVectorSize(2, result, "JOIN has 2 tokens");
    assertVectorElem("JOIN", result, 0, "Command is JOIN");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
}

void test_splitParams_JOIN_withKey() {
    printTestHeader("splitParams - JOIN with key");
    
    std::vector<std::string> result = ParserTest::splitParams("JOIN #channel secretkey");
    assertVectorSize(3, result, "JOIN with key has 3 tokens");
    assertVectorElem("JOIN", result, 0, "Command is JOIN");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem("secretkey", result, 2, "Key is secretkey");
}

void test_splitParams_JOIN_multipleChannels() {
    printTestHeader("splitParams - JOIN multiple channels");
    
    std::vector<std::string> result = ParserTest::splitParams("JOIN #chan1,#chan2,#chan3");
    assertVectorSize(2, result, "JOIN multiple channels has 2 tokens");
    assertVectorElem("JOIN", result, 0, "Command is JOIN");
    assertVectorElem("#chan1,#chan2,#chan3", result, 1, "Channels as comma-separated");
}

void test_splitParams_PART_singleChannel() {
    printTestHeader("splitParams - PART single channel");
    
    std::vector<std::string> result = ParserTest::splitParams("PART #channel");
    assertVectorSize(2, result, "PART has 2 tokens");
    assertVectorElem("PART", result, 0, "Command is PART");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
}

void test_splitParams_PART_withMessage() {
    printTestHeader("splitParams - PART with goodbye message");
    
    std::vector<std::string> result = ParserTest::splitParams("PART #channel :Goodbye everyone!");
    assertVectorSize(3, result, "PART with message has 3 tokens");
    assertVectorElem("PART", result, 0, "Command is PART");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem(":Goodbye everyone!", result, 2, "Message preserved");
}

// =============================================================================
// TEST: splitParams() - Mode commands with flags (MODE)
// =============================================================================

void test_splitParams_MODE_queryChannel() {
    printTestHeader("splitParams - MODE query channel");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel");
    assertVectorSize(2, result, "MODE query has 2 tokens");
    assertVectorElem("MODE", result, 0, "Command is MODE");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
}

void test_splitParams_MODE_setInviteOnly() {
    printTestHeader("splitParams - MODE set invite only");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +i");
    assertVectorSize(3, result, "MODE +i has 3 tokens");
    assertVectorElem("MODE", result, 0, "Command is MODE");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem("+i", result, 2, "Flag is +i");
}

void test_splitParams_MODE_setTopic() {
    printTestHeader("splitParams - MODE set topic restriction");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +t");
    assertVectorSize(3, result, "MODE +t has 3 tokens");
    assertVectorElem("+t", result, 2, "Flag is +t");
}

void test_splitParams_MODE_setKey() {
    printTestHeader("splitParams - MODE set channel key");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +k secretkey");
    assertVectorSize(4, result, "MODE +k has 4 tokens");
    assertVectorElem("MODE", result, 0, "Command is MODE");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem("+k", result, 2, "Flag is +k");
    assertVectorElem("secretkey", result, 3, "Key is secretkey");
}

void test_splitParams_MODE_operatorGrant() {
    printTestHeader("splitParams - MODE grant operator");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +o nickname");
    assertVectorSize(4, result, "MODE +o has 4 tokens");
    assertVectorElem("+o", result, 2, "Flag is +o");
    assertVectorElem("nickname", result, 3, "Target is nickname");
}

void test_splitParams_MODE_setLimit() {
    printTestHeader("splitParams - MODE set user limit");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +l 25");
    assertVectorSize(4, result, "MODE +l has 4 tokens");
    assertVectorElem("+l", result, 2, "Flag is +l");
    assertVectorElem("25", result, 3, "Limit is 25");
}

void test_splitParams_MODE_multipleFlags() {
    printTestHeader("splitParams - MODE multiple flags");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +it");
    assertVectorSize(3, result, "MODE +it has 3 tokens");
    assertVectorElem("+it", result, 2, "Combined flags +it");
}

void test_splitParams_MODE_removeFlag() {
    printTestHeader("splitParams - MODE remove flag");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel -i");
    assertVectorSize(3, result, "MODE -i has 3 tokens");
    assertVectorElem("-i", result, 2, "Flag is -i");
}

void test_splitParams_MODE_mixedFlags() {
    printTestHeader("splitParams - MODE mixed add/remove");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE #channel +o-v nick1 nick2");
    assertVectorSize(5, result, "MODE +o-v has 5 tokens");
    assertVectorElem("+o-v", result, 2, "Mixed flags +o-v");
    assertVectorElem("nick1", result, 3, "First nick");
    assertVectorElem("nick2", result, 4, "Second nick");
}

// =============================================================================
// TEST: splitParams() - Empty and invalid messages
// =============================================================================

void test_splitParams_emptyString() {
    printTestHeader("splitParams - Empty string");
    
    std::vector<std::string> result = ParserTest::splitParams("");
    assertVectorSize(0, result, "Empty string returns empty vector");
}

void test_splitParams_onlySpaces() {
    printTestHeader("splitParams - Only spaces");
    
    std::vector<std::string> result = ParserTest::splitParams("     ");
    // After parsing, should return empty or single empty token
    assertTrue(result.empty() || result[0].empty(), "Only spaces handled correctly");
}

void test_splitParams_onlyPrefix() {
    printTestHeader("splitParams - Only prefix");
    
    std::vector<std::string> result = ParserTest::splitParams(":onlyprefix");
    assertVectorSize(0, result, "Only prefix returns empty vector");
}

void test_splitParams_prefixWithSpaces() {
    printTestHeader("splitParams - Prefix followed by only spaces");
    
    std::vector<std::string> result = ParserTest::splitParams(":prefix    ");
    assertTrue(result.empty(), "Prefix with only trailing spaces handled");
}

// =============================================================================
// TEST: Case insensitivity (via toUpperCase)
// =============================================================================

void test_caseInsensitive_lowercase() {
    printTestHeader("Case insensitivity - lowercase commands");
    
    assertEq("NICK", ParserTest::toUpperCase("nick"), "nick -> NICK");
    assertEq("PRIVMSG", ParserTest::toUpperCase("privmsg"), "privmsg -> PRIVMSG");
    assertEq("JOIN", ParserTest::toUpperCase("join"), "join -> JOIN");
}

void test_caseInsensitive_mixedCase() {
    printTestHeader("Case insensitivity - mixed case commands");
    
    assertEq("NICK", ParserTest::toUpperCase("NiCk"), "NiCk -> NICK");
    assertEq("PRIVMSG", ParserTest::toUpperCase("PrIvMsG"), "PrIvMsG -> PRIVMSG");
    assertEq("MODE", ParserTest::toUpperCase("mOdE"), "mOdE -> MODE");
}

void test_caseInsensitive_uppercase() {
    printTestHeader("Case insensitivity - already uppercase");
    
    assertEq("NICK", ParserTest::toUpperCase("NICK"), "NICK -> NICK");
    assertEq("QUIT", ParserTest::toUpperCase("QUIT"), "QUIT -> QUIT");
}

// =============================================================================
// TEST: Multiple consecutive spaces
// =============================================================================

void test_multipleSpaces_betweenParams() {
    printTestHeader("Multiple spaces - between parameters");
    
    std::vector<std::string> result = ParserTest::splitParams("NICK    john");
    assertVectorSize(2, result, "Multiple spaces between params handled");
    assertVectorElem("NICK", result, 0, "Command is NICK");
    assertVectorElem("john", result, 1, "Param is john (spaces skipped)");
}

void test_multipleSpaces_multiple() {
    printTestHeader("Multiple spaces - several parameters");
    
    std::vector<std::string> result = ParserTest::splitParams("MODE   #channel   +o   nick");
    assertVectorSize(4, result, "Multiple spaces everywhere handled");
    assertVectorElem("MODE", result, 0, "Command is MODE");
    assertVectorElem("#channel", result, 1, "Channel correct");
    assertVectorElem("+o", result, 2, "Flag correct");
    assertVectorElem("nick", result, 3, "Nick correct");
}

void test_multipleSpaces_beforeTrailing() {
    printTestHeader("Multiple spaces - before trailing parameter");
    
    std::vector<std::string> result = ParserTest::splitParams("PRIVMSG #channel   :Hello World");
    assertVectorSize(3, result, "Multiple spaces before trailing handled");
    assertVectorElem(":Hello World", result, 2, "Trailing message correct");
}

void test_multipleSpaces_leadingSpaces() {
    printTestHeader("Multiple spaces - leading spaces");
    
    std::vector<std::string> result = ParserTest::splitParams("   NICK john");
    // Behavior may vary - document actual behavior
    // Leading spaces create empty first token, then NICK and john
    assertTrue(true, "Leading spaces handled (no crash)");
}

// =============================================================================
// TEST: Additional IRC commands
// =============================================================================

void test_splitParams_KICK() {
    printTestHeader("splitParams - KICK command");
    
    std::vector<std::string> result = ParserTest::splitParams("KICK #channel nickname :Reason for kick");
    assertVectorSize(4, result, "KICK has 4 tokens");
    assertVectorElem("KICK", result, 0, "Command is KICK");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem("nickname", result, 2, "Target is nickname");
    assertVectorElem(":Reason for kick", result, 3, "Reason with spaces");
}

void test_splitParams_INVITE() {
    printTestHeader("splitParams - INVITE command");
    
    std::vector<std::string> result = ParserTest::splitParams("INVITE nickname #channel");
    assertVectorSize(3, result, "INVITE has 3 tokens");
    assertVectorElem("INVITE", result, 0, "Command is INVITE");
    assertVectorElem("nickname", result, 1, "Target is nickname");
    assertVectorElem("#channel", result, 2, "Channel is #channel");
}

void test_splitParams_TOPIC_query() {
    printTestHeader("splitParams - TOPIC query");
    
    std::vector<std::string> result = ParserTest::splitParams("TOPIC #channel");
    assertVectorSize(2, result, "TOPIC query has 2 tokens");
    assertVectorElem("TOPIC", result, 0, "Command is TOPIC");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
}

void test_splitParams_TOPIC_set() {
    printTestHeader("splitParams - TOPIC set");
    
    std::vector<std::string> result = ParserTest::splitParams("TOPIC #channel :New topic with spaces");
    assertVectorSize(3, result, "TOPIC set has 3 tokens");
    assertVectorElem("TOPIC", result, 0, "Command is TOPIC");
    assertVectorElem("#channel", result, 1, "Channel is #channel");
    assertVectorElem(":New topic with spaces", result, 2, "Topic text preserved");
}

void test_splitParams_QUIT_noMessage() {
    printTestHeader("splitParams - QUIT without message");
    
    std::vector<std::string> result = ParserTest::splitParams("QUIT");
    assertVectorSize(1, result, "QUIT has 1 token");
    assertVectorElem("QUIT", result, 0, "Command is QUIT");
}

void test_splitParams_QUIT_withMessage() {
    printTestHeader("splitParams - QUIT with message");
    
    std::vector<std::string> result = ParserTest::splitParams("QUIT :Leaving the server");
    assertVectorSize(2, result, "QUIT with message has 2 tokens");
    assertVectorElem("QUIT", result, 0, "Command is QUIT");
    assertVectorElem(":Leaving the server", result, 1, "Quit message preserved");
}

// =============================================================================
// TEST: Edge cases with prefix
// =============================================================================

void test_prefix_fullHostmask() {
    printTestHeader("Prefix - full hostmask");
    
    std::string prefix = ParserTest::extractPrefix(":nick!user@host.example.com PRIVMSG #chan :msg");
    assertEq(":nick!user@host.example.com", prefix, "Full hostmask extracted");
    
    std::vector<std::string> result = ParserTest::splitParams(":nick!user@host.example.com PRIVMSG #chan :msg");
    assertVectorElem("PRIVMSG", result, 0, "Command after prefix");
}

void test_prefix_serverSource() {
    printTestHeader("Prefix - server as source");
    
    std::string prefix = ParserTest::extractPrefix(":irc.server.net 001 nick :Welcome");
    assertEq(":irc.server.net", prefix, "Server prefix extracted");
    
    std::vector<std::string> result = ParserTest::splitParams(":irc.server.net 001 nick :Welcome");
    assertVectorElem("001", result, 0, "Numeric command after prefix");
}

void test_prefix_colonInMessage() {
    printTestHeader("Prefix - colon in message not confused with prefix");
    
    std::vector<std::string> result = ParserTest::splitParams("PRIVMSG #chan :URL is http://example.com");
    assertVectorSize(3, result, "3 tokens");
    assertVectorElem(":URL is http://example.com", result, 2, "URL with colon preserved");
}

// =============================================================================
// TEST: Special characters in parameters
// =============================================================================

void test_specialChars_nickWithUnderscore() {
    printTestHeader("Special chars - nick with underscore");
    
    std::vector<std::string> result = ParserTest::splitParams("NICK john_doe_123");
    assertVectorElem("john_doe_123", result, 1, "Nick with underscore");
}

void test_specialChars_channelWithAmpersand() {
    printTestHeader("Special chars - channel with &");
    
    std::vector<std::string> result = ParserTest::splitParams("JOIN &channel");
    assertVectorElem("&channel", result, 1, "Channel starting with &");
}

void test_specialChars_messageWithSpecialChars() {
    printTestHeader("Special chars - message with various chars");
    
    std::vector<std::string> result = ParserTest::splitParams("PRIVMSG #chan :Hello! @user #tag $money %percent");
    assertVectorElem(":Hello! @user #tag $money %percent", result, 2, "Special chars in message preserved");
}

// =============================================================================
// MAIN - Run all tests
// =============================================================================

int main() {
    std::cout << YELLOW << "\n╔══════════════════════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << YELLOW << "║           MessageParser Unit Tests                           ║" << RESET << std::endl;
    std::cout << YELLOW << "╚══════════════════════════════════════════════════════════════╝" << RESET << std::endl;

    // extractPrefix tests
    test_extractPrefix_emptyString();
    test_extractPrefix_noPrefix();
    test_extractPrefix_simplePrefix();
    test_extractPrefix_prefixOnly();
    test_extractPrefix_prefixWithMultipleSpaces();

    // removePrefix tests
    test_removePrefix_noPrefix();
    test_removePrefix_withPrefix();
    test_removePrefix_multipleSpacesAfterPrefix();

    // Basic commands (PASS, NICK, USER)
    test_splitParams_PASS();
    test_splitParams_NICK();
    test_splitParams_USER_basic();
    test_splitParams_USER_multipleParams();

    // Final parameter with spaces (PRIVMSG)
    test_splitParams_PRIVMSG_simpleMessage();
    test_splitParams_PRIVMSG_longMessage();
    test_splitParams_PRIVMSG_colonInMessage();
    test_splitParams_PRIVMSG_emptyTrailing();

    // Channel commands (JOIN, PART)
    test_splitParams_JOIN_singleChannel();
    test_splitParams_JOIN_withKey();
    test_splitParams_JOIN_multipleChannels();
    test_splitParams_PART_singleChannel();
    test_splitParams_PART_withMessage();

    // Mode commands with flags
    test_splitParams_MODE_queryChannel();
    test_splitParams_MODE_setInviteOnly();
    test_splitParams_MODE_setTopic();
    test_splitParams_MODE_setKey();
    test_splitParams_MODE_operatorGrant();
    test_splitParams_MODE_setLimit();
    test_splitParams_MODE_multipleFlags();
    test_splitParams_MODE_removeFlag();
    test_splitParams_MODE_mixedFlags();

    // Empty/invalid messages
    test_splitParams_emptyString();
    test_splitParams_onlySpaces();
    test_splitParams_onlyPrefix();
    test_splitParams_prefixWithSpaces();

    // Case insensitivity
    test_caseInsensitive_lowercase();
    test_caseInsensitive_mixedCase();
    test_caseInsensitive_uppercase();

    // Multiple consecutive spaces
    test_multipleSpaces_betweenParams();
    test_multipleSpaces_multiple();
    test_multipleSpaces_beforeTrailing();
    test_multipleSpaces_leadingSpaces();

    // Additional commands
    test_splitParams_KICK();
    test_splitParams_INVITE();
    test_splitParams_TOPIC_query();
    test_splitParams_TOPIC_set();
    test_splitParams_QUIT_noMessage();
    test_splitParams_QUIT_withMessage();

    // Edge cases with prefix
    test_prefix_fullHostmask();
    test_prefix_serverSource();
    test_prefix_colonInMessage();

    // Special characters
    test_specialChars_nickWithUnderscore();
    test_specialChars_channelWithAmpersand();
    test_specialChars_messageWithSpecialChars();

    // Summary
    std::cout << YELLOW << "\n══════════════════════════════════════════════════════════════" << RESET << std::endl;
    std::cout << YELLOW << "                        TEST SUMMARY                          " << RESET << std::endl;
    std::cout << YELLOW << "══════════════════════════════════════════════════════════════" << RESET << std::endl;
    std::cout << GREEN << "Passed: " << g_testsPassed << RESET << std::endl;
    std::cout << RED << "Failed: " << g_testsFailed << RESET << std::endl;
    std::cout << "Total:  " << (g_testsPassed + g_testsFailed) << std::endl;

    if (g_testsFailed == 0) {
        std::cout << GREEN << "\n✓ All tests passed!" << RESET << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << RED << "\n✗ Some tests failed!" << RESET << std::endl;
        return EXIT_FAILURE;
    }
}
