# Nouveaux Changements - 22 Janvier 2026

Ce document détaille les nouvelles classes et modifications ajoutées au projet IRC.

---

## 📋 Résumé des Ajouts

| Fichier | Type | Description |
|---------|------|-------------|
| `includes/Replies.hpp` | Header | Macros pour les codes numériques IRC |
| `includes/NoticeCommand.hpp` | Header | Commande NOTICE |
| `includes/PingCommand.hpp` | Header | Commande PING |
| `includes/PongCommand.hpp` | Header | Commande PONG |
| `src/NoticeCommand.cpp` | Source | Implémentation NOTICE |
| `src/PingCommand.cpp` | Source | Implémentation PING |
| `src/PongCommand.cpp` | Source | Implémentation PONG |
| `includes/Client.hpp` | Modifié | Ajout de `getPrefix()` |
| `src/Client.cpp` | Modifié | Implémentation de `getPrefix()` |

---

## 1. Replies.hpp - Codes Numériques IRC

### Description
`Replies.hpp` centralise tous les codes de réponse numériques IRC (RFC 1459) sous forme de macros. Cela évite la duplication de code dans chaque commande et garantit un format cohérent.

### Emplacement
- **Fichier** : `includes/Replies.hpp`

### Contenu Principal

#### Constantes du Serveur
```cpp
#define SERVER_NAME "ircserv"
#define SERVER_VERSION "1.0"
```

#### Messages de Bienvenue (001-004)
```cpp
#define RPL_WELCOME(nick, user, host)     // 001 - Welcome to IRC
#define RPL_YOURHOST(nick)                // 002 - Your host is...
#define RPL_CREATED(nick, date)           // 003 - Server created
#define RPL_MYINFO(nick)                  // 004 - Server info
```

#### Réponses de Canal (3xx)
```cpp
#define RPL_UMODEIS(nick, modes)                    // 221
#define RPL_CHANNELMODEIS(nick, chan, modes, params) // 324
#define RPL_NOTOPIC(nick, chan)                     // 331
#define RPL_TOPIC(nick, chan, topic)                // 332
#define RPL_INVITING(nick, target, chan)            // 341
#define RPL_NAMREPLY(nick, chan, names)             // 353
#define RPL_ENDOFNAMES(nick, chan)                  // 366
```

#### Codes d'Erreur (4xx)
```cpp
#define ERR_NOSUCHNICK(nick, target)           // 401
#define ERR_NOSUCHCHANNEL(nick, chan)          // 403
#define ERR_CANNOTSENDTOCHAN(nick, chan)       // 404
#define ERR_TOOMANYCHANNELS(nick, chan)        // 405
#define ERR_NORECIPIENT(nick, cmd)             // 411
#define ERR_NOTEXTTOSEND(nick)                 // 412
#define ERR_UNKNOWNCOMMAND(nick, cmd)          // 421
#define ERR_NONICKNAMEGIVEN(nick)              // 431
#define ERR_ERRONEUSNICKNAME(nick, badnick)    // 432
#define ERR_NICKNAMEINUSE(nick)                // 433
#define ERR_USERNOTINCHANNEL(nick, target, chan) // 441
#define ERR_NOTONCHANNEL(nick, chan)           // 442
#define ERR_USERONCHANNEL(nick, target, chan)  // 443
#define ERR_NEEDMOREPARAMS(nick, cmd)          // 461
#define ERR_ALREADYREGISTERED(nick)            // 462
#define ERR_PASSWDMISMATCH(nick)               // 464
#define ERR_CHANNELISFULL(nick, chan)          // 471
#define ERR_INVITEONLYCHAN(nick, chan)         // 473
#define ERR_BANNEDFROMCHAN(nick, chan)         // 474
#define ERR_BADCHANNELKEY(nick, chan)          // 475
#define ERR_BADCHANMASK(nick, chan)            // 476
#define ERR_CHANOPRIVSNEEDED(nick, chan)       // 482
```

#### Utilitaire Prefix
```cpp
#define USER_PREFIX(nick, user, host)  // ":nick!user@host"
```

### Utilisation
```cpp
#include "Replies.hpp"

// Dans une commande
std::string nick = client->getNickname();
client->sendMessage(RPL_WELCOME(nick, user, host));
client->sendMessage(ERR_NEEDMOREPARAMS(nick, "JOIN"));
```

### Liaison avec les Classes Existantes
- **Command** : Toutes les sous-classes de Command utilisent ces macros
- **Server** : Utilise les RPL_WELCOME pour les messages de bienvenue
- **Channel** : Utilise RPL_TOPIC, RPL_NAMREPLY, RPL_ENDOFNAMES

---

## 2. NoticeCommand - Commande NOTICE

### Description
La commande `NOTICE` est similaire à `PRIVMSG` mais avec une différence importante : elle ne génère **jamais** de réponse automatique, même en cas d'erreur. C'est utilisé pour les notifications système et éviter les boucles de messages.

### Emplacement
- **Header** : `includes/NoticeCommand.hpp`
- **Source** : `src/NoticeCommand.cpp`

### Syntaxe IRC
```
NOTICE <cible> :<message>
```

