# Classe NoticeCommand - Description de la Tâche

## Vue d'ensemble
La classe `NoticeCommand` gère la commande NOTICE pour envoyer des messages aux utilisateurs ou aux canaux. Contrairement à PRIVMSG, NOTICE ne génère jamais de réponse automatique, même en cas d'erreur.

## Emplacement de la Classe
- **En-tête** : `includes/NoticeCommand.hpp`
- **Implémentation** : `src/NoticeCommand.cpp`

## Commande IRC
**Syntaxe** : `NOTICE <cible> :<message>`

---

## Méthodes

### Constructeur
```cpp
NoticeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande NOTICE

**TODO** :
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Envoyer une notification à un utilisateur ou un canal

**TODO** :

#### Étape 1 : Valider les Paramètres (SANS ERREUR)
- [x] Vérifier si params contient au moins 2 éléments (cible et message)
- [x] Si paramètres insuffisants : **retourner silencieusement** (pas d'erreur)
- [x] RFC 1459 : NOTICE ne doit JAMAIS générer de réponse d'erreur

#### Étape 2 : Extraire la Cible et le Message
- [x] Cible : params[0]
- [x] Message : params[1]
- [x] Si message vide : retourner silencieusement

#### Étape 3 : Construire le Prefix
- [x] Utiliser `client->getPrefix()` pour obtenir `:nick!user@host`
- [x] Formater : `prefix + " NOTICE " + target + " :" + message`

#### Étape 4a : Notification de Canal
Si la cible commence par `#` ou `&` :
- [x] Récupérer le canal depuis le serveur
- [x] Si le canal n'existe pas : **retourner silencieusement**
- [x] Vérifier si le client est membre du canal
- [x] Si pas membre : **retourner silencieusement**
- [x] Diffuser au canal, EXCLURE l'expéditeur

#### Étape 4b : Notification Privée
Si la cible est un pseudonyme :
- [x] Trouver le client cible par pseudonyme
- [x] Si non trouvé : **retourner silencieusement**
- [x] Envoyer au client cible

---

## Différences avec PrivmsgCommand

| Aspect | PRIVMSG | NOTICE |
|--------|---------|--------|
| ERR_NORECIPIENT (411) | Oui | Non |
| ERR_NOTEXTTOSEND (412) | Oui | Non |
| ERR_NOSUCHNICK (401) | Oui | Non |
| ERR_CANNOTSENDTOCHAN (404) | Oui | Non |
| Réponse automatique des bots | Autorisée | INTERDITE |

---

## Cas de Test

### Test 1 : Notice vers un Canal
```
Client → Server: NOTICE #general :Hello everyone
Server → #general: :nick!user@host NOTICE #general :Hello everyone
```
**Résultat attendu** : Message diffusé à tous sauf l'expéditeur

### Test 2 : Notice Privée
```
Client → Server: NOTICE john :Private notification
Server → john: :nick!user@host NOTICE john :Private notification
```
**Résultat attendu** : Message envoyé uniquement à john

### Test 3 : Paramètres Manquants
```
Client → Server: NOTICE
```
**Résultat attendu** : Aucune réponse (silence)

### Test 4 : Cible Inexistante
```
Client → Server: NOTICE nonexistent :Hello
```
**Résultat attendu** : Aucune réponse (silence)

---

## Dépendances

### Classes Utilisées
- `Command` (classe parente)
- `Server` : `getOrCreateChannel()`, `getClientByNick()`
- `Client` : `getPrefix()`, `sendMessage()`
- `Channel` : `isMember()`, `broadcast()`

### Fichiers Headers
```cpp
#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Replies.hpp"  // Pour USER_PREFIX si besoin
```

---

## État d'Implémentation

- [x] Header créé
- [x] Implémentation créée
- [ ] Ajouté à MessageParser
- [ ] Ajouté au Makefile
- [ ] Tests effectués

---

## Références
- RFC 1459, Section 4.4.2 - Notice
- [docs/irc_messages.md](../docs/irc_messages.md) - Section 3. Messaging
