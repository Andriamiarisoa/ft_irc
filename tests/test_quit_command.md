# QuitCommand Test Suite Documentation

> **Version:** 1.0  
> **Author:** herrakot  
> **Last Updated:** 2026-01-22  
> **File:** `tests/test_quit_command.cpp`

---

## 📋 Overview

This document describes all test cases for `QuitCommand.cpp`. Use this as a reference when implementing or modifying the QUIT command functionality.

---

## 🔧 Compilation

```bash
# From project root
cd tests

# Compile test suite
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_quit_command.cpp ../src/Server.cpp ../src/Client.cpp \
    ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
    ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
    -o test_quit_command
```

---

## 🚀 Usage

```bash
# Run all tests
./test_quit_command

# Run specific test (1-20)
./test_quit_command 5

# Show help
./test_quit_command help

# Show valgrind instructions
./test_quit_command valgrind

# Show manual test instructions
./test_quit_command manual
```

---

## ✅ Test Cases Summary

| # | Test Name | Type | Status | Dependencies |
|---|-----------|------|--------|--------------|
| 01 | QuitCommand Construction | Unit | ⬜ | None |
| 02 | QuitCommand Destruction | Unit | ⬜ | None |
| 03 | QUIT Without Message | Unit | ⬜ | None |
| 04 | QUIT With Custom Message | Unit | ⬜ | None |
| 05 | Special Characters | Unit | ⬜ | None |
| 06 | getClientChannels | Unit | ⬜ | Channel* |
| 07 | broadcastQuitNotification | Unit | ⬜ | Channel* |
| 08 | Empty Channel Removal | Unit | ⬜ | None |
| 09 | removeChannel Function | Unit | ⬜ | None |
| 10 | Client Removed All Channels | Unit | ⬜ | Channel* |
| 11 | Other Clients See QUIT | Manual | ⬜ | Channel* |
| 12 | Socket Closed | Manual | ⬜ | None |
| 13 | No Memory Leaks | Valgrind | ⬜ | None |
| 14 | Client Object Deleted | Unit | ⬜ | None |
| 15 | QUIT Before Registration | Unit | ⬜ | None |
| 16 | QUIT Empty Nickname | Unit | ⬜ | None |
| 17 | Concurrent QUITs | Unit | ⬜ | Channel* |
| 18 | Broadcast All Channels | Unit | ⬜ | Channel* |
| 19 | QUIT Not Sent to Self | Unit | ⬜ | Channel* |
| 20 | Operator Status Removed | Unit | ⬜ | Channel* |

**Legend:** 
- ⬜ Not tested | ✅ Passed | ❌ Failed
- **Channel*** = Requires Channel class to be fully implemented (addMember, isMember, removeMember)

> **Note:** Tests marked with "Channel*" will pass but only test structural aspects (no crashes) 
> until the Channel class is fully implemented. Full functional testing requires Channel implementation.

---

## 📝 Detailed Test Descriptions

### TEST 01: QuitCommand Construction
**Type:** Unit Test  
**Purpose:** Verify QuitCommand can be constructed properly.

**Checks:**
- ✅ QuitCommand created with empty params
- ✅ QuitCommand created with message param
- ✅ QuitCommand created with long message (500 chars)

**Expected:** No crash, object created successfully.

---

### TEST 02: QuitCommand Destruction
**Type:** Unit Test (+ Valgrind)  
**Purpose:** Verify destructor cleans up properly.

**Checks:**
- ✅ 10 create/destroy cycles completed
- ✅ No memory leaks (run with valgrind)

**Valgrind Command:**
```bash
valgrind --leak-check=full ./test_quit_command 2
```

---

### TEST 03: QUIT Without Message (Default)
**Type:** Unit Test  
**Purpose:** Verify QUIT without message uses default format.

**Expected Format:**
```
:nick!user@host QUIT\r\n
```

**Note:** No trailing colon or message when params are empty.

---

### TEST 04: QUIT With Custom Message
**Type:** Unit Test  
**Purpose:** Verify QUIT with message includes custom text.

**Expected Format:**
```
:nick!user@host QUIT :custom message\r\n
```

**Examples:**
```
Input:  QUIT :Going to sleep
Output: :john!john@host QUIT :Going to sleep
```

---

### TEST 05: Special Characters in Message
**Type:** Unit Test  
**Purpose:** Verify special characters don't cause crashes.

