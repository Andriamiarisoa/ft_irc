# Commandes PING et PONG - Description de la Tâche

## Vue d'ensemble
Les commandes `PING` et `PONG` sont essentielles pour maintenir les connexions actives (keep-alive) et détecter les clients déconnectés sans notification.

## Emplacement des Classes
- **Headers** : `includes/PingCommand.hpp`, `includes/PongCommand.hpp`
- **Sources** : `src/PingCommand.cpp`, `src/PongCommand.cpp`

## Assignation
- **Responsable** : ALL (intégration commune)
- **Priorité** : Haute (fonctionnalité essentielle)

---

# PingCommand

## Commande IRC
**Syntaxe** : `PING <token>`

## Comportement
Quand un client envoie PING au serveur, le serveur doit répondre avec PONG.

### execute()
```cpp
void execute()
```

**TODO** :

#### Étape 1 : Valider les Paramètres
- [x] Vérifier si params contient au moins 1 élément (token)
- [x] Si manquant : envoyer erreur 409 (ERR_NOORIGIN)

#### Étape 2 : Construire la Réponse PONG
- [x] Format : `:servername PONG servername :token`
- [x] Exemple : `:ircserv PONG ircserv :LAG123`

#### Étape 3 : Envoyer la Réponse
- [x] Appeler `client->sendMessage(pongReply)`

---

## Cas de Test

### Test 1 : PING Normal
```
Client → Server: PING :LAG123456
Server → Client: :ircserv PONG ircserv :LAG123456
```

### Test 2 : PING sans Token
```
Client → Server: PING
Server → Client: :ircserv 409 nick :No origin specified
```

---

# PongCommand

## Commande IRC
**Syntaxe** : `PONG <token>`

## Comportement
Quand un client répond au PING du serveur avec PONG, le serveur confirme que la connexion est active.

### execute()
```cpp
void execute()
```

**TODO** :

#### Étape 1 : Recevoir le PONG
- [x] Pas de validation stricte nécessaire
- [x] Le token peut être ignoré ou comparé au PING envoyé

#### Étape 2 : Mettre à jour l'état du client (optionnel)
- [ ] Mettre à jour le timestamp de dernière activité
- [ ] Réinitialiser le compteur de timeout

#### Étape 3 : Aucune réponse requise
- [x] PONG ne génère pas de réponse du serveur

---

## Cas de Test

### Test 1 : PONG Normal
```
Server → Client: PING :timestamp123
Client → Server: PONG :timestamp123
```
**Résultat** : Connexion maintenue active

### Test 2 : PONG sans PING préalable
```
Client → Server: PONG :random
```
**Résultat** : Accepté silencieusement (pas d'erreur)

---

# Flux Keep-Alive Complet

```
┌────────┐                          ┌────────┐
│ Server │                          │ Client │
└───┬────┘                          └───┬────┘
    │                                   │
    │  PING :1706012345                 │
    │──────────────────────────────────>│
    │                                   │
    │  PONG :1706012345                 │
    │<──────────────────────────────────│
    │                                   │
    │  [Connexion confirmée active]     │
    │                                   │
```

Ou initié par le client :

```
┌────────┐                          ┌────────┐
│ Client │                          │ Server │
└───┬────┘                          └───┬────┘
    │                                   │
    │  PING :LAG123                     │
    │──────────────────────────────────>│
    │                                   │
    │  :ircserv PONG ircserv :LAG123    │
    │<──────────────────────────────────│
    │                                   │
```

---

# Implémentation Timeout (Futur)

## Dans Server (optionnel, basse priorité)

Pour détecter les clients inactifs :

```cpp
// Ajouter dans Client
time_t lastActivity;
void updateActivity() { lastActivity = time(NULL); }

// Ajouter dans Server::handleSelect() ou boucle principale
void checkTimeouts() {
    time_t now = time(NULL);
    for (auto& pair : clients) {
        Client* c = pair.second;
        if (now - c->lastActivity > PING_TIMEOUT) {
            // Envoyer PING
            c->sendMessage("PING :" + std::to_string(now));
        }
        if (now - c->lastActivity > DISCONNECT_TIMEOUT) {
            // Déconnecter le client
            disconnectClient(c->getFd());
        }
    }
}
```

---

## Dépendances

### Classes Utilisées
- `Command` (classe parente)
- `Client` : `sendMessage()`

### Fichiers Headers
```cpp
#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"  // Pour SERVER_NAME
```

---

## État d'Implémentation

### PingCommand
- [x] Header créé
- [x] Implémentation créée
- [ ] Ajouté à MessageParser
- [ ] Ajouté au Makefile
- [ ] Tests effectués

### PongCommand
- [x] Header créé
- [x] Implémentation créée
- [ ] Ajouté à MessageParser
- [ ] Ajouté au Makefile
- [ ] Tests effectués

### Timeout Server
- [ ] Implémentation (basse priorité)

---

## Références
- RFC 1459, Section 4.6.2 - Ping
- RFC 1459, Section 4.6.3 - Pong
