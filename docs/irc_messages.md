# IRC Messages Cheat Sheet

Reference guide for IRC message formats following RFC 1459.

---

## 1. Connection & Registration

### PASS (Password)
```
Client → Server: PASS <password>
```

### NICK (Nickname)
```
Client → Server: NICK <nickname>
Server → Client: :<oldnick>!<user>@<host> NICK <newnick>
```

### USER (User registration)
```
Client → Server: USER <username> <mode> <unused> :<realname>
```

### Welcome Messages (001-004)
```
:server 001 <nick> :Welcome to the IRC Network <nick>!<user>@<host>
:server 002 <nick> :Your host is <server>, running version <version>
:server 003 <nick> :This server was created <date>
:server 004 <nick> <server> <version> <user_modes> <chan_modes>
```

---

## 2. Channel Operations

### JOIN
```
Client → Server: JOIN <#channel> [key]
Server → All:    :<nick>!<user>@<host> JOIN <#channel>
```

**After JOIN, server sends:**
```
:server 332 <nick> <#channel> :<topic>           (RPL_TOPIC - if topic exists)
:server 331 <nick> <#channel> :No topic is set   (RPL_NOTOPIC - if no topic)
:server 353 <nick> = <#channel> :[@]<nick1> [<nick2>...]  (RPL_NAMREPLY)
:server 366 <nick> <#channel> :End of /NAMES list        (RPL_ENDOFNAMES)
```

### PART
```
Client → Server: PART <#channel> [:<reason>]
Server → All:    :<nick>!<user>@<host> PART <#channel> :<reason>
```

### QUIT
```
Client → Server: QUIT [:<reason>]
Server → All:    :<nick>!<user>@<host> QUIT :<reason>
```

### KICK
```
Client → Server: KICK <#channel> <nick> [:<reason>]
Server → All:    :<nick>!<user>@<host> KICK <#channel> <target> :<reason>
```

### INVITE
```
Client → Server: INVITE <nick> <#channel>
Server → Target: :<nick>!<user>@<host> INVITE <target> <#channel>
Server → Sender: :server 341 <nick> <target> <#channel>  (RPL_INVITING)
```

---

## 3. Messaging

### PRIVMSG (Private message / Channel message)
```
Client → Server:  PRIVMSG <target> :<message>
Server → Target:  :<nick>!<user>@<host> PRIVMSG <target> :<message>
```

- `<target>` = `#channel` for channel messages
- `<target>` = `nickname` for private messages

### NOTICE
```
Client → Server:  NOTICE <target> :<message>
Server → Target:  :<nick>!<user>@<host> NOTICE <target> :<message>
```

---

## 4. Channel Management

### TOPIC
```
Client → Server: TOPIC <#channel>                (query topic)
Client → Server: TOPIC <#channel> :<new_topic>   (set topic)
Server → All:    :<nick>!<user>@<host> TOPIC <#channel> :<new_topic>
```

### MODE (Channel modes)
```
Client → Server: MODE <#channel> <+/-modes> [params]
Server → All:    :<nick>!<user>@<host> MODE <#channel> <+/-modes> [params]
```

**Channel Mode Examples:**
```
MODE #channel +o nick    → Give operator status
MODE #channel -o nick    → Remove operator status
MODE #channel +i         → Set invite-only
MODE #channel -i         → Remove invite-only
MODE #channel +t         → Only ops can change topic
MODE #channel -t         → Anyone can change topic
MODE #channel +k secret  → Set channel key
MODE #channel -k         → Remove channel key
MODE #channel +l 50      → Set user limit
MODE #channel -l         → Remove user limit
```

### MODE (User modes)
```
Client → Server: MODE <nick> <+/-modes>
Server → Client: :<nick>!<user>@<host> MODE <nick> <+/-modes>
```

---

## 5. Common Numeric Replies

### Success Codes (2xx-3xx)
| Code | Name | Format |
|------|------|--------|
| 001 | RPL_WELCOME | `:server 001 <nick> :Welcome...` |
| 221 | RPL_UMODEIS | `:server 221 <nick> <modes>` |
| 324 | RPL_CHANNELMODEIS | `:server 324 <nick> <#chan> <modes> [params]` |
| 331 | RPL_NOTOPIC | `:server 331 <nick> <#chan> :No topic is set` |
| 332 | RPL_TOPIC | `:server 332 <nick> <#chan> :<topic>` |
| 341 | RPL_INVITING | `:server 341 <nick> <target> <#chan>` |
| 353 | RPL_NAMREPLY | `:server 353 <nick> = <#chan> :<names>` |
| 366 | RPL_ENDOFNAMES | `:server 366 <nick> <#chan> :End of /NAMES list` |