**Test Cases:**
- "Hello World!" - Basic with exclamation
- "Test: with colon" - Contains colon
- "Multi  spaces" - Multiple spaces
- "Emoji test 😀" - Unicode
- "Numbers 12345" - Numeric
- "Symbols !@#$%^&*()" - Special symbols
- "" - Empty message

---

### TEST 06: getClientChannels Function
**Type:** Unit Test  
**Purpose:** Verify Server::getClientChannels() returns correct list.

**Checks:**
- ✅ Client in no channels → empty vector
- ✅ Client in 1 channel → vector of size 1
- ✅ Client in 3 channels → vector of size 3

---

### TEST 07: broadcastQuitNotification Function
**Type:** Unit Test  
**Purpose:** Verify quit message is broadcast and client is removed.

**Checks:**
- ✅ Client removed from channel after broadcast
- ✅ Works with multiple channels
- ✅ All channel members receive message

---

### TEST 08: Empty Channel Removal
**Type:** Unit Test  
**Purpose:** Verify empty channels are deleted after QUIT.

**Checks:**
- ✅ Channel created successfully
- ✅ removeChannel() can be called
- ✅ No crash on empty channel removal

**Important:** When last member quits, channel should be deleted to prevent memory leaks.

---

### TEST 09: removeChannel Function
**Type:** Unit Test  
**Purpose:** Verify Server::removeChannel() works correctly.

**Checks:**
- ✅ Channel is deleted from server map
- ✅ Removing non-existent channel doesn't crash
- ✅ Function is case-insensitive

---

### TEST 10: Client Removed from All Channels
**Type:** Unit Test  
**Purpose:** Verify client is removed from ALL channels on QUIT.

**Checks:**
- ✅ Client added to 5 channels
- ✅ After broadcastQuitNotification:
  - Removed from channel 1
  - Removed from channel 2
  - Removed from channel 3
  - Removed from channel 4
  - Removed from channel 5

---

### TEST 11: Other Clients See QUIT Message
**Type:** Manual Test  
**Purpose:** Verify other channel members receive QUIT notification.

**Manual Steps:**
1. Start server: `./ircserv 6667 password`
2. Connect 2 clients with `nc localhost 6667`
3. Both join: `JOIN #test`
4. Client 1 sends: `QUIT :Goodbye`
5. Verify Client 2 sees: `:nick!user@host QUIT :Goodbye`

---

### TEST 12: Socket Closed Correctly
**Type:** Manual Test  
**Purpose:** Verify socket file descriptor is closed.

**Verification:**
```bash
lsof -i :6667
# Should show no connection for the quitting client
```

---

### TEST 13: No Memory Leaks
**Type:** Valgrind Test  
**Purpose:** Verify no memory leaks after QUIT.

**Command:**
```bash
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./test_quit_command
```

**Expected Results:**
- definitely lost: 0 bytes
- indirectly lost: 0 bytes
- All heap blocks were freed

**Test:** 100 QUIT simulation cycles

---

### TEST 14: Client Object Deleted
**Type:** Unit Test  
**Purpose:** Verify Client object is properly deleted.

**Critical Points:**
- ✅ Client removed from clients map
- ✅ `delete client` is called
- ⚠️ **NEVER access client pointer after disconnectClient()**

---

### TEST 15: QUIT Before Registration
**Type:** Unit Test  
**Purpose:** Verify unregistered clients can QUIT.

**Checks:**
- ✅ QuitCommand created for client without NICK/USER
- ✅ No channels to broadcast to
- ✅ Clean disconnect

---

### TEST 16: QUIT with Empty Nickname
**Type:** Unit Test  
**Purpose:** Verify empty nickname doesn't cause crash.

**Note:** Empty nickname results in malformed message but should not crash:
```
:!@host QUIT :Goodbye\r\n
```

---

### TEST 17: Concurrent QUITs
**Type:** Unit Test  
**Purpose:** Verify server handles multiple simultaneous QUITs.

**Checks:**
- ✅ 5 clients added to channel
- ✅ All clients removed after QUIT sequence
- ✅ No crash or corruption

---

### TEST 18: QUIT Broadcast to All Channels
**Type:** Unit Test  
**Purpose:** Verify QUIT is sent to all client's channels.

**Scenario:**
- Quitter is in: #chan1, #chan2, #chan3
- Observer is in: #chan1, #chan3

