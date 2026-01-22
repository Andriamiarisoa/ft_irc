# Channel.cpp Test Suite Documentation

> **Version:** 1.0  
> **Author:** ft_irc team  
> **Last Updated:** 2026-01-22  
> **File:** `tests/test_channel.cpp`

---

## 📋 Overview

This document describes all test cases for `Channel.cpp`. These are **isolated unit tests** that do NOT require the server to be running. Use this as a reference when making changes to the Channel implementation.

---

## 🔧 Compilation

```bash
# From project root
cd tests

# Compile test suite (isolated - minimal dependencies)
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_channel.cpp ../src/Channel.cpp ../src/Client.cpp \
    -o test_channel

# Note: Only Channel.cpp and Client.cpp needed!
# Server is passed as NULL for isolated testing
```

---

## 🚀 Usage

```bash
# Run all tests
./test_channel

# Run specific test (1-20)
./test_channel 5

# Run with valgrind for memory check
valgrind --leak-check=full ./test_channel
```

---

## ✅ Test Cases Summary

| # | Test Name | Tests | Status |
|---|-----------|-------|--------|
| 01 | Channel Construction | 9 | ⬜ |
| 02 | Channel Destruction | 3 | ⬜ |
| 03 | First Member Becomes Operator | 4 | ⬜ |
| 04 | Members Join and Leave | 7 | ⬜ |
| 05 | Operator Can Change Topic (+t) | 5 | ⬜ |
| 06 | Non-Operator Cannot Change Topic (+t) | 3 | ⬜ |
| 07 | Channel Key Blocks Joins | 7 | ⬜ |
| 08 | Invite Only Mode | 8 | ⬜ |
| 09 | User Limit | 8 | ⬜ |
| 10 | Kick Removes Member | 7 | ⬜ |
| 11 | Broadcast Function | 3 | ⬜ |
| 12 | Operator Management | 6 | ⬜ |
| 13 | Topic Restricted Mode | 3 | ⬜ |
| 14 | getMembers() Function | 4 | ⬜ |
| 15 | getMembersCount() Function | 6 | ⬜ |
| 16 | isMember/isOperator Functions | 4 | ⬜ |
| 17 | clearAllSet() Function | 3 | ⬜ |
| 18 | Edge Cases | 4 | ⬜ |
| 19 | Memory Management | 3 | ⬜ |
| 20 | Mode Combinations | 3 | ⬜ |

**Legend:** ⬜ Not tested | ✅ Passed | ❌ Failed | ⏭️ Skipped

**Total Tests:** ~97 individual test assertions

---

## 📝 Detailed Test Descriptions

### TEST 01: Channel Construction
**Purpose:** Verify that Channel objects can be created correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 1.1 | Create channel with `#general` | Name = `#general` |
| 1.2a | Channel with hyphen `#test-channel` | Accepted |
| 1.2b | Channel with `&` prefix | Accepted |
| 1.2c | Channel with numbers | Accepted |
| 1.3a | Initial topic | Empty string |
| 1.3b | Initial member count | 0 |
| 1.3c | Initial hasKey() | false |
| 1.3d | Initial isChannelFull() | false |
| 1.3e | Initial isChannelInvitOnly() | false |

---

### TEST 02: Channel Destruction
**Purpose:** Verify destructor cleans up properly without crashes or leaks.

| Test | Description | Expected |
|------|-------------|----------|
| 2.1 | Simple destruction | No crash |
| 2.2 | 100 create/destroy cycles | No crash |
| 2.3 | Destruction with key set | No crash |

**Valgrind Command:**
```bash
valgrind --leak-check=full ./test_channel 2
```

---

### TEST 03: First Member Becomes Operator
**Purpose:** Verify the first user to join becomes operator automatically.

| Test | Description | Expected |
|------|-------------|----------|
| 3.1a | First member is added | isMember() = true |
| 3.1b | First member is operator | isOperator() = true |
| 3.2a | Second member added | isMember() = true |
| 3.2b | Second member NOT operator | isOperator() = false |

---

### TEST 04: Members Join and Leave
**Purpose:** Verify addMember() and removeMember() work correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 4.1 | 5 members join | getMembersCount() = 5 |
| 4.2a | Member leaves | getMembersCount() decreases |
| 4.2b | Left member not in channel | isMember() = false |
| 4.3 | Remove non-member | No crash, count unchanged |
| 4.4a | addMember(NULL) | No crash |
| 4.4b | addMember(NULL) | Count stays 0 |

---

### TEST 05: Operator Can Change Topic (+t)
**Purpose:** Verify operators can set topic when +t mode is active.

| Test | Description | Expected |
|------|-------------|----------|
| 5.1a | Alice is operator | isOperator() = true |
| 5.1b | Operator sets topic | getTopic() = new topic |
| 5.2a | Change topic multiple times | Topic updates |
| 5.2b | Clear topic with empty string | getTopic() = "" |

