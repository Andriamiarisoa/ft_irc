#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>

// Server name constant
#define SERVER_NAME "ircserv"
#define SERVER_VERSION "1.0"

// ============================================================================
// WELCOME MESSAGES (001-004)
// ============================================================================
#define RPL_WELCOME(nick, user, host) \
    (":" SERVER_NAME " 001 " + (nick) + " :Welcome to the IRC Network " + (nick) + "!" + (user) + "@" + (host))

#define RPL_YOURHOST(nick) \
    (":" SERVER_NAME " 002 " + (nick) + " :Your host is " SERVER_NAME ", running version " SERVER_VERSION)

#define RPL_CREATED(nick, date) \
    (":" SERVER_NAME " 003 " + (nick) + " :This server was created " + (date))

#define RPL_MYINFO(nick) \
    (":" SERVER_NAME " 004 " + (nick) + " " SERVER_NAME " " SERVER_VERSION " o itkol")

// ============================================================================
// CHANNEL REPLIES (3xx)
// ============================================================================
#define RPL_UMODEIS(nick, modes) \
    (":" SERVER_NAME " 221 " + (nick) + " " + (modes))

#define RPL_CHANNELMODEIS(nick, chan, modes, params) \
    (":" SERVER_NAME " 324 " + (nick) + " " + (chan) + " " + (modes) + " " + (params))

#define RPL_NOTOPIC(nick, chan) \
    (":" SERVER_NAME " 331 " + (nick) + " " + (chan) + " :No topic is set")

#define RPL_TOPIC(nick, chan, topic) \
    (":" SERVER_NAME " 332 " + (nick) + " " + (chan) + " :" + (topic))

#define RPL_INVITING(nick, target, chan) \
    (":" SERVER_NAME " 341 " + (nick) + " " + (target) + " " + (chan))

#define RPL_NAMREPLY(nick, chan, names) \
    (":" SERVER_NAME " 353 " + (nick) + " = " + (chan) + " :" + (names))

#define RPL_ENDOFNAMES(nick, chan) \
    (":" SERVER_NAME " 366 " + (nick) + " " + (chan) + " :End of /NAMES list")

// ============================================================================
// ERROR REPLIES (4xx)
// ============================================================================
#define ERR_NOSUCHNICK(nick, target) \
    (":" SERVER_NAME " 401 " + (nick) + " " + (target) + " :No such nick/channel")

#define ERR_NOSUCHCHANNEL(nick, chan) \
    (":" SERVER_NAME " 403 " + (nick) + " " + (chan) + " :No such channel")

#define ERR_CANNOTSENDTOCHAN(nick, chan) \
    (":" SERVER_NAME " 404 " + (nick) + " " + (chan) + " :Cannot send to channel")

#define ERR_TOOMANYCHANNELS(nick, chan) \
    (":" SERVER_NAME " 405 " + (nick) + " " + (chan) + " :You have joined too many channels")

#define ERR_NOORIGIN(nick) \
    (":" SERVER_NAME " 409 " + (nick) + " :No origin specified")

#define ERR_NORECIPIENT(nick, cmd) \
    (":" SERVER_NAME " 411 " + (nick) + " :No recipient given (" + (cmd) + ")")

#define ERR_NOTEXTTOSEND(nick) \
    (":" SERVER_NAME " 412 " + (nick) + " :No text to send")

#define ERR_UNKNOWNCOMMAND(nick, cmd) \
    (":" SERVER_NAME " 421 " + (nick) + " " + (cmd) + " :Unknown command")

#define ERR_NONICKNAMEGIVEN(nick) \
    (":" SERVER_NAME " 431 " + (nick) + " :No nickname given")

#define ERR_ERRONEUSNICKNAME(nick, badnick) \
    (":" SERVER_NAME " 432 " + (nick) + " " + (badnick) + " :Erroneous nickname")

#define ERR_NICKNAMEINUSE(nick) \
    (":" SERVER_NAME " 433 * " + (nick) + " :Nickname is already in use")

#define ERR_USERNOTINCHANNEL(nick, target, chan) \
    (":" SERVER_NAME " 441 " + (nick) + " " + (target) + " " + (chan) + " :They aren't on that channel")

#define ERR_NOTONCHANNEL(nick, chan) \
    (":" SERVER_NAME " 442 " + (nick) + " " + (chan) + " :You're not on that channel")

#define ERR_USERONCHANNEL(nick, target, chan) \
    (":" SERVER_NAME " 443 " + (nick) + " " + (target) + " " + (chan) + " :is already on channel")

#define ERR_NOTREGISTERED(nick) \
    (":" SERVER_NAME " 451 " + (nick) + " :You have not registered")

#define ERR_NEEDMOREPARAMS(nick, cmd) \
    (":" SERVER_NAME " 461 " + (nick) + " " + (cmd) + " :Not enough parameters")

#define ERR_ALREADYREGISTERED(nick) \
    (":" SERVER_NAME " 462 " + (nick) + " :You may not reregister")

#define ERR_PASSWDMISMATCH(nick) \
    (":" SERVER_NAME " 464 " + (nick) + " :Password incorrect")

#define ERR_CHANNELISFULL(nick, chan) \
    (":" SERVER_NAME " 471 " + (nick) + " " + (chan) + " :Cannot join channel (+l)")

#define ERR_INVITEONLYCHAN(nick, chan) \
    (":" SERVER_NAME " 473 " + (nick) + " " + (chan) + " :Cannot join channel (+i)")

#define ERR_BANNEDFROMCHAN(nick, chan) \
    (":" SERVER_NAME " 474 " + (nick) + " " + (chan) + " :Cannot join channel (+b)")

#define ERR_BADCHANNELKEY(nick, chan) \
    (":" SERVER_NAME " 475 " + (nick) + " " + (chan) + " :Cannot join channel (+k)")

#define ERR_BADCHANMASK(nick, chan) \
    (":" SERVER_NAME " 476 " + (nick) + " " + (chan) + " :Bad Channel Mask")

#define ERR_CHANOPRIVSNEEDED(nick, chan) \
    (":" SERVER_NAME " 482 " + (nick) + " " + (chan) + " :You're not channel operator")

#define ERR_INVALIDMODEPARAM(nick, chan, mode, param, reason) \
    (":" SERVER_NAME " 696 " + (nick) + " " + (chan) + " " + (mode) + " " + (param) + " :" + (reason))

// ============================================================================
// PREFIX HELPER
// ============================================================================
#define USER_PREFIX(nick, user, host) \
    (":" + (nick) + "!" + (user) + "@" + (host))

#endif
