# KICK Command Test Suite Documentation

## Overview

This document describes the isolated test suite for the `KickCommand` class in the ft_irc project.  
The tests are **standalone** and do not require a running server.

---

## Compilation

```bash
cd tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_kick_command.cpp \
    ../src/KickCommand.cpp \
    ../src/JoinCommand.cpp \
    ../src/Command.cpp \
    ../src/Channel.cpp \
    ../src/Client.cpp \
    ../src/Server.cpp \
    ../src/MessageParser.cpp \
    ../src/PassCommand.cpp \
    ../src/NickCommand.cpp \
    ../src/UserCommand.cpp \
    ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp \
    ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp \
    ../src/ModeCommand.cpp \
    ../src/QuitCommand.cpp \
    -o test_kick_command
```

## Usage

```bash
# Run all tests
./test_kick_command

# Run specific test (e.g., test 5)
./test_kick_command 5
```

---

## Test Cases

### Basic Tests (Required by Subject)

| # | Test Name | Description | Expected Behavior | Error Code |
|---|-----------|-------------|-------------------|------------|
| 01 | KICK Success | `KICK #channel user` by operator | User kicked, KICK broadcast | - |
| 02 | KICK With Reason | `KICK #channel user :reason` | Reason included in message | - |
| 03 | KICK No Parameters | `KICK` without params | Error sent | 461 (ERR_NEEDMOREPARAMS) |
| 04 | KICK Nonexistent Channel | `KICK #nonexistent user` | Error sent | 403 (ERR_NOSUCHCHANNEL) |
| 05 | KICK by Non-Member | User not in channel tries KICK | Error sent | 442 (ERR_NOTONCHANNEL) |
| 06 | KICK by Non-Operator | Member without op tries KICK | Error sent | 482 (ERR_CHANOPRIVSNEEDED) |
| 07 | KICK Nonexistent User | `KICK #channel ghost` | Error sent | 401 (ERR_NOSUCHNICK) |
| 08 | KICK User Not in Channel | Target exists but not in channel | Error sent | 441 (ERR_USERNOTINCHANNEL) |
| 09 | KICK Broadcasts | KICK message sent to all members | broadcast() called | - |
| 10 | Kicked User Removed | Complete removal from channel | Not in members/operators | - |
| 11 | Empty Channel Removed | Last member kicked leaves empty channel | Channel deleted | - |
| 12 | Kicked User Can Rejoin | KICK ≠ BAN | User can JOIN again | - |

### Extended Tests (Additional Coverage)

| # | Test Name | Description | Expected Behavior |
|---|-----------|-------------|-------------------|
| 13 | KICK Before Registration | Unregistered client | Error 451 sent |
| 14 | Operator Kicks Operator | Op kicks another op | Kick succeeds |
| 15 | KICK Default Reason | No reason provided | Uses kicker's nickname |
| 16 | KICK Multiple Channels | User in multiple channels | Only target channel affected |
| 17 | Self-Kick | Operator kicks themselves | Implementation-dependent |
| 18 | KICK Long Reason | 500-char reason | Handled without crash |
| 19 | KICK Special Chars | Special chars in reason | Handled correctly |
| 20 | Multiple Consecutive Kicks | Kick several users in row | All kicked successfully |
| 21 | KICK Case Sensitivity | `bob` vs `Bob` | Depends on implementation |
| 22 | KICK Empty Reason | Empty string as reason | Uses default reason |
| 23 | Stress Test | 20 users kicked | No crash, all removed |
| 24 | NULL Safety Checks | NULL kicker/target | No crash |
| 25 | Memory Operations | 10 cycles of operations | No crash/leaks |

---

## Error Codes Reference

| Code | Name | Description |
|------|------|-------------|
| 401 | ERR_NOSUCHNICK | Target nickname doesn't exist |
| 403 | ERR_NOSUCHCHANNEL | Channel doesn't exist |
| 441 | ERR_USERNOTINCHANNEL | Target not in specified channel |
| 442 | ERR_NOTONCHANNEL | Kicker not in specified channel |
| 451 | ERR_NOTREGISTERED | Client not registered (PASS/NICK/USER) |
| 461 | ERR_NEEDMOREPARAMS | Missing required parameters |
| 482 | ERR_CHANOPRIVSNEEDED | Kicker not channel operator |

---

## KICK Command Syntax

```
KICK <channel> <user> [:<reason>]
```

