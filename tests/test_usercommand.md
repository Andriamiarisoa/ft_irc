# UserCommand Standalone Test Suite - README

## 📋 Overview

This document explains the **standalone** `test_usercommand_standalone.cpp` test suite, which provides comprehensive unit testing for the `UserCommand` class in the ft_irc project. 

**Key Feature**: This test file includes **mock implementations** of `Server`, `Client`, and `Command` classes, allowing you to test `UserCommand` **independently** without needing the other source files to be complete.

---

## 🏗️ Architecture

### File Structure
```
tests/
├── test_usercommand_standalone.cpp   # Self-contained test file (USE THIS)
├── test_usercommand.cpp              # Full integration test (needs all sources)
└── test_usercommand.md               # This documentation
```

### What's Included in the Standalone File
- ✅ Mock `Server` class
- ✅ Mock `Client` class  
- ✅ Mock `Command` base class
- ✅ Copy of `UserCommand` implementation
- ✅ All test utilities
- ✅ **83 comprehensive tests**

### No External Dependencies Required!
- ❌ No need for `Server.cpp`
- ❌ No need for `Client.cpp`
- ❌ No need for `Command.cpp`
- ❌ No need for `Channel.cpp`
- ❌ No need for `MessageParser.cpp`

---

## 🔧 Compilation

### Single Command (Recommended)
```bash
g++ -Wall -Wextra -Werror -std=c++98 tests/test_usercommand_standalone.cpp -o test_usercommand
```

### Running Tests
```bash
./test_usercommand
```

### One-liner
```bash
g++ -Wall -Wextra -Werror -std=c++98 tests/test_usercommand_standalone.cpp -o test_usercommand && ./test_usercommand
```

---

## 📊 Test Categories (83 Total Tests)

### 1. Constructor Tests (6 tests)
| Test Name | Description |
|-----------|-------------|
| `test_constructor_valid` | Verifies constructor with valid parameters |
| `test_constructor_null_server` | Expects exception when server is NULL |
| `test_constructor_null_client` | Expects exception when client is NULL |
| `test_constructor_null_both` | Expects exception when both are NULL |
| `test_constructor_empty_params` | Verifies empty params are accepted |
| `test_constructor_many_params` | Handles 100 parameters without crash |

### 2. Authentication Tests (5 tests)
| Test Name | Description |
|-----------|-------------|
| `test_user_without_pass` | USER without PASS returns error 464 |
| `test_user_without_pass_message_content` | Error message mentions PASS |
| `test_user_with_pass` | USER after PASS proceeds normally |
| `test_user_unauthenticated_with_nick` | NICK without PASS still fails with 464 |
| `test_user_unauthenticated_no_params` | Auth check happens before param check |

### 3. Parameter Validation Tests (10 tests)
| Test Name | Description |
|-----------|-------------|
| `test_user_no_params` | 0 params → error 461 |
| `test_user_one_param` | 1 param → error 461 |
| `test_user_two_params` | 2 params → error 461 |
| `test_user_three_params` | 3 params → error 461 |
| `test_user_four_params` | 4 params → accepted |
| `test_user_five_params` | 5 params → accepted |
| `test_user_ten_params` | 10 params → accepted |
| `test_user_insufficient_params_message` | Error mentions USER |
| `test_user_params_not_modified_on_error` | Client state unchanged on param error |

### 4. Re-registration Tests (5 tests)
| Test Name | Description |
|-----------|-------------|
| `test_user_already_registered` | Second USER returns error 462 |
| `test_user_already_registered_message` | Error mentions "reregister" |
| `test_user_does_not_change_when_registered` | Username unchanged on failure |
| `test_user_realname_not_changed_when_registered` | Realname unchanged on failure |
| `test_user_registered_uses_nick_in_error` | Error response uses nickname |

### 5. Username Setting Tests (7 tests)
| Test Name | Description |
|-----------|-------------|
| `test_user_sets_username` | Verifies username is set correctly |
| `test_user_username_special_chars` | Handles underscores, dashes |
| `test_user_username_with_numbers` | Handles numeric characters |
| `test_user_username_all_numbers` | Handles all-numeric username |
| `test_user_username_single_char` | Handles single character username |
| `test_user_username_uppercase` | Handles uppercase letters |
| `test_user_username_mixed_case` | Handles mixed case preservation |

### 6. Realname Setting Tests (7 tests)
| Test Name | Description |
|-----------|-------------|
| `test_user_sets_realname` | Verifies realname is set |
| `test_user_realname_with_spaces` | Handles multiple spaces |
| `test_user_realname_with_special_chars` | Handles parentheses, etc. |
| `test_user_realname_with_colon` | Handles colons in realname |
| `test_user_realname_with_numbers` | Handles numbers in realname |
| `test_user_realname_unicode_like` | Handles special characters |
| `test_user_realname_symbols` | Handles <, >, &, ! symbols |