**Results:**
- ✅ Quitter removed from chan1, chan2, chan3
- ✅ Observer still in chan1 and chan3
- ✅ Observer receives QUIT in both channels

---

### TEST 19: QUIT Not Sent to Self
**Type:** Unit Test  
**Purpose:** Verify quitting client doesn't receive their own QUIT.

**Important:** Use `Channel::broadcast(msg, exclude)` where exclude is the quitting client.

---

### TEST 20: Operator Status Removed
**Type:** Unit Test  
**Purpose:** Verify operator status is removed when client quits.

**Checks:**
- ✅ Client was operator before quit
- ✅ Client removed from channel after quit
- ✅ Operator status implicitly removed with membership

---

## 🧪 Manual Test Instructions

### Test 1: Basic QUIT
```bash
# Terminal 1
./ircserv 6667 password

# Terminal 2
nc localhost 6667
PASS password
NICK testuser
USER testuser 0 * :Test
QUIT
# Expected: Connection closes
```

### Test 2: QUIT with Message
```bash
nc localhost 6667
# ... register ...
QUIT :Goodbye everyone!
# Expected: Connection closes, message sent to others
```

### Test 3: Broadcast Verification
```bash
# Connect 2 clients, both join #test
# Client 1: QUIT :Leaving
# Client 2 should see: :nick!user@host QUIT :Leaving
```

### Test 4: Empty Channel Removal
```bash
# One client joins #test
# Client quits
# Verify #test no longer exists on server
```

---

## ⚠️ Common Issues

### Issue: Use-After-Free Crash
**Cause:** Accessing `client` pointer after `disconnectClient()`

**Solution:**
```cpp
// WRONG
server->disconnectClient(client->getFd());
client->getNickname(); // CRASH!

// CORRECT
server->disconnectClient(client->getFd());
return; // Do nothing else
```

### Issue: QUIT Not Seen by Others
**Cause:** Broadcast called after disconnect

**Solution:** Always broadcast BEFORE disconnecting.

### Issue: Memory Leak - Channels
**Cause:** Empty channels not deleted

**Solution:**
```cpp
if (channel->getMembers().empty()) {
    server->removeChannel(channel->getName());
}
```

---

## 📊 Dependencies

### Channel Class Requirements

The following Channel methods must be implemented for full QUIT testing:

| Method | Status | Required For |
|--------|--------|--------------|
| `addMember()` | ⬜ | Tests 6, 7, 10, 11, 17, 18, 19, 20 |
| `removeMember()` | ⬜ | Tests 7, 10, 18, 20 |
| `isMember()` | ⬜ | Tests 6, 7, 10, 11, 18, 20 |
| `broadcast()` | ⬜ | Tests 7, 11, 19 |
| `addOperator()` | ⬜ | Test 20 |
| `isOperator()` | ⬜ | Test 20 |
| `getMembers()` | ✅ | Test 8, 13 |

**Legend:** ⬜ Stub (not implemented) | ✅ Implemented

### Server Class Requirements

| Function | File | Required For |
|----------|------|--------------|
| `getClientChannels()` | Server.cpp | Getting client's channels |
| `broadcastQuitNotification()` | Server.cpp | Broadcasting QUIT |
| `removeChannel()` | Server.cpp | Removing empty channels |
| `disconnectClient()` | Server.cpp | Closing connection |
| `getOrCreateChannel()` | Server.cpp | Creating channels |

---

## ✅ Implementation Checklist

Copy this to track your progress:

```
### QuitCommand Implementation Status

- [ ] QUIT disconnects the client
- [ ] QUIT :message includes custom message
- [ ] QUIT without message uses default format
- [ ] QUIT broadcasts to all channels
- [ ] Client removed from all channels
- [ ] Empty channels deleted after QUIT
- [ ] Socket closed correctly
- [ ] No memory leaks (valgrind)
- [ ] Client object deleted
- [ ] Other clients see QUIT message
- [ ] No use-after-free bugs
- [ ] Operator status removed
- [ ] Works for unregistered clients
- [ ] Handles empty nickname
- [ ] Handles concurrent QUITs
```

---

## 🔗 Related Files

- `QuitCommand.hpp/cpp` - Main implementation
- `Server.hpp/cpp` - disconnectClient(), broadcastQuitNotification()
- `Channel.hpp/cpp` - removeMember(), broadcast()
- `Client.hpp/cpp` - getChannels(), getFd()
- `PartCommand.cpp` - Similar channel cleanup logic