### Parameters
- `<channel>` - The channel to kick from (e.g., `#general`)
- `<user>` - Nickname of user to kick (e.g., `Bob`)
- `<reason>` - Optional reason (default: kicker's nickname)

### Examples
```
KICK #general Bob                    → Kicks Bob, reason "Alice"
KICK #general Bob :Spamming          → Kicks Bob, reason "Spamming"
KICK #general Bob :Being annoying    → Kicks Bob, reason "Being annoying"
```

---

## Test Status Legend

| Symbol | Meaning |
|--------|---------|
| `[PASS]` | Test passed |
| `[FAIL]` | Test failed - bug detected |
| `[SKIP]` | Test skipped - feature not implemented |
| `[INFO]` | Informational - behavior varies |

---

## Functions Tested

### KickCommand::execute()

**Flow:**
```
1. Check client registered → 451 if not
2. Check params (need 2+) → 461 if missing
3. Check channel exists → 403 if not
4. Check kicker in channel → 442 if not
5. Check kicker is operator → 482 if not
6. Check target exists → 401 if not
7. Check target in channel → 441 if not
8. Call channel->kickMember()
```

### Channel::kickMember(kicker, target, reason)

**Flow:**
```
1. NULL checks
2. Verify kicker is operator
3. Verify target is member
4. Broadcast KICK message
5. Remove from members set
6. Remove from operators set
7. Remove from invitedUsers set
8. Update target's channel list
9. Delete channel if empty
```

**Dependencies:**
- `Client::isRegistered()` - Check registration
- `Client::getNickname()` - For messages and default reason
- `Server::channelExistOrNot()` - Check channel exists
- `Server::getChannel()` - Get channel pointer
- `Server::getClientByNick()` - Find target client
- `Channel::isMember()` - Check membership
- `Channel::isOperator()` - Check operator status
- `Channel::broadcast()` - Send to all members
- `Channel::getMembersCount()` - Check if empty
- `Server::removeChannel()` - Delete empty channel

---

## KICK Message Format

```
:Kicker!username@host KICK #channel Target :reason\r\n
```

### Example
```
:Alice!alice@host KICK #general Bob :Spamming the channel\r\n
```

---

## Notes for Developers

### Expected "Bad file descriptor" Errors

The tests use **fake file descriptors** (10, 11, 12...) for mock clients.  
When `broadcast()` or `sendMessage()` tries to send via `send()`, it fails.  
**This is expected behavior** - the tests verify logic, not network I/O.

### Important Implementation Notes

1. **KICK vs BAN**: KICK only removes user temporarily. User can rejoin immediately.

2. **Double checks**: KickCommand does checks, then kickMember() does them again.
   This is intentional for safety but could be optimized.

3. **Default reason**: If no reason provided, use kicker's nickname (RFC standard).

4. **Operator removal**: If kicked user was an operator, they lose op status.

5. **Empty channel**: When last member is kicked, channel should be deleted.

### Adding New Tests

1. Create function: `void test_XX_description()`
2. Add to `tests[]` array in `main()`
3. Update this documentation

---

## Coverage Checklist

- [x] KICK #channel user kicks successfully
- [x] KICK #channel user :reason includes the reason
- [x] KICK without parameters sends error 461
- [x] KICK #nonexistent user sends error 403
- [x] KICK by non-member sends error 442
- [x] KICK by non-operator sends error 482
- [x] KICK nonexistent user sends error 401
- [x] KICK user not in channel sends error 441
- [x] KICK broadcasts to all members
- [x] Kicked user is removed from channel
- [x] Empty channel is deleted after kick
- [x] Kicked user can rejoin (not banned)
- [x] KICK before registration (error 451)
- [x] Operator kicks another operator
- [x] Default reason (kicker's nickname)
- [x] KICK from one of multiple channels
- [x] Self-kick behavior
- [x] Long reason handling
- [x] Special characters in reason
- [x] Multiple consecutive kicks
- [x] Case sensitivity
- [x] Empty reason handling
- [x] Stress test (20 kicks)
- [x] NULL safety checks
- [x] Memory leak check

---

## Differences: KICK vs PART vs QUIT

| Command | Who Leaves | Scope | Can Rejoin |
|---------|------------|-------|------------|
| **KICK** | Target (by operator) | One channel | Yes (immediately) |
| **PART** | Self (voluntary) | One channel | Yes |
| **QUIT** | Self (disconnect) | All channels | Yes (reconnect) |

---

## Author

ft_irc team - 2026/01/23
