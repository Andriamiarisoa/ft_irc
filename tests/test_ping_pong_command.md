# PING/PONG Command Test Suite Documentation

## Overview

This document describes the isolated test suite for the `PingCommand` and `PongCommand` classes in the ft_irc project.  
The tests are **standalone** and do not require a running server.

---

## Compilation

```bash
cd tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_ping_pong_command.cpp ../src/*.cpp \
    -o test_ping_pong_command
```

## Usage

```bash
# Run all tests
./test_ping_pong_command

# Run specific test (e.g., test 5)
./test_ping_pong_command 5
```

---

## What are PING and PONG?

### Purpose

PING and PONG are **keep-alive** commands used to:
1. Verify the connection is still active
2. Detect "zombie" clients (disconnected without notification)
3. Measure network latency (lag)

### Flow Diagrams

**Client-initiated PING (measure lag):**
```
Client                              Server
   │                                   │
   │  PING :LAG123                     │
   │──────────────────────────────────>│
   │                                   │
   │  :ircserv PONG ircserv :LAG123    │
   │<──────────────────────────────────│
```

**Server-initiated PING (keep-alive):**
```
Server                              Client
   │                                   │
   │  PING :1706012345                 │
   │──────────────────────────────────>│
   │                                   │
   │  PONG :1706012345                 │
   │<──────────────────────────────────│
   │                                   │
   │  [Connection confirmed active]    │
```

---

## Test Cases

### PING Command Tests (1-10)

| # | Test Name | Description | Expected Behavior |
|---|-----------|-------------|-------------------|
| 01 | PING With Valid Token | `PING :LAG123456` | Server sends `:ircserv PONG ircserv :LAG123456` |
| 02 | PING Without Token | `PING` (no params) | Error 409 (ERR_NOORIGIN) |
| 03 | PING With Numeric Token | `PING :1706012345` | Token echoed in PONG |
| 04 | PING With Special Chars | `PING :test-token_123.456` | Handled without crash |
| 05 | PING With Long Token | `PING :<500 chars>` | Handled without crash |
| 06 | PING With Empty Token | `PING :` (empty string) | Implementation-dependent |
| 07 | Multiple PINGs | 10 consecutive PINGs | All get PONG responses |
| 08 | PING From Unregistered | Client not registered | PING still works |
| 09 | PING Multiple Params | `PING :a b c` | Only first token used |
| 10 | PING Response Format | Verify IRC format | `:servername PONG servername :token` |

### PONG Command Tests (11-17)

| # | Test Name | Description | Expected Behavior |
|---|-----------|-------------|-------------------|
| 11 | PONG With Valid Token | `PONG :timestamp` | Accepted silently |
| 12 | PONG Without Token | `PONG` (no params) | Accepted silently |
| 13 | Unsolicited PONG | PONG without prior PING | No error |
| 14 | PONG Wrong Token | Token doesn't match PING | Accepted (no strict validation) |
| 15 | PONG From Unregistered | Client not registered | PONG still works |
| 16 | Multiple PONGs | 10 consecutive PONGs | All accepted |
| 17 | PONG No Response | Verify no reply sent | Server is silent |

### Integration Tests (18-25)

| # | Test Name | Description | Expected Behavior |
|---|-----------|-------------|-------------------|
| 18 | PING-PONG Round Trip | Full exchange simulation | Both directions work |
| 19 | Stress Test | 50 PINGs + 50 PONGs | No crash |
| 20 | Multiple Clients PING | 3 clients ping simultaneously | Each gets their PONG |
| 21 | PING With Unicode | UTF-8 characters in token | Handled without crash |
| 22 | Constructor/Destructor | 100 create/destroy cycles | No memory leak |
| 23 | PING Colon Prefix | Token starts with `:` | Handled correctly |
| 24 | Edge Case Safety | Minimal/empty inputs | No crash |
| 25 | Rapid Alternation | 50 rapid PING/PONG switches | No state confusion |

---

## Error Codes Reference

| Code | Name | Description |
|------|------|-------------|
| 409 | ERR_NOORIGIN | PING missing required token |

---

## Command Syntax

### PING
```
PING <token>
```
- `<token>` - Any string to be echoed back (usually timestamp or random)

### PONG
```
PONG <token>
```
- `<token>` - Token from the PING being responded to

---

## Protocol Format

### PING Response (Server → Client)
```
:servername PONG servername :token\r\n
```

