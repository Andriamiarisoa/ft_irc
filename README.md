*This project has been created as part of the 42 curriculum by Herrakot, Yandriam and Braelino.*

# ft_irc - Internet Relay Chat Server

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Architecture](#architecture)
- [Instructions](#instructions)
  - [Requirements](#requirements)
  - [Compilation](#compilation)
  - [Execution](#execution)
  - [Usage Examples](#usage-examples)
- [Implemented Commands](#implemented-commands)
- [Technical Choices](#technical-choices)
- [Resources](#resources)
- [AI Usage](#ai-usage)

---

## Description

**ft_irc** is a custom IRC (Internet Relay Chat) server implementation built entirely in **C++98**. This project aims to provide a functional IRC server that complies with the IRC protocol standards defined in RFC 1459.

The server allows multiple clients to connect simultaneously, authenticate, join channels, send private messages, and manage channels through operator commands. The implementation uses non-blocking I/O with `select()` for efficient handling of multiple client connections in a single-threaded environment.

### Goal

Create a fully functional IRC server capable of:
- Handling multiple simultaneous client connections
- Supporting standard IRC authentication and registration
- Managing channels with operator privileges
- Implementing core IRC commands for communication and channel management

---

## Features

### Connection & Authentication
- Password-protected server access
- Nickname and username registration
- Welcome messages following IRC protocol (RPL_WELCOME, etc.)

### Channel Operations
- Create and join channels automatically
- Leave channels with optional reason
- First user to join becomes channel operator
- Channel topic management

### Messaging
- Private messages between users (PRIVMSG)
- Channel-wide messages
- NOTICE command for notifications

### Channel Operator Commands
- **KICK** - Eject a client from the channel
- **INVITE** - Invite a client to a channel
- **TOPIC** - Change or view the channel topic
- **MODE** - Change the channel's mode:
  - `i` : Set/remove Invite-only channel
  - `t` : Set/remove the restrictions of the TOPIC command to channel operators
  - `k` : Set/remove the channel key (password)
  - `o` : Give/take channel operator privilege
  - `l` : Set/remove the user limit to channel

### Keep-Alive
- PING/PONG implementation for connection maintenance

---

## Architecture

The project follows the **Command Design Pattern** for handling IRC commands. This architecture separates each command into its own class, providing:
- Clean and maintainable code structure
- Easy addition of new commands
- Independent testing of each command
- Reusable code components

### Main Components

| Class | Description |
|-------|-------------|
| `Server` | Main server class handling socket operations, `select()` multiplexing, and client management |
| `Client` | Represents a connected client with authentication state, nickname, and associated channels |
| `Channel` | Manages channel properties, members, operators, and modes |
| `Command` | Abstract base class for all IRC commands |
| `MessageParser` | Parses incoming IRC messages into command components |

### Command Classes
- `PassCommand`, `NickCommand`, `UserCommand` - Authentication
- `JoinCommand`, `PartCommand`, `QuitCommand` - Connection management
- `PrivmsgCommand`, `NoticeCommand` - Messaging
- `KickCommand`, `InviteCommand`, `TopicCommand`, `ModeCommand` - Channel operations
- `PingCommand`, `PongCommand` - Keep-alive

---

## Instructions

### Requirements

- **Compiler**: `c++` with C++98 standard support
- **Operating System**: Linux or macOS
- **Dependencies**: None (no external libraries)

### Compilation

```bash
# Clone the repository
git clone <repository-url>
cd ft_irc

# Build the project
make

# Other Makefile rules
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Rebuild the project
```

### Execution

```bash
./ircserv <port> <password>
```

- **port**: The port number on which the IRC server will listen for incoming connections (e.g., 6667)
- **password**: The connection password required by clients to connect

**Example:**
```bash
./ircserv 6667 mypassword
```

### Usage Examples

#### Connecting with netcat
```bash
nc -C 127.0.0.1 6667
```

#### Basic IRC Session
```
PASS mypassword
NICK mynickname
USER myuser 0 * :My Real Name
JOIN #mychannel
PRIVMSG #mychannel :Hello everyone!
PRIVMSG othernick :Private message
QUIT :Goodbye!
```

#### Testing partial data (as per subject requirements)
```bash
$> nc -C 127.0.0.1 6667
com^Dman^Dd
$>
```
Use `ctrl+D` to send the command in several parts: 'com', then 'man', then 'd\n'.

#### Connecting with an IRC Client
You can use any IRC client but the project is recommended for NetCat


---

## Implemented Commands

| Command | Description | Syntax |
|---------|-------------|--------|
| PASS | Set connection password | `PASS <password>` |
| NICK | Set/change nickname | `NICK <nickname>` |
| USER | User registration | `USER <username> <mode> <unused> :<realname>` |
| JOIN | Join a channel | `JOIN <#channel> [key]` |
| PART | Leave a channel | `PART <#channel> [:<reason>]` |
| PRIVMSG | Send message | `PRIVMSG <target> :<message>` |
| NOTICE | Send notice | `NOTICE <target> :<message>` |
| KICK | Kick user from channel | `KICK <#channel> <nick> [:<reason>]` |
| INVITE | Invite user to channel | `INVITE <nick> <#channel>` |
| TOPIC | Set/view channel topic | `TOPIC <#channel> [:<topic>]` |
| MODE | Set channel modes | `MODE <#channel> <+/-modes> [params]` |
| PING | Ping server | `PING <token>` |
| PONG | Respond to ping | `PONG <token>` |
| QUIT | Disconnect from server | `QUIT [:<reason>]` |

---

## Technical Choices

### I/O Multiplexing: select()
We chose `select()` for I/O multiplexing because:
- Well-documented and widely supported
- Sufficient for the project's requirements
- Compatible with both Linux and macOS
- Simpler to understand and implement for learning purposes

### Non-blocking I/O
All file descriptors are set to non-blocking mode

### Message Buffering
The server handles partial message reception by:
- Accumulating data in per-client buffers
- Processing complete commands only when `\r\n` is received
- Properly handling low bandwidth scenarios

### Design Patterns
- **Command Pattern**: Each IRC command is encapsulated in its own class
- **Singleton considerations**: Server manages single instances of global resources

---

## Resources

### Official Documentation
- **[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)** - Comprehensive guide to socket programming
- **[RFC 1459: Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html#section-4.1.2)** - Official IRC protocol specification
- **[RFC 2812: Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc2812.html#section-2.3.1)** - Official IRC protocol specification
- **[Linux man pages for select()](https://man7.org/linux/man-pages/man2/select.2.html)** - System call documentation


### Learning Resources
- **YouTube tutorials** - Various video resources for socket programming and IRC concepts
- **Peer-to-peer learning** - Collaboration with other 42 students

---

## AI Usage

### Tools Used

| AI Tool | Purpose |
|---------|---------|
| **Claude Opus 4.5 (via GitHub Copilot)** | Generating unit tests and comprehensive manual test procedures |
| **GPT-5** | Understanding and summarizing concepts needed for the project (socket programming, IRC protocol, etc.) |
| **Claude Sonnet 4.5** | Deep understanding of concepts with clean examples, graphics, and use cases |
| **Google Gemini** | Quick research and fact-checking |

### How AI Was Used

1. **Test Generation**: AI was used to generate unit tests for individual components (Client, Channel, Server, Commands) and comprehensive manual test scenarios to ensure proper functionality.

2. **Concept Understanding**: Complex topics like the IRC protocol, socket programming, `select()` multiplexing, and design patterns were clarified using AI explanations and summaries.

3. **Code Examples**: AI provided clean code examples demonstrating proper implementation patterns for socket operations, message parsing, and command handling.

4. **Documentation**: AI assisted in creating internal documentation files explaining various concepts and tutorials.

5. **Debugging Assistance**: AI helped identify potential issues and suggest solutions during development.

### Human Verification

All AI-generated content was:
- Reviewed and validated by team members
- Tested against actual IRC client behavior
- Cross-referenced with official documentation (RFC 1459, man pages)
- Peer-reviewed with other 42 students

---

## Project Structure

```
ft_irc/
├── main.cpp                 # Entry point
├── Makefile                 # Build configuration
├── README.md                # This file
├── includes/                # Header files
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Command.hpp
│   ├── Replies.hpp          # IRC numeric replies
│   └── *Command.hpp         # Individual command headers
└── src/                     # Source files
    ├── Server.cpp
    ├── Client.cpp
    ├── Channel.cpp
    ├── Command.cpp
    ├── MessageParser.cpp
    └── *Command.cpp         # Individual command implementations

```

---

## Authors

- **Herrakot**
- **Yandriam**
- **Braelino**

---

## License

This project is part of the 42 curriculum and is intended for educational purposes.