### Comportement
1. Si la cible est un canal (`#channel`) :
   - Vérifie que le client est membre du canal
   - Diffuse le message à tous les membres (sauf l'expéditeur)
2. Si la cible est un utilisateur :
   - Envoie le message directement à l'utilisateur cible

### Différences avec PRIVMSG
| Aspect | PRIVMSG | NOTICE |
|--------|---------|--------|
| Erreurs | Envoie des codes d'erreur | Aucune réponse d'erreur |
| Bots | Peuvent répondre | Ne doivent PAS répondre |
| Usage | Communication normale | Notifications système |

### Liaison avec les Classes Existantes
- **Hérite de** : `Command`
- **Utilise** : `Server::getOrCreateChannel()`, `Server::getClientByNick()`
- **Utilise** : `Channel::broadcast()`, `Channel::isMember()`
- **Utilise** : `Client::sendMessage()`, `Client::getPrefix()`

### Exemple
```
Client → Server: NOTICE #general :Server maintenance at 22:00
Server → #general: :nick!user@host NOTICE #general :Server maintenance at 22:00
```

---

## 3. PingCommand & PongCommand - Keep-Alive

### Description
`PING` et `PONG` sont essentiels pour maintenir les connexions actives et détecter les clients déconnectés.

### Emplacement
- **Headers** : `includes/PingCommand.hpp`, `includes/PongCommand.hpp`
- **Sources** : `src/PingCommand.cpp`, `src/PongCommand.cpp`

### Syntaxe IRC
```
PING <token>
PONG <token>
```

### Comportement

#### PingCommand
Quand le serveur reçoit un `PING` du client :
1. Extrait le token du paramètre
2. Répond immédiatement avec `PONG :token`

```cpp
void PingCommand::execute() {
    std::string token = params[0];
    std::string pongReply = ":ircserv PONG ircserv :" + token;
    client->sendMessage(pongReply);
}
```

#### PongCommand
Quand le serveur reçoit un `PONG` du client :
1. Confirme que le client est toujours actif
2. (Optionnel) Met à jour le timestamp de dernière activité

### Flux Typique
```
Server → Client: PING :timestamp123
Client → Server: PONG :timestamp123
```

Ou initié par le client :
```
Client → Server: PING :test
Server → Client: :ircserv PONG ircserv :test
```

### Liaison avec les Classes Existantes
- **Hérite de** : `Command`
- **Utilise** : `Client::sendMessage()`
- **Futur** : Peut être utilisé par Server pour détecter les timeouts

---

## 4. Client::getPrefix() - Génération du Prefix IRC

### Description
Nouvelle méthode utilitaire dans la classe `Client` pour générer le prefix IRC standard.

### Emplacement
- **Header** : `includes/Client.hpp`
- **Source** : `src/Client.cpp`

### Signature
```cpp
std::string getPrefix() const;
```

### Implémentation
```cpp
std::string Client::getPrefix() const {
    return ":" + nickname + "!" + username + "@" + hostname;
}
```

### Format de Sortie
```
:nickname!username@hostname
```

### Exemple
Pour un client avec :
- nickname = "john"
- username = "~john"
- hostname = "192.168.1.1"

Retourne : `:john!~john@192.168.1.1`

### Utilisation
```cpp
// Avant (dans chaque commande)
std::string prefix = ":" + client->getNickname() + "!" + 
                     client->getUsername() + "@" + client->getHostname();

// Après (simplifié)
std::string prefix = client->getPrefix();
```

### Liaison avec les Classes Existantes
- **Utilisé par** : Toutes les commandes qui diffusent des messages
  - PrivmsgCommand, NoticeCommand
  - JoinCommand, PartCommand, QuitCommand
  - KickCommand, InviteCommand, TopicCommand
  - ModeCommand, NickCommand

---

## 5. Diagrammes Mis à Jour

### UML PlantUML
Le fichier `docs/diagram/uml.puml` a été mis à jour avec :
- Ajout de `NoticeCommand`, `PingCommand`, `PongCommand`
- Ajout de `getPrefix()` dans Client
- Note sur l'utilisation de `Replies.hpp`

### UML DrawIO
Le fichier `docs/diagram/uml.drawio` a été mis à jour avec les mêmes ajouts visuels.

---

## 📊 Diagramme de Dépendances

```
                    Replies.hpp
                         │
                         ▼
    ┌────────────────────────────────────────┐
    │              Command (base)            │
    │   - utilise Replies.hpp pour réponses  │
    └────────────────────────────────────────┘
                         △
                         │
    ┌────────┬───────────┼───────────┬────────┐
    │        │           │           │        │
    ▼        ▼           ▼           ▼        ▼
 Privmsg  Notice      Ping        Pong     [autres]
    │        │           │           │
    │        │           │           │
    ▼        ▼           ▼           ▼
    └────────┴───────────┴───────────┘
                    │
                    ▼
              Client.getPrefix()
```

---

## 🔧 Actions Requises

### Mise à jour du MessageParser
Ajouter les nouvelles commandes dans `MessageParser::createCommand()` :

```cpp
// À ajouter dans MessageParser.cpp
#include "../includes/NoticeCommand.hpp"
#include "../includes/PingCommand.hpp"
#include "../includes/PongCommand.hpp"

// Dans createCommand()
else if (cmd == "NOTICE") {
    return new NoticeCommand(srv, cli, params);
}
else if (cmd == "PING") {
    return new PingCommand(srv, cli, params);
}
else if (cmd == "PONG") {
    return new PongCommand(srv, cli, params);
}
```

### Mise à jour du Makefile
Ajouter les nouveaux fichiers sources :
```makefile
SRCS += src/NoticeCommand.cpp
SRCS += src/PingCommand.cpp
SRCS += src/PongCommand.cpp
```

---

## 📅 Assignation des Nouvelles Tâches

| Tâche | Assigné à | Priorité |
|-------|-----------|----------|
| Intégrer Replies.hpp dans les commandes existantes | ALL | Haute |
| Ajouter NoticeCommand au MessageParser | BAHOLY | Haute |
| Ajouter PingCommand/PongCommand au MessageParser | BAHOLY | Haute |
| Mettre à jour le Makefile | NERO | Haute |
| Refactoriser les commandes pour utiliser getPrefix() | ALL | Moyenne |
| Implémenter le timeout PING côté serveur | NERO | Basse |