---

### TEST 06: Non-Operator Cannot Change Topic (+t)
**Purpose:** Verify non-operators cannot set topic when +t mode is active.

| Test | Description | Expected |
|------|-------------|----------|
| 6.1a | Bob is not operator | isOperator() = false |
| 6.1b | Non-op cannot set topic (+t) | Topic unchanged |
| 6.2 | Non-op CAN set topic (-t) | Topic changes |

---

### TEST 07: Channel Key Blocks Unauthorized Joins
**Purpose:** Verify setKey(), hasKey(), and checkKey() work correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 7.1a | Initially no key | hasKey() = false |
| 7.1b | After setKey() | hasKey() = true |
| 7.2 | checkKey with correct key | true |
| 7.3a | checkKey with wrong key | false |
| 7.3b | checkKey with empty key | false |
| 7.4a | checkKey when no key set | true (any key works) |
| 7.5 | Remove key with setKey("") | hasKey() = false |

---

### TEST 08: Invite Only Mode
**Purpose:** Verify +i mode, inviteUser(), and isInvited() work correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 8.1a | Initially not invite-only | isChannelInvitOnly() = false |
| 8.1b | After setInviteOnly(true) | isChannelInvitOnly() = true |
| 8.1c | After setInviteOnly(false) | isChannelInvitOnly() = false |
| 8.2a | User not initially invited | isInvited() = false |
| 8.2b | After inviteUser() | isInvited() = true |
| 8.3 | Invitation cleared after join | isInvited() = false |
| 8.4 | setInviteOnly(false) clears invites | All invites cleared |
| 8.5 | inviteUser(NULL) | No crash |

---

### TEST 09: User Limit
**Purpose:** Verify setUserLimit() and isChannelFull() work correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 9.1a | Initial (limit 0 = no limit) | isChannelFull() = false |
| 9.1b | Limit set, 0 members | isChannelFull() = false |
| 9.2a | 3 members, limit 3 | getMembersCount() = 3 |
| 9.2b | At limit | isChannelFull() = true |
| 9.3a | Try join full channel | Member rejected |
| 9.3b | Count unchanged | getMembersCount() = 2 |
| 9.4 | Limit 0 = no limit | 10+ members can join |
| 9.5 | Negative limit rejected | No change |
| 9.6 | Lower limit doesn't kick | Existing members stay |

---

### TEST 10: Kick Removes Member Correctly
**Purpose:** Verify kickMember() works correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 10.1a | Operator kicks member | isMember() = false |
| 10.1b | Kicker still member | isMember() = true |
| 10.2 | Non-operator cannot kick | Target still member |
| 10.3 | Kick non-member | No crash |
| 10.4a | kickMember(NULL, target) | No crash |
| 10.4b | kickMember(kicker, NULL) | No crash |
| 10.5 | Kick removes operator status | isOperator() = false |

---

### TEST 11: Broadcast Function
**Purpose:** Verify broadcast() sends messages correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 11.1 | Broadcast to empty channel | No crash |
| 11.2 | Broadcast with exclude | No crash |
| 11.3 | Broadcast with NULL exclude | No crash |

*Note: Cannot verify message content without mocking sendMessage()*

---

### TEST 12: Operator Management
**Purpose:** Verify addOperator() and removeOperator() work correctly.

| Test | Description | Expected |
|------|-------------|----------|
| 12.1a | Bob not initially operator | isOperator() = false |
| 12.1b | After addOperator(Bob) | isOperator() = true |
| 12.2 | addOperator for non-member | Does nothing |
| 12.3a | Two operators | Both isOperator() = true |
| 12.3b | removeOperator(Bob) | Bob not operator |
| 12.4 | Cannot remove last operator | Stays operator |
| 12.5 | removeOperator for non-op | No crash |

---

### TEST 13: Topic Restricted Mode (+t)
**Purpose:** Verify setTopicRestricted() affects topic setting.

| Test | Description | Expected |
|------|-------------|----------|
| 13.1a | Default +t, operator can set | Topic changes |
| 13.1b | Default +t, non-op cannot | Topic unchanged |
| 13.2 | With -t, anyone can set | Topic changes |

---

### TEST 14: getMembers() Function
**Purpose:** Verify getMembers() returns correct set.

| Test | Description | Expected |
|------|-------------|----------|
| 14.1a | Returns correct size | size() = 2 |
| 14.1b | Alice in set | find() != end() |
| 14.1c | Bob in set | find() != end() |
| 14.2 | Empty channel | Empty set |

---

### TEST 15: getMembersCount() Function
**Purpose:** Verify getMembersCount() returns correct count.

| Test | Description | Expected |
|------|-------------|----------|
| 15.1 | Initial count | 0 |
| 15.2 | After each join | Increments correctly |

---

### TEST 16: isMember() and isOperator() Functions
**Purpose:** Verify membership and operator status checking.