### 7. Registration Completion Tests (5 tests)
| Test Name | Description |
|-----------|-------------|
| `test_registration_pass_nick_user` | PASS→NICK→USER completes registration |
| `test_registration_pass_user_only` | PASS→USER alone doesn't register |
| `test_user_sets_username_without_nick` | Username set even without NICK |
| `test_user_sets_realname_without_nick` | Realname set even without NICK |
| `test_user_empty_nick_no_registration` | Empty nickname prevents registration |

### 8. Welcome Message Tests (8 tests)
| Test Name | Description |
|-----------|-------------|
| `test_welcome_001_sent` | RPL_WELCOME (001) is sent |
| `test_welcome_contains_welcome_text` | Message contains "Welcome" |
| `test_welcome_contains_nickname` | Message contains nickname |
| `test_welcome_contains_irc_network` | Message mentions IRC Network |
| `test_no_welcome_without_nick` | No welcome if no NICK |
| `test_response_ends_with_crlf` | Response ends with \r\n |
| `test_welcome_single_response` | Only one response sent |
| `test_welcome_starts_with_colon` | Response starts with : prefix |

### 9. Error Precedence Tests (3 tests)
| Test Name | Description |
|-----------|-------------|
| `test_auth_checked_before_params` | Auth error before param error |
| `test_params_checked_before_registration` | Param error before re-registration error |
| `test_single_error_response` | Only one error sent at a time |

### 10. Multiple Commands Tests (3 tests)
| Test Name | Description |
|-----------|-------------|
| `test_multiple_user_commands` | Second USER fails with 462 |
| `test_first_username_preserved` | First username kept after failed re-registration |
| `test_three_user_commands` | Third USER also fails |

### 11. Edge Cases Tests (14 tests)
| Test Name | Description |
|-----------|-------------|
| `test_empty_username` | Empty username parameter |
| `test_empty_realname` | Empty realname parameter |
| `test_long_username` | 100-character username |
| `test_very_long_username` | 1000-character username |
| `test_long_realname` | 500-character realname |
| `test_mode_parameter_zero` | Mode = 0 (default) |
| `test_mode_parameter_four` | Mode = 4 (wallops) |
| `test_mode_parameter_eight` | Mode = 8 (invisible) |
| `test_mode_parameter_invalid` | Mode = 99 (invalid but ignored) |
| `test_mode_parameter_text` | Mode = "text" (ignored) |
| `test_unused_parameter_asterisk` | Unused = * (standard) |
| `test_unused_parameter_custom` | Unused = custom value |
| `test_unused_parameter_empty` | Unused = empty string |
| `test_whitespace_in_params` | Preserves leading/trailing spaces |

### 12. Destructor Tests (3 tests)
| Test Name | Description |
|-----------|-------------|
| `test_destructor` | Destructor cleanup |
| `test_polymorphic_destruction` | Delete through base pointer |
| `test_multiple_allocations` | 10 allocations/deletions |

### 13. Error Format Tests (5 tests)
| Test Name | Description |
|-----------|-------------|
| `test_error_461_format` | 461 format: starts with :, ends with CRLF |
| `test_error_462_format` | 462 format: starts with :, ends with CRLF |
| `test_error_464_format` | 464 format: starts with :, ends with CRLF |
| `test_error_uses_asterisk_when_unregistered` | Uses * for unregistered clients |
| `test_error_code_padding` | Error codes are 3 digits (zero-padded) |

### 14. Stress Tests (2 tests)
| Test Name | Description |
|-----------|-------------|
| `test_rapid_commands` | 50 rapid USER commands |
| `test_many_clients_simulation` | 10 simulated clients |

---

## 🎯 IRC Protocol Coverage

### USER Command Format
```
USER <username> <mode> <unused> :<realname>
```

### Parameters
| Index | Name | Description | Example |
|-------|------|-------------|---------|
| 0 | username | User's identity | `john` |
| 1 | mode | User mode flags | `0`, `4`, `8` |
| 2 | unused | Ignored parameter | `*` |
| 3 | realname | Full name (can have spaces) | `John Doe` |

### Error Codes Tested
| Code | Name | When Returned |
|------|------|---------------|
| 461 | ERR_NEEDMOREPARAMS | < 4 parameters |
| 462 | ERR_ALREADYREGISTRED | Client already registered |
| 464 | ERR_PASSWDMISMATCH | PASS not sent first |

