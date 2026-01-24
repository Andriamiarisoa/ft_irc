# Server.cpp Test Suite Documentation

> **Version:** 1.0  
> **Author:** herrakot  
> **Last Updated:** 2026-01-20  
> **File:** `tests/test_server.cpp`

---

## 📋 Overview

This document describes all test cases for `Server.cpp`. Use this as a reference when making changes to the server implementation.

---

## 🔧 Compilation

```bash
# From project root
cd tests

# Compile test suite
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_server.cpp \
    ../src/Server.cpp ../src/Client.cpp ../src/Channel.cpp \
    ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/InviteCommand.cpp ../src/JoinCommand.cpp ../src/KickCommand.cpp \
    ../src/ModeCommand.cpp ../src/NickCommand.cpp ../src/PartCommand.cpp \
    ../src/PassCommand.cpp ../src/PrivmsgCommand.cpp ../src/QuitCommand.cpp \
    ../src/TopicCommand.cpp ../src/UserCommand.cpp \
    -o test_server

# Or add to Makefile:
test: $(OBJS)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) tests/test_server.cpp $(OBJS) -o test_server
```

---

## 🚀 Usage

```bash
# Run all tests
./test_server

# Run specific test (1-20)
./test_server 5

# Show help
./test_server help

# Show valgrind instructions
./test_server valgrind

# Show manual/integration test instructions
./test_server manual
```

---

## ✅ Test Cases Summary

| # | Test Name | Type | Status |
|---|-----------|------|--------|
| 01 | Server Construction | Unit | ⬜ |
| 02 | Server Destruction | Unit | ⬜ |
| 03 | Port Validation | Unit | ⬜ |
| 04 | Socket Setup | Unit | ⬜ |
| 05 | Client Management (Server-side) | Unit | ⬜ |
| 06 | Channel Management | Unit | ⬜ |
| 07 | toLower Function | Unit | ⬜ |
| 08 | getClientByNick Function | Unit | ⬜ |
| 09 | Malformed Input Handling | Unit | ⬜ |
| 10 | Channel Name Validation | Unit | ⬜ |
| 11 | Signal Handling | Manual | ⬜ |
| 12 | Memory Management | Valgrind | ⬜ |
| 13 | CPU Usage (Select Timeout) | Manual | ⬜ |
| 14 | Concurrent Connections | Manual | ⬜ |
| 15 | Disconnect Handling | Manual | ⬜ |
| 16 | Non-blocking Socket Mode | Unit | ⬜ |
| 17 | Welcome Message | Manual | ⬜ |
| 18 | Iterator Invalidation Safety | Unit | ⬜ |
| 19 | Error Recovery | Unit | ⬜ |
| 20 | SO_REUSEADDR Option | Manual | ⬜ |

**Legend:** ⬜ Not tested | ✅ Passed | ❌ Failed

---

## 📝 Detailed Test Descriptions

### TEST 01: Server Construction
**Type:** Unit Test  
**Purpose:** Verifies that the Server object can be constructed with valid parameters.

**Checks:**
- ✅ Server constructed with valid port 6667
- ✅ Server constructed with boundary ports (1, 65535, 8080)
- ✅ Server constructed with empty password
- ✅ Server constructed with 1000-char password

**Expected:** No crash, object created successfully.

---

### TEST 02: Server Destruction
**Type:** Unit Test (+ Valgrind)  
**Purpose:** Verifies that the Server destructor cleans up properly.

**Checks:**
- ✅ Server deleted without crash
- ✅ 10 create/destroy cycles completed

**Expected:** No memory leaks, all resources freed.

**Valgrind Command:**
```bash
valgrind --leak-check=full ./test_server 2
```

---

### TEST 03: Port Validation
**Type:** Unit Test  
**Purpose:** Verifies that the server validates port numbers correctly.

**Valid Ports:**
- Port 1 (minimum)
- Port 65535 (maximum)
- Port 6667 (standard IRC)

**Invalid Ports (should be rejected):**
- Port 0 (below minimum)
- Port 65536 (above maximum)
- Port -1 (negative)
- Port 99999 (way above maximum)

---

### TEST 04: Socket Setup
**Type:** Unit Test  
**Purpose:** Verifies socket creation, binding, and listening.

**Checks:**
- Socket created with `socket()`
- `SO_REUSEADDR` option set
- `O_NONBLOCK` flag set
- `bind()` succeeds
- `listen()` succeeds

---

### TEST 05: Client Management (Server-side)
**Type:** Unit Test  
**Purpose:** Verifies Server's ability to work with Client objects.

> **Note:** Client-specific tests (setNickname, setUsername, buffer operations) belong to the Client test suite.
> This test only verifies Server can interact with Client objects without crashing.

