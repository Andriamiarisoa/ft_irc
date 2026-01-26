# IRC Server Manual Test Checklist

## How to Test

### Start server with valgrind:
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ircserv 6667 password
```

### Connect client:
```bash
nc localhost 6667
# or
irssi -c localhost -p 6667
```

---

## Legend
- ✅ = Pass
- ❌ = Fail
- ⏳ = Not tested

| Status | Test | Problem (if any) |
|--------|------|------------------|

---

## 1. CONNECTION & AUTHENTICATION

### PASS Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `PASS password` - correct password | |
|✅| `PASS wrongpass` - incorrect password (should reject) | |
|✅| `PASS` - no parameter (should error) | |
|✅| `PASS pass1 pass2` - extra parameters (should use first) | |
|✅| Send commands before PASS (should require auth) | |
|✅| Send PASS twice (should error: already registered) |  |

### NICK Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `NICK validnick` - valid nickname | |
|✅| `NICK 123invalid` - starts with number (should reject) | |
|✅| `NICK` - no parameter (should error) | |
|✅| `NICK nick!invalid` - invalid character (should reject) | |
|✅| `NICK existingnick` - nickname already in use (should reject) | |
|✅| `NICK newnick` - change nickname while connected | |
|✅| `NICK verylongnicknameover9chars` - too long nickname | |
|✅| `NICK a` - single character nickname | |

### USER Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `USER username 0 * :Real Name` - valid registration | |
|✅| `USER` - no parameters (should error) | |
|✅| `USER user` - missing parameters (should error) | |
|✅| `USER user 0 * :` - empty realname | Yes empty real name should work in a classic IRC |
|✅| Send USER twice (should error: already registered) | |
|✅| USER before PASS (should require auth first) | |
|✅| USER before NICK (should work, wait for NICK) | |

### Full Registration Flow
| Status | Test | Problem |
|--------|------|---------|
|✅| PASS → NICK → USER (correct order) | |
|✅| PASS → USER → NICK (different order) | |
|✅| Connect without completing registration, then disconnect | |

---

## 2. CHANNEL OPERATIONS

### JOIN Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `JOIN #channel` - join new channel (become operator) | |
|✅| `JOIN #channel` - join existing channel | |
|✅| `JOIN #channel key` - join with correct key | |
|✅| `JOIN #channel wrongkey` - join with wrong key (should reject) | |
|✅| `JOIN #channel` - join channel with key without providing key | |
|✅| `JOIN invalidchannel` - no # prefix (should reject) | |
|✅| `JOIN` - no parameter (should error) | |
|✅| `JOIN #chan1,#chan2` - join multiple channels | |
|✅| `JOIN #channel` - join invite-only without invite (should reject) | |
|✅| `JOIN #channel` - join invite-only with invite | |
|✅| `JOIN #channel` - join full channel (+l limit reached) | |
|✅| `JOIN #channel` when already in channel | Normal IRC does not send any error message for this case |
|✅| `JOIN 0` - leave all channels | |

### PART Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `PART #channel` - leave channel | |
|✅| `PART #channel :goodbye` - leave with message | |
|✅| `PART #nonexistent` - part channel not in (should error) | |
|✅| `PART` - no parameter (should error) | |
|✅| `PART #chan1,#chan2` - part multiple channels | |
|✅| PART last member (channel should be deleted) | |


### TOPIC Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `TOPIC #channel` - view topic | |
|✅| `TOPIC #channel :new topic` - set topic as operator | |
|✅| `TOPIC #channel :new topic` - set topic as non-op (no +t) | |
|✅| `TOPIC #channel :new topic` - set topic as non-op (+t set, should reject) | |
|✅| `TOPIC #channel :` - clear topic | |
|✅| `TOPIC #nonexistent` - channel doesn't exist (should error) | |
|✅| `TOPIC #channel` - not in channel (should error) | |

### KICK Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `KICK #channel nick` - kick user as operator | |
|✅| `KICK #channel nick :reason` - kick with reason | |
|✅| `KICK #channel nick` - kick as non-operator (should reject) | |
|✅| `KICK #channel nonexistent` - kick non-member (should error) | |
|✅| `KICK #nonexistent nick` - channel doesn't exist (should error) | |
|✅| `KICK #channel` - missing nick parameter (should error) | |
|✅| `KICK` - no parameters (should error) | |
|✅| Kick last other member (you remain alone) | |
|✅| Self-kick attempt | |


### INVITE Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `INVITE nick #channel` - invite user as operator | |
|✅| `INVITE nick #channel` - invite as non-op (+i not set) | |
|✅| `INVITE nick #channel` - invite as non-op (+i set, should reject) | |
|✅| `INVITE nick #channel` - invite already-member (should error) | |
|✅| `INVITE nonexistent #channel` - invite non-existent user (should error) | |
|✅| `INVITE nick #nonexistent` - channel doesn't exist (should error) | |
|✅| `INVITE` - no parameters (should error) | |
|✅| Invited user can join invite-only channel | |

---

## 3. MODE COMMAND

### Channel Modes
| Status | Test | Problem |
|--------|------|---------|
|✅| `MODE #channel` - view current modes | |
|✅| `MODE #channel +i` - set invite-only | |
|✅| `MODE #channel -i` - remove invite-only | |
|✅| `MODE #channel +t` - set topic restricted | |
|✅| `MODE #channel -t` - remove topic restricted | |
|✅| `MODE #channel +k secretkey` - set channel key | |
|✅| `MODE #channel -k` - remove channel key | |
|✅| `MODE #channel +o nick` - give operator status | |
|✅| `MODE #channel -o nick` - remove operator status | |
|✅| `MODE #channel +l 10` - set user limit | |
|✅| `MODE #channel -l` - remove user limit | |
|✅| `MODE #channel +itk key` - multiple modes at once | |
|✅| `MODE #channel +o nonexistent` - op non-member (should error) | |
|✅| MODE as non-operator (should reject) | |
|✅| `MODE #nonexistent +i` - channel doesn't exist | |