### Success Codes Tested
| Code | Name | When Sent |
|------|------|-----------|
| 001 | RPL_WELCOME | On successful registration (PASS + NICK + USER) |

---

## 🔍 Understanding the Mock Classes

### Mock Server
```cpp
class Server {
    int port;
    std::string password;
public:
    Server(int p, const std::string& pass);
    int getPort() const;
    const std::string& getPassword() const;
};
```

### Mock Client
```cpp
class Client {
    int fd;
    std::string nickname, username, realname, hostname;
    bool authenticated, registered;
public:
    // Getters
    std::string getNickname() const;
    std::string getUsername() const;
    bool isAuthenticated() const;
    bool isRegistered() const;
    
    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void authenticate();
    void registerClient();
    
    // I/O
    void sendMessage(const std::string& msg);
};
```

### Mock Command Base
```cpp
class Command {
protected:
    Server* server;
    Client* client;
    std::vector<std::string> params;
    
    void sendReply(int code, const std::string& msg);
    void sendError(int code, const std::string& msg);
public:
    Command(Server* srv, Client* cli, const std::vector<std::string>& p);
    virtual void execute() = 0;
};
```

---

## 📈 Expected Test Output

```
╔════════════════════════════════════════════════════════════════════╗
║          USERCOMMAND STANDALONE TEST SUITE - ft_irc Project        ║
║                                                                    ║
║  This test uses mock implementations - no external dependencies    ║
╚════════════════════════════════════════════════════════════════════╝

--- CONSTRUCTOR TESTS (6 tests) ---

Testing Constructor with valid parameters... PASS
Testing Constructor with NULL server throws exception... PASS
Testing Constructor with NULL client throws exception... PASS
Testing Constructor with NULL server AND client throws exception... PASS
Testing Constructor with empty params (validation in execute)... PASS
Testing Constructor with many parameters... PASS

... (more tests) ...

╔════════════════════════════════════════════════════════════════════╗
║                          TEST SUMMARY                              ║
╚════════════════════════════════════════════════════════════════════╝

  Total tests run: 83
  Passed: 83
  Failed: 0

╔════════════════════════════════════════════════════════════════════╗
║                 ALL TESTS PASSED SUCCESSFULLY!                    ║
╚════════════════════════════════════════════════════════════════════╝
```

---

## 🔧 Troubleshooting

### Common Issues

#### 1. Compilation Errors
```bash
# Make sure you're using the standalone file
g++ -Wall -Wextra -Werror -std=c++98 tests/test_usercommand_standalone.cpp -o test_usercommand
```

#### 2. Socket Errors
- Tests use `socketpair()` which requires POSIX (Linux/macOS)
- Windows users: use WSL or adapt the socket code

#### 3. Test Failures
| Error | Likely Cause | Solution |
|-------|--------------|----------|
| 464 not returned | `isAuthenticated()` wrong | Check auth logic |
| 461 not returned | Param count check wrong | Check `params.size() < 4` |
| 462 not returned | `isRegistered()` wrong | Check registration flag |
| No welcome | Registration incomplete | Check NICK + USER logic |

---

## 📝 Updating the UserCommand Implementation

The `UserCommand` class is copied directly into the test file. If you modify your real `UserCommand.cpp`, update the copy in the test file:

```cpp
// In test_usercommand_standalone.cpp, around line 160
class UserCommand : public Command {
public:
    UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params) {
    }

    ~UserCommand() {
    }

    void execute() {
        // YOUR IMPLEMENTATION HERE
        // Copy from src/UserCommand.cpp
    }
};
```

---

## 📚 Related Documentation

- [UserCommand.md](../tasks/BAHOLY/UserCommand.md) - Task specification
- [IRC RFC 2812](https://tools.ietf.org/html/rfc2812) - IRC protocol specification
- [test_command.cpp](./test_command.cpp) - Base Command class tests

---

## ✅ Checklist for Reviewers

- [x] Compiles with single command (no dependencies)
- [x] All 83 tests pass
- [x] Mock classes match real class interfaces
- [x] Socket file descriptors properly closed
- [x] Test names are descriptive
- [x] Edge cases covered (long strings, empty strings, special chars)
- [x] Error messages are helpful
- [x] Stress tests included

---

## 🎉 Quick Start

```bash
# Clone/navigate to project
cd /path/to/ft_irc

# Compile and run in one command
g++ -Wall -Wextra -Werror -std=c++98 tests/test_usercommand_standalone.cpp -o test_usercommand && ./test_usercommand
```

**That's it! No other files needed.**
