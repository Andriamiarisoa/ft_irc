# Replies.hpp - Codes Numériques IRC

## Vue d'ensemble
Le fichier `Replies.hpp` centralise tous les codes de réponse numériques IRC sous forme de macros. Cela garantit un format cohérent et évite la duplication de code.

## Emplacement
- **Fichier** : `includes/Replies.hpp`

## Assignation
- **Responsable** : ALL (utilisé par toutes les commandes)
- **Intégration** : Chaque membre intègre dans ses commandes

---

## Structure du Fichier

### Constantes Serveur
```cpp
#define SERVER_NAME "ircserv"
#define SERVER_VERSION "1.0"
```

**Usage** : Identifier le serveur dans les réponses.

---

### Messages de Bienvenue (001-004)

| Code | Macro | Description |
|------|-------|-------------|
| 001 | `RPL_WELCOME` | Welcome to the IRC Network |
| 002 | `RPL_YOURHOST` | Your host is... |
| 003 | `RPL_CREATED` | This server was created... |
| 004 | `RPL_MYINFO` | Server info (modes supportés) |

**Utilisation** : Après l'enregistrement complet (NICK + USER)

```cpp
// Dans UserCommand ou NickCommand après enregistrement
client->sendMessage(RPL_WELCOME(nick, user, host));
client->sendMessage(RPL_YOURHOST(nick));
client->sendMessage(RPL_CREATED(nick, "Jan 2026"));
client->sendMessage(RPL_MYINFO(nick));
```

---

### Réponses de Canal (2xx-3xx)

| Code | Macro | Description |
|------|-------|-------------|
| 221 | `RPL_UMODEIS` | User mode is... |
| 324 | `RPL_CHANNELMODEIS` | Channel mode is... |
| 331 | `RPL_NOTOPIC` | No topic is set |
| 332 | `RPL_TOPIC` | Topic is... |
| 341 | `RPL_INVITING` | Inviting user to channel |
| 353 | `RPL_NAMREPLY` | Names list |
| 366 | `RPL_ENDOFNAMES` | End of /NAMES list |

**Utilisation** : Après JOIN ou sur demande TOPIC/NAMES

```cpp
// Après JOIN
client->sendMessage(RPL_TOPIC(nick, channel, topic));
// ou
client->sendMessage(RPL_NOTOPIC(nick, channel));

client->sendMessage(RPL_NAMREPLY(nick, channel, "@op user1 user2"));
client->sendMessage(RPL_ENDOFNAMES(nick, channel));
```

---

### Codes d'Erreur (4xx)

| Code | Macro | Description |
|------|-------|-------------|
| 401 | `ERR_NOSUCHNICK` | No such nick/channel |
| 403 | `ERR_NOSUCHCHANNEL` | No such channel |
| 404 | `ERR_CANNOTSENDTOCHAN` | Cannot send to channel |
| 405 | `ERR_TOOMANYCHANNELS` | Too many channels |
| 411 | `ERR_NORECIPIENT` | No recipient given |
| 412 | `ERR_NOTEXTTOSEND` | No text to send |
| 421 | `ERR_UNKNOWNCOMMAND` | Unknown command |
| 431 | `ERR_NONICKNAMEGIVEN` | No nickname given |
| 432 | `ERR_ERRONEUSNICKNAME` | Erroneous nickname |
| 433 | `ERR_NICKNAMEINUSE` | Nickname is already in use |
| 441 | `ERR_USERNOTINCHANNEL` | They aren't on that channel |
| 442 | `ERR_NOTONCHANNEL` | You're not on that channel |
| 443 | `ERR_USERONCHANNEL` | User is already on channel |
| 461 | `ERR_NEEDMOREPARAMS` | Not enough parameters |
| 462 | `ERR_ALREADYREGISTERED` | You may not reregister |
| 464 | `ERR_PASSWDMISMATCH` | Password incorrect |
| 471 | `ERR_CHANNELISFULL` | Cannot join channel (+l) |
| 473 | `ERR_INVITEONLYCHAN` | Cannot join channel (+i) |
| 474 | `ERR_BANNEDFROMCHAN` | Cannot join channel (+b) |
| 475 | `ERR_BADCHANNELKEY` | Cannot join channel (+k) |
| 476 | `ERR_BADCHANMASK` | Bad Channel Mask |
| 482 | `ERR_CHANOPRIVSNEEDED` | You're not channel operator |

---

### Utilitaire Prefix

```cpp
#define USER_PREFIX(nick, user, host) \
    (":" + (nick) + "!" + (user) + "@" + (host))
```

**Note** : Préférer `client->getPrefix()` qui est plus pratique.

---

## Guide d'Intégration

### Étape 1 : Inclure le Header
```cpp
#include "Replies.hpp"
```

### Étape 2 : Remplacer les Constructions Manuelles

**Avant** :
```cpp
std::string reply = ":ircserv 461 " + nick + " JOIN :Not enough parameters";
client->sendMessage(reply);
```

**Après** :
```cpp
client->sendMessage(ERR_NEEDMOREPARAMS(nick, "JOIN"));
```

### Étape 3 : Construire les Messages de Diffusion

**Avant** :
```cpp
std::string msg = ":" + client->getNickname() + "!" + 
                  client->getUsername() + "@" + client->getHostname() +
                  " JOIN " + channelName;
```

**Après** :
```cpp
std::string msg = client->getPrefix() + " JOIN " + channelName;
```

---

## Commandes et Codes Utilisés

| Commande | Codes Utilisés |
|----------|----------------|
| PASS | 462, 464, 461 |
| NICK | 431, 432, 433, 461 |
| USER | 461, 462, 001-004 |
| JOIN | 403, 461, 471, 473, 475, 331/332, 353, 366 |
| PART | 403, 442, 461 |
| PRIVMSG | 401, 404, 411, 412 |
| NOTICE | (aucun - silencieux) |
| KICK | 403, 441, 442, 461, 482 |
| INVITE | 401, 403, 442, 443, 341, 482 |
| TOPIC | 331, 332, 403, 442, 461, 482 |
| MODE | 221, 324, 403, 442, 461, 472, 482 |
| QUIT | (aucun) |
| PING | 409 |
| PONG | (aucun) |

---

## État d'Intégration par Commande

| Commande | Responsable | Intégré |
|----------|-------------|---------|
| PassCommand | NERO | [ ] |
| NickCommand | YASSER | [ ] |
| UserCommand | BAHOLY | [ ] |
| JoinCommand | YASSER | [ ] |
| PartCommand | YASSER | [ ] |
| PrivmsgCommand | BAHOLY | [ ] |
| NoticeCommand | BAHOLY | [x] |
| KickCommand | NERO | [ ] |
| InviteCommand | YASSER | [ ] |
| TopicCommand | BAHOLY | [ ] |
| ModeCommand | ALL | [ ] |
| QuitCommand | NERO | [ ] |
| PingCommand | ALL | [x] |
| PongCommand | ALL | [x] |

---

## Références
- [docs/irc_messages.md](../docs/irc_messages.md) - Section 5. Common Numeric Replies
- RFC 1459 - Internet Relay Chat Protocol