### Error Codes (4xx)
| Code | Name | Format |
|------|------|--------|
| 401 | ERR_NOSUCHNICK | `:server 401 <nick> <target> :No such nick/channel` |
| 403 | ERR_NOSUCHCHANNEL | `:server 403 <nick> <#chan> :No such channel` |
| 404 | ERR_CANNOTSENDTOCHAN | `:server 404 <nick> <#chan> :Cannot send to channel` |
| 405 | ERR_TOOMANYCHANNELS | `:server 405 <nick> <#chan> :You have joined too many channels` |
| 411 | ERR_NORECIPIENT | `:server 411 <nick> :No recipient given (<cmd>)` |
| 412 | ERR_NOTEXTTOSEND | `:server 412 <nick> :No text to send` |
| 421 | ERR_UNKNOWNCOMMAND | `:server 421 <nick> <cmd> :Unknown command` |
| 431 | ERR_NONICKNAMEGIVEN | `:server 431 <nick> :No nickname given` |
| 432 | ERR_ERRONEUSNICKNAME | `:server 432 <nick> <badnick> :Erroneous nickname` |
| 433 | ERR_NICKNAMEINUSE | `:server 433 * <nick> :Nickname is already in use` |
| 441 | ERR_USERNOTINCHANNEL | `:server 441 <nick> <target> <#chan> :They aren't on that channel` |
| 442 | ERR_NOTONCHANNEL | `:server 442 <nick> <#chan> :You're not on that channel` |
| 443 | ERR_USERONCHANNEL | `:server 443 <nick> <target> <#chan> :is already on channel` |
| 461 | ERR_NEEDMOREPARAMS | `:server 461 <nick> <cmd> :Not enough parameters` |
| 462 | ERR_ALREADYREGISTERED | `:server 462 <nick> :You may not reregister` |
| 464 | ERR_PASSWDMISMATCH | `:server 464 <nick> :Password incorrect` |
| 471 | ERR_CHANNELISFULL | `:server 471 <nick> <#chan> :Cannot join channel (+l)` |
| 473 | ERR_INVITEONLYCHAN | `:server 473 <nick> <#chan> :Cannot join channel (+i)` |
| 474 | ERR_BANNEDFROMCHAN | `:server 474 <nick> <#chan> :Cannot join channel (+b)` |
| 475 | ERR_BADCHANNELKEY | `:server 475 <nick> <#chan> :Cannot join channel (+k)` |
| 476 | ERR_BADCHANMASK | `:server 476 <nick> <#chan> :Bad Channel Mask` |
| 482 | ERR_CHANOPRIVSNEEDED | `:server 482 <nick> <#chan> :You're not channel operator` |

---

## 6. Message Structure

### General Format
```
:<prefix> <command> <params> :<trailing>
```

- **Prefix**: `nick!user@host` (origin of message)
- **Command**: IRC command or numeric code
- **Params**: Space-separated parameters
- **Trailing**: Everything after `:` (can contain spaces)

### Examples
```
:nick!user@host PRIVMSG #channel :Hello everyone!
:server 433 * nickname :Nickname is already in use
:nick!user@host JOIN #channel
:nick!user@host QUIT :Leaving
```

---

## 7. Prefix Format

```
:<nickname>!<username>@<hostname>
```

### Example
```
:john!~john@192.168.1.1 PRIVMSG #general :Hello!
```

| Part | Example | Description |
|------|---------|-------------|
| nickname | john | User's display name |
| username | ~john | System username (~ = ident not verified) |
| hostname | 192.168.1.1 | User's host/IP |

---

## 8. NAMES Reply Prefixes

In RPL_NAMREPLY (353), user prefixes indicate status:

| Prefix | Meaning |
|--------|---------|
| `@` | Channel operator |
| `+` | Voice (can speak in moderated channel) |
| (none) | Regular user |

### Example
```
:server 353 nick = #channel :@operator +voiced regular
```

---

## Quick Reference Table

| Action | Client Sends | Server Broadcasts |
|--------|--------------|-------------------|
| Join channel | `JOIN #chan` | `:nick!u@h JOIN #chan` |
| Leave channel | `PART #chan :bye` | `:nick!u@h PART #chan :bye` |
| Quit server | `QUIT :bye` | `:nick!u@h QUIT :bye` |
| Send message | `PRIVMSG #chan :hi` | `:nick!u@h PRIVMSG #chan :hi` |
| Send notice | `NOTICE #chan :hi` | `:nick!u@h NOTICE #chan :hi` |
| Set topic | `TOPIC #chan :new` | `:nick!u@h TOPIC #chan :new` |
| Kick user | `KICK #chan bob` | `:nick!u@h KICK #chan bob` |
| Invite user | `INVITE bob #chan` | `:nick!u@h INVITE bob #chan` |
| Give op | `MODE #chan +o bob` | `:nick!u@h MODE #chan +o bob` |
| Take op | `MODE #chan -o bob` | `:nick!u@h MODE #chan -o bob` |
| Keep-alive | `PING :token` | `:server PONG server :token` |

---

## 9. État d'Implémentation

| Commande | Fichier | État |
|----------|---------|------|
| PASS | `PassCommand.cpp` | ✅ Implémenté |
| NICK | `NickCommand.cpp` | ✅ Implémenté |
| USER | `UserCommand.cpp` | ✅ Implémenté |
| JOIN | `JoinCommand.cpp` | ✅ Implémenté |
| PART | `PartCommand.cpp` | ✅ Implémenté |
| QUIT | `QuitCommand.cpp` | ✅ Implémenté |
| KICK | `KickCommand.cpp` | ✅ Implémenté |
| INVITE | `InviteCommand.cpp` | ✅ Implémenté |
| TOPIC | `TopicCommand.cpp` | ✅ Implémenté |
| MODE | `ModeCommand.cpp` | ✅ Implémenté |
| PRIVMSG | `PrivmsgCommand.cpp` | ✅ Implémenté |
| NOTICE | `NoticeCommand.cpp` | ✅ Nouveau (22/01) |
| PING | `PingCommand.cpp` | ✅ Nouveau (22/01) |
| PONG | `PongCommand.cpp` | ✅ Nouveau (22/01) |

### Fichiers Utilitaires Ajoutés

| Fichier | Description |
|---------|-------------|
| `Replies.hpp` | Macros pour codes numériques IRC (001-4xx) |
| `Client::getPrefix()` | Méthode pour générer `:nick!user@host` |