**Example:**
```
:ircserv PONG ircserv :LAG123456\r\n
```

### PONG (Client → Server)
```
PONG :token\r\n
```
No response from server.

---

## Test Status Legend

| Symbol | Meaning |
|--------|---------|
| `[PASS]` | Test passed |
| `[FAIL]` | Test failed - bug detected |
| `[SKIP]` | Test skipped - feature not implemented |
| `[INFO]` | Informational message |

---

## Implementation Details

### PingCommand::execute()

**Current Implementation:**
```cpp
void PingCommand::execute() {
    // 1. Check if token provided
    if (params.empty()) {
        sendError(409, "No origin specified");
        return;
    }
    
    // 2. Build PONG response
    std::string token = params[0];
    std::string pongReply = ":ircserv PONG ircserv :" + token;
    
    // 3. Send to client
    client->sendMessage(pongReply);
}
```

**Flow:**
```
PING received
    │
    ├── params empty?
    │   └── Yes → Error 409
    │
    └── No → Send PONG with token
```

### PongCommand::execute()

**Current Implementation:**
```cpp
void PongCommand::execute() {
    // PONG is just acknowledgment
    // No response needed
    // Optional: update client's lastActivity timestamp
    (void)params;  // Unused
}
```

**Flow:**
```
PONG received
    │
    └── Do nothing (silently accept)
        └── Optionally update lastActivity (not yet implemented)
```

---

## What's NOT Yet Implemented

| Feature | Status | Description |
|---------|--------|-------------|
| MessageParser integration | ❌ | PING/PONG not in parser |
| Server-initiated PING | ❌ | Server doesn't auto-ping |
| Timeout detection | ❌ | No zombie client detection |
| lastActivity tracking | ❌ | Client activity not tracked |

### To Complete Integration

1. **Add to MessageParser.hpp:**
```cpp
#include "PingCommand.hpp"
#include "PongCommand.hpp"
```

2. **Add to MessageParser::createCommand():**
```cpp
if (cmd == "PING")
    return new PingCommand(server, client, params);
if (cmd == "PONG")
    return new PongCommand(server, client, params);
```

---

## Expected "Bad file descriptor" Errors

The tests use **fake file descriptors** (10, 11, 12...) for mock clients.  
When `sendMessage()` tries to call `send()`, it fails with "Bad file descriptor".  
**This is expected behavior** - the tests verify logic, not network I/O.

---

## Notes for Developers

### Key Behaviors

1. **PING requires token**: Without token, error 409 is sent.

2. **PONG is silent**: Server never responds to PONG.

3. **Registration not required**: PING/PONG work for unregistered clients (keep-alive during connection setup).

4. **Token is echoed exactly**: Whatever token client sends, server echoes it back.

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| PING doesn't work | Not in MessageParser | Add to createCommand() |
| No PONG response | Verify sendMessage() called | Check logs |
| Token corrupted | Parsing issue | Check MessageParser |

---

## Coverage Checklist

### PING Command
- [x] Valid token → PONG response
- [x] No token → Error 409
- [x] Numeric token
- [x] Special characters in token
- [x] Very long token (500 chars)
- [x] Empty string token
- [x] Multiple consecutive PINGs
- [x] Unregistered client
- [x] Multiple parameters (only first used)
- [x] Response format verification
- [x] Unicode/UTF-8 token

### PONG Command
- [x] Valid token accepted
- [x] No token accepted
- [x] Unsolicited PONG accepted
- [x] Wrong token accepted
- [x] Unregistered client
- [x] Multiple consecutive PONGs
- [x] No response generated

### Integration
- [x] Full round-trip simulation
- [x] Stress test (100 operations)
- [x] Multiple clients
- [x] Constructor/destructor cycles
- [x] Edge cases
- [x] Rapid alternation

---

## Differences: PING vs PONG

| Aspect | PING | PONG |
|--------|------|------|
| **Direction** | Both (client↔server) | Both (response to PING) |
| **Token Required** | Yes (error 409 if missing) | No (lenient) |
| **Server Response** | Yes (sends PONG) | No (silent) |
| **Purpose** | Ask "are you alive?" | Answer "yes, I'm alive" |
| **RFC Reference** | 4.6.2 | 4.6.3 |

---

## Running with Valgrind

```bash
valgrind --leak-check=full --show-leak-kinds=all ./test_ping_pong_command
```

Expected: No memory leaks from command execution.

---

## Author

ft_irc team - 2026/01/24