### Edge Cases
| Status | Test | Problem |
|--------|------|---------|
|✅| `MODE #channel +k` - key mode without key param | |
|✅| `MODE #channel +l` - limit mode without number | |
|✅| `MODE #channel +l abc` - limit with non-numeric | |
|✅| `MODE #channel +l -5` - limit with negative number | |
|✅| `MODE #channel +oo nick1 nick2` - op multiple users | |
|✅| Remove last operator from channel | |

---

## 4. MESSAGING

### PRIVMSG Command
| Status | Test | Problem |
|--------|------|---------|
|✅| `PRIVMSG nick :hello` - private message to user | |
|✅| `PRIVMSG #channel :hello` - message to channel | |
|✅| `PRIVMSG nick` - no message (should error) | |
|✅| `PRIVMSG` - no parameters (should error) | |
|✅| `PRIVMSG nonexistent :hello` - message to non-existent user | |
|✅| `PRIVMSG #nonexistent :hello` - message to non-existent channel | |
|✅| `PRIVMSG #channel :hello` - message to channel not joined | |
|✅| Message with special characters: `:`, `!`, `@` | |
|✅| Empty message `PRIVMSG nick :` | |
|✅| Very long message (>512 chars) | |

### NOTICE Command (if implemented)
| Status | Test | Problem |
|--------|------|---------|
|✅| `NOTICE nick :hello` - notice to user | |
|✅| `NOTICE #channel :hello` - notice to channel | Should call an "you're not in channel error" |

---

## 5. PING/PONG

| Status | Test | Problem |
|--------|------|---------|
|✅| `PING server` - should receive PONG | |
|✅| `PING` - no parameter | |
|✅| `PING :with trailing` - with trailing parameter | |
|| Server sends PING, client responds PONG | Not implemented, not mandatory |

---

## 6. QUIT & DISCONNECTION

| Status | Test | Problem |
|--------|------|---------|
|❌| `QUIT` - clean disconnect | invalid read of size 8 |
|❌| `QUIT :goodbye message` - quit with message | invalid read of size 8 |
|✅| Ctrl+C client while in channel | |
|✅| Ctrl+C client while in channel (last member) | |
|❌| Ctrl+C client while in multiple channels | massive leaks and segfault |
|✅| Close nc connection abruptly | |
| | Client timeout (if implemented) | Not implemented |

---

## 7. MULTI-CLIENT SCENARIOS

| Status | Test | Problem |
|--------|------|---------|
|✅| Two clients join same channel | |
|✅| Client A messages Client B in channel | |
|✅| Client A kicks Client B | |
|✅| Client A changes topic, Client B sees it | Topic on client B become topic without the topic[0]|
|✅| Client A quits, Client B receives quit message | massive leaks on quit command |
|✅| Multiple clients join, one becomes op (first joiner) | |
|✅| Transfer operator status between clients | |
|✅| 10+ clients in same channel | |

---

## 8. MEMORY & STABILITY (VALGRIND)

| Status | Test | Problem |
|--------|------|---------|
|✅| Start server, stop immediately - no leaks | |
|✅| One client connects, disconnects - no leaks | |
|❌| Client joins channel, parts - no leaks | Invalid read of size 8 |
|❌| Client joins channel, quits - no leaks | Invalid read of size 8 |
|❌| Last client leaves channel (channel deleted) - no leaks | channel is deleted but again the invalid read of size from part |
|✅| Multiple clients connect/disconnect rapidly - no leaks | |
|✅| Client kicked from channel - no leaks | |
|✅| Ctrl+C while last in channel - no crash | |
|❌| QUIT while last in channel - no crash | invalid read of size 8 |
|❌| Invalid commands spam - no crash | When I spam a command and after write a valid one it sometime bug |
|✅| Partial commands (incomplete \r\n) - no crash | |

---

## 9. EDGE CASES & STRESS

| Status | Test | Problem |
|--------|------|---------|
|✅| Send empty line | |
|✅| Send only `\r\n` | |
|✅| Send command without `\r\n` (partial) | |
|✅| Send very long line (>512 chars) | |
|✅| Send binary data | |
|✅| Send UTF-8 characters | |
|✅| Rapid connect/disconnect | |
|✅| Send commands before registration complete | |
|✅| Case sensitivity: `JOIN` vs `join` vs `Join` | |
|✅| Channel name case: `#Channel` vs `#channel` | |
|✅| Nickname case sensitivity | |

---

## 10. IRSSI/REAL CLIENT COMPATIBILITY

| Status | Test | Problem |
|--------|------|---------|
| | Connect with irssi | |
| | Register with irssi (auto PASS/NICK/USER) | |
| | Join channel with irssi | |
| | Send messages with irssi | |
| | /kick /ban /mode commands from irssi | |
| | /quit from irssi | |
| | Connect with HexChat (if available) | |
| | Connect with WeeChat (if available) | |

---

## Summary

| Category | Passed | Failed | Not Tested |
|----------|--------|--------|------------|
| Connection & Auth | | | |
| Channel Operations | | | |
| Mode Command | | | |
| Messaging | | | |
| Ping/Pong | | | |
| Quit & Disconnect | | | |
| Multi-Client | | | |
| Memory (Valgrind) | | | |
| Edge Cases | | | |
| Real Client | | | |
| **TOTAL** | | | |

---

## Notes

_Write any additional notes or recurring issues here:_