**Checks:**
- ✅ Client created with correct fd
- ℹ️ Nickname/Username setters → see Client test suite
- ℹ️ Buffer operations → see Client test suite
- ✅ Client deleted without crash
- ✅ Multiple client create/destroy cycles

---

### TEST 06: Channel Management
**Type:** Unit Test  
**Purpose:** Verifies channel creation and validation.

**Valid Channel Names:**
- `#general` ✅
- `#test-channel` ✅
- `&local` ✅

**Case Insensitivity:**
- `#General` = `#GENERAL` = `#general` ✅

**Invalid Channel Names:**
- `nochanprefix` (no # or &) ❌
- `#` (too short) ❌
- `#bad channel` (contains space) ❌
- `#bad,channel` (contains comma) ❌

---

### TEST 07: toLower Function
**Type:** Unit Test  
**Purpose:** Verifies case-insensitive string conversion.

**Test Cases:**
| Input | Expected Output |
|-------|-----------------|
| `"HELLO"` | `"hello"` |
| `"Hello World"` | `"hello world"` |
| `"already lower"` | `"already lower"` |
| `"MiXeD CaSe"` | `"mixed case"` |
| `""` | `""` |
| `"123ABC!@#"` | `"123abc!@#"` |

---

### TEST 08: getClientByNick Function
**Type:** Unit Test  
**Purpose:** Verifies nickname lookup is case-insensitive.

**Checks:**
- `getClientByNick("john")` finds client "JOHN"
- `getClientByNick("JANE")` finds client "jane"
- `getClientByNick("nonexistent")` returns `NULL`

---

### TEST 09: Malformed Input Handling
**Type:** Unit Test  
**Purpose:** Verifies server doesn't crash on bad input.

> **Note:** Detailed buffer/command parsing tests belong to the Client test suite.
> This test only verifies Server doesn't crash when using Client objects.

**Test Cases:**
- ✅ Empty input handled without crash
- ✅ Binary data doesn't crash
- ℹ️ Buffer parsing details → see Client test suite

---

### TEST 10: Channel Name Validation (isValidName)
**Type:** Unit Test  
**Purpose:** Verifies IRC channel name rules.

**Rules:**
1. Must start with `#` or `&`
2. Length: 2-50 characters
3. Cannot contain: space, comma, control chars (ASCII 0-31), bell (`\x07`)

---

### TEST 11: Signal Handling
**Type:** Manual Test  
**Purpose:** Verifies SIGINT and SIGTERM handlers.

**Manual Steps:**
1. Start the server
2. Press Ctrl+C
3. Verify graceful shutdown message appears
4. Verify all clients receive shutdown notification
5. Verify server exits cleanly (exit code 0)

---

### TEST 12: Memory Management
**Type:** Valgrind Test  
**Purpose:** Verifies no memory leaks.

**Command:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./test_server
```

**Expected Results:**
- 0 bytes definitely lost
- 0 bytes indirectly lost
- All heap blocks freed

---

### TEST 13: CPU Usage (Select Timeout)
**Type:** Manual Test  
**Purpose:** Verifies select timeout doesn't cause busy-waiting.

**Manual Steps:**
1. Start the server
2. Leave it idle for 30 seconds
3. Check CPU usage with `top` or `htop`
4. CPU usage should be near 0% when idle

**Configuration:**
- `tv_sec = 1`
- `tv_usec = 0`
- 1 second timeout between select() calls

---

### TEST 14: Concurrent Connections
**Type:** Manual Test  
**Purpose:** Verifies multiple clients can connect simultaneously.

**Manual Steps:**
```bash
# Terminal 1
./server 6667 password

# Terminals 2-6
nc localhost 6667
```

**Stress Test:**
```bash
for i in {1..50}; do nc localhost 6667 & done
```

---

### TEST 15: Disconnect Handling
**Type:** Manual Test  
**Purpose:** Verifies clean disconnect when client closes connection.

**Checks:**
- ✅ `disconnectClient()` removes from all channels
- ✅ `disconnectClient()` broadcasts QUIT message
- ✅ `disconnectClient()` closes socket
- ✅ `disconnectClient()` deletes client object
- ✅ `disconnectClient()` removes from clients map

**Manual Steps:**
1. Connect a client with `nc localhost 6667`
2. Send NICK and JOIN commands
3. Press Ctrl+C in nc to disconnect
4. Check server logs for disconnect message
5. Verify other channel members receive QUIT

---

### TEST 16: Non-blocking Socket Mode
**Type:** Unit Test  
**Purpose:** Verifies all sockets are set to non-blocking.

**Checks:**
- ✅ `fcntl(F_SETFL, O_NONBLOCK)` used for server socket
- ✅ `fcntl(F_SETFL, O_NONBLOCK)` used for client sockets
- ✅ `EAGAIN/EWOULDBLOCK` handled in `handleClientMessage()`

---

### TEST 17: Welcome Message
**Type:** Manual Test  
**Purpose:** Verifies clients receive welcome message on connect.

**Current Message:**
```
:Server ft_ic : welcome tho the IRC Server\r\n
```

> ⚠️ **Note:** Contains typo "tho" instead of "to"

**Manual Verification:**
```bash
nc localhost 6667
# Should immediately receive welcome message
```

---

### TEST 18: Iterator Invalidation Safety
**Type:** Unit Test  
**Purpose:** Verifies safe iteration when modifying collections.

**Implementation:**
- `handleSelect()` collects fds into vector before processing
- This prevents iterator invalidation when `disconnectClient()` erases

**Test Scenario:**
1. Connect 3 clients
2. Disconnect all 3 simultaneously (Ctrl+C each)
3. Server should not crash

---

### TEST 19: Error Recovery
**Type:** Unit Test  
**Purpose:** Verifies server recovers from various error conditions.

**Scenarios:**
| Error | Expected Behavior |
|-------|-------------------|
| `select()` returns `EINTR` | Server continues, no crash |
| `accept()` fails | Log error, continue accepting others |
| `recv()` fails with unknown error | Disconnect client, server continues |
| `send()` fails | Handle gracefully |

---

### TEST 20: SO_REUSEADDR Option
**Type:** Manual Test  
**Purpose:** Verifies SO_REUSEADDR is set for quick restart.

**Without SO_REUSEADDR:**
- "Address already in use" error on quick restart

**Manual Test:**
1. Start server
2. Stop server (Ctrl+C)
3. Immediately restart server
4. Should bind successfully without waiting

---

## 🔬 Integration Tests

These tests require running the actual server:

### 1. Basic Connection Test
```bash
# Terminal 1
./server 6667 password

# Terminal 2
nc localhost 6667
```
**Expected:** Receive welcome message

### 2. Multiple Clients Test
```bash
# Terminal 1
./server 6667 password

# Terminals 2-6
nc localhost 6667
```
**Expected:** All clients connected, server shows count

### 3. Graceful Shutdown Test
```bash
# Terminal 1
./server 6667 password

# Terminal 2
nc localhost 6667

# Press Ctrl+C in Terminal 1
```
**Expected:** Client receives shutdown message

### 4. Valgrind Memory Test
```bash
valgrind --leak-check=full --show-leak-kinds=all ./server 6667 password
# Connect/disconnect a few clients
# Press Ctrl+C to stop
```
**Expected:** 0 bytes definitely lost

### 5. Stress Test
```bash
./server 6667 password &
for i in {1..100}; do
  echo -e 'NICK test$i\r\n' | nc -q1 localhost 6667 &
done
```
**Expected:** No crashes, all connections handled

---

## 🐛 Common Issues

### Issue: "Address already in use"
**Cause:** Previous server instance still binding port  
**Solution:** Wait 60 seconds or check `SO_REUSEADDR` is set

### Issue: Segmentation fault on client disconnect
**Cause:** Iterator invalidation in `handleSelect()`  
**Solution:** Collect fds in vector before processing

### Issue: High CPU usage when idle
**Cause:** select() timeout too short or missing  
**Solution:** Ensure `tv_sec = 1` in timeout struct

### Issue: Memory leaks
**Cause:** Not deleting clients/channels properly  
**Solution:** Check `disconnectClient()` and destructor

---

## 📊 Test Coverage Checklist

### Core Functionality
- [ ] Server starts on specified port
- [ ] Server rejects connections on wrong port
- [ ] Multiple clients can connect simultaneously
- [ ] Server handles client disconnections properly
- [ ] Server doesn't crash on malformed input

### Memory & Performance
- [ ] No memory leaks (verified with valgrind)
- [ ] CPU usage near 0% when idle
- [ ] All resources freed on shutdown

### Signal Handling
- [ ] Server responds correctly to Ctrl+C (SIGINT)
- [ ] Server responds correctly to SIGTERM
- [ ] Graceful shutdown with client notification

### Network
- [ ] Non-blocking sockets used
- [ ] SO_REUSEADDR option set
- [ ] EAGAIN/EWOULDBLOCK handled properly

---

## 👥 Contributors

| Name | Tests Assigned |
|------|----------------|
| NERO | Server.cpp, Channel.cpp |
| BAHOLY | MessageParser.cpp, PrivmsgCommand.cpp |
| YASSER | Client.cpp, Command.cpp |

---

## 📅 Test Log

| Date | Tester | Tests Run | Passed | Failed | Notes |
|------|--------|-----------|--------|--------|-------|
| 2026-01-20 | - | - | - | - | Initial test suite created |

---

> **Remember:** Run all tests before pushing to main branch!