| Test | Description | Expected |
|------|-------------|----------|
| 16.1a | Before join | isMember() = false |
| 16.1b | After join | isMember() = true |
| 16.2a | First member | isOperator() = true |
| 16.2b | Second member | isOperator() = false |

---

### TEST 17: clearAllSet() Function
**Purpose:** Verify clearAllSet() clears all channel data.

| Test | Description | Expected |
|------|-------------|----------|
| 17.1a | After clear | No members |
| 17.1b | After clear | No key |
| 17.1c | After clear | Topic empty |

---

### TEST 18: Edge Cases
**Purpose:** Test unusual but valid scenarios.

| Test | Description | Expected |
|------|-------------|----------|
| 18.1 | Same client joins twice | Count stays 1 |
| 18.2 | Special chars in channel name | Accepted |
| 18.3 | Very long topic (1000 chars) | Accepted |
| 18.4 | Very long key (500 chars) | Works correctly |

---

### TEST 19: Memory Management
**Purpose:** Verify no memory leaks or crashes.

| Test | Description | Expected |
|------|-------------|----------|
| 19.1 | 100 channel cycles | No crash |
| 19.2 | Many join/leave operations | No crash |

**Valgrind Command:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./test_channel 19
```

---

### TEST 20: Mode Combinations
**Purpose:** Verify multiple modes can be combined.

| Test | Description | Expected |
|------|-------------|----------|
| 20.1 | +i and +k together | Both work |
| 20.2 | +i and +l together | Both work |
| 20.3 | All modes at once | All work |

---

## 🔄 Function Dependencies

```
addMember()
├── isChannelFull()
├── addOperator() (if first member)
├── broadcast()
└── invitedUsers.erase() (single-use invitation)

removeMember()
├── isMember()
├── broadcast()
└── server->removeChannel() (if empty)

kickMember()
├── isOperator()
├── isMember()
├── broadcast()
└── removeMember()

setTopic()
├── isOperator() (if +t)
└── broadcast()

addOperator()
├── isMember()
└── broadcast()

removeOperator()
├── isOperator()
├── operators.size() (prevent removing last)
└── broadcast()

setInviteOnly()
├── invitedUsers.clear() (if disabling)
└── broadcast()

setTopicRestricted()
└── broadcast()

setUserLimit()
└── broadcast()
```

---

## 🧪 Test Coverage Checklist

### Core Features
- [x] Channel creation with valid name
- [x] First member becomes operator
- [x] Members can join and leave
- [x] Operators can modify topic (+t)
- [x] Non-operators cannot modify topic (+t)
- [x] Channel key blocks unauthorized joins
- [x] Invite-only mode works
- [x] User limit prevents excessive joins
- [x] Kick removes member correctly
- [x] Broadcast sends to all except excluded

### Edge Cases
- [x] NULL client handling
- [x] Double join same client
- [x] Remove non-member
- [x] Kick non-member
- [x] Cannot remove last operator
- [x] Lowering limit doesn't kick existing members
- [x] Invitation is single-use

### Memory
- [x] Constructor/Destructor cycles
- [x] Many operations without leaks

---

## 👥 Contributors

| Name | Tests Assigned |
|------|----------------|
| NERO | Channel.cpp, Server.cpp |
| BAHOLY | MessageParser.cpp |
| YASSER | Client.cpp, Command.cpp |

---

## 📅 Test Log

| Date | Tester | Tests Run | Passed | Failed | Notes |
|------|--------|-----------|--------|--------|-------|
| 2026-01-22 | - | - | - | - | Test suite created |

---

## 🐛 Known Issues

1. **`isMember(NULL)` / `isOperator(NULL)`**: May crash if not handled. Test skipped.
2. **Broadcast verification**: Cannot verify message content without mocking `sendMessage()`.
3. **Double broadcast on kick**: `kickMember()` calls `removeMember()` which also broadcasts.

---

## 📊 Quick Reference

### Channel Modes

| Mode | Function | Flag |
|------|----------|------|
| +i | setInviteOnly(true) | inviteOnly |
| -i | setInviteOnly(false) | inviteOnly |
| +t | setTopicRestricted(true) | topicRestricted |
| -t | setTopicRestricted(false) | topicRestricted |
| +k | setKey("key") | key |
| -k | setKey("") | key |
| +l | setUserLimit(n) | userLimit |
| -l | setUserLimit(0) | userLimit |
| +o | addOperator(client) | operators set |
| -o | removeOperator(client) | operators set |

### Error Codes Used

| Code | Name | When |
|------|------|------|
| 441 | ERR_USERNOTINCHANNEL | kickMember: target not in channel |
| 471 | ERR_CHANNELISFULL | addMember: channel full (+l) |
| 482 | ERR_CHANOPRIVSNEEDED | setTopic/kick: not operator |

---

> **Remember:** Run `valgrind` on all tests before merging to main!
