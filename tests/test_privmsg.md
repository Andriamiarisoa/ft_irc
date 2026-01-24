# PRIVMSG Command - Documentation des Tests

## Vue d'ensemble

La commande **PRIVMSG** est la fonctionnalité de messagerie centrale d'IRC. Elle permet d'envoyer des messages à des canaux ou directement à d'autres utilisateurs.

**Syntaxe IRC** : `PRIVMSG <cible> :<message>`

---

## Codes d'Erreur IRC

| Code | Nom | Description |
|------|-----|-------------|
| 401 | ERR_NOSUCHNICK | `<nick> :No such nick/channel` - Destinataire inexistant |
| 403 | ERR_NOSUCHCHANNEL | `<channel> :No such channel` - Canal inexistant |
| 404 | ERR_CANNOTSENDTOCHAN | `<channel> :Cannot send to channel` - Pas membre du canal |
| 411 | ERR_NORECIPIENT | `:No recipient given (PRIVMSG)` - Pas de destinataire |
| 412 | ERR_NOTEXTTOSEND | `:No text to send` - Pas de message |
| 451 | ERR_NOTREGISTERED | `:You have not registered` - Client non enregistré |

---

## Compilation

```bash
# Depuis le dossier tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_privmsg_command.cpp ../src/Server.cpp ../src/Client.cpp \
    ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
    ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
    -o test_privmsg_command
```

---

## Exécution

```bash
# Tous les tests
./test_privmsg_command

# Test spécifique (1-20)
./test_privmsg_command 5

# Aide
./test_privmsg_command help

# Instructions manuelles
./test_privmsg_command manual

# Instructions valgrind
./test_privmsg_command valgrind
```

---

## Liste des Tests

### Tests d'Enregistrement (1-2)

#### Test 01: PRIVMSG Before Registration (ERR_NOTREGISTERED 451)
**Objectif** : Vérifier qu'un client non enregistré ne peut pas envoyer de messages.

**Préconditions** :
- Client connecté mais non enregistré (PASS/NICK/USER non complétés)

**Entrée** :
```
PRIVMSG #general :Hello
```

**Sortie attendue** :
```
:server 451 * :You have not registered
```

**Vérifications** :
- [ ] Erreur 451 retournée
- [ ] Message non transmis

---

#### Test 02: PRIVMSG After Partial Registration
**Objectif** : Vérifier le comportement après PASS mais avant NICK/USER.

**Préconditions** :
- Client a envoyé PASS mais pas NICK/USER

**Entrée** :
```
PASS testpass
PRIVMSG alice :Hi
```

**Sortie attendue** :
```
:server 451 * :You have not registered
```

---

### Tests de Validation des Paramètres (3-6)

#### Test 03: PRIVMSG Without Parameters (ERR_NORECIPIENT 411)
**Objectif** : Vérifier l'erreur quand aucun paramètre n'est fourni.

**Entrée** :
```
PRIVMSG
```

**Sortie attendue** :
```
:server 411 nick :No recipient given (PRIVMSG)
```

**Note** : L'implémentation actuelle retourne "Wrong parameters" - à corriger.

---

#### Test 04: PRIVMSG Without Message (ERR_NOTEXTTOSEND 412)
**Objectif** : Vérifier l'erreur quand le message est manquant.

**Entrée** :
```
PRIVMSG #general
```

**Sortie attendue** :
```
:server 412 nick :No text to send
```

---

#### Test 05: PRIVMSG With Empty Message
**Objectif** : Vérifier le comportement avec un message vide.

**Entrée** :
```
PRIVMSG #general :
```

**Sortie attendue** :
```
:server 412 nick :No text to send
```
OU le message vide est transmis (selon implémentation).

---

#### Test 06: PRIVMSG Target Only No Colon
**Objectif** : Vérifier le comportement sans le préfixe `:` du message.

**Entrée** :
```
PRIVMSG #general Hello world
```

**Sortie attendue** :
- Erreur 411/412 (message mal formaté)
- OU message "Hello" transmis (selon parser)

---

### Tests de Messages de Canal (7-12)

#### Test 07: PRIVMSG to Channel - Success
**Objectif** : Envoyer un message à un canal dont on est membre.

**Préconditions** :
- Client enregistré
- Client membre de #general

**Entrée** :
```
PRIVMSG #general :Hello everyone!
```

**Sortie attendue** (aux autres membres) :
```
:nick!user@host PRIVMSG #general :Hello everyone!
```

**Vérifications** :
- [ ] Message reçu par tous les membres du canal
- [ ] L'expéditeur NE reçoit PAS son propre message
- [ ] Format du préfixe correct `:nick!user@host`

---

#### Test 08: PRIVMSG to Non-existent Channel (ERR_NOSUCHCHANNEL 403)
**Objectif** : Envoyer un message à un canal inexistant.

**Entrée** :
```
PRIVMSG #nonexistent :Hello
```

**Sortie attendue** :
```
:server 403 nick #nonexistent :No such channel
```

---

#### Test 09: PRIVMSG to Channel Not Member (ERR_CANNOTSENDTOCHAN 404)
**Objectif** : Envoyer un message à un canal dont on n'est pas membre.

**Préconditions** :
- Canal #private existe
- Client n'est PAS membre de #private

**Entrée** :
```
PRIVMSG #private :Hello
```

**Sortie attendue** :
```
:server 404 nick #private :Cannot send to channel
```

---

#### Test 10: PRIVMSG to Channel with & Prefix
**Objectif** : Tester avec le préfixe `&` (canaux locaux).

**Entrée** :
```
PRIVMSG &local :Hello
```

**Sortie attendue** :
- Si &local existe et membre → message transmis
- Sinon → ERR_NOSUCHCHANNEL ou ERR_CANNOTSENDTOCHAN

---

#### Test 11: PRIVMSG Channel Broadcast Excludes Sender
**Objectif** : Vérifier que l'expéditeur ne reçoit pas son propre message.

**Setup** :
```
Terminal 1 (alice): JOIN #test
Terminal 2 (bob): JOIN #test
Terminal 1: PRIVMSG #test :Hello from Alice
```

**Vérifications** :
- [ ] Terminal 2 (bob) reçoit le message
- [ ] Terminal 1 (alice) NE reçoit PAS le message

---

#### Test 12: PRIVMSG to Empty Channel
**Objectif** : Envoyer un message à un canal vide (cas limite).

**Préconditions** :
- Canal existe mais tous les membres sont partis
- Client n'est pas membre

**Entrée** :
```
PRIVMSG #emptychannel :Anyone here?
```

**Sortie attendue** :
- ERR_NOSUCHCHANNEL (403) si le canal a été supprimé
- ERR_CANNOTSENDTOCHAN (404) si le canal existe encore

---

### Tests de Messages Privés (13-17)

#### Test 13: PRIVMSG to User - Success
**Objectif** : Envoyer un message privé à un utilisateur existant.

**Setup** :
- alice et bob sont connectés et enregistrés

**Entrée (alice)** :
```
PRIVMSG bob :Hi Bob!
```

**Sortie attendue (bob reçoit)** :
```
:alice!alice@host PRIVMSG bob :Hi Bob!
```

**Vérifications** :
- [ ] Seul bob reçoit le message
- [ ] alice ne reçoit pas d'écho

---

#### Test 14: PRIVMSG to Non-existent User (ERR_NOSUCHNICK 401)
**Objectif** : Envoyer un message à un utilisateur inexistant.

**Entrée** :
```
PRIVMSG unknownuser :Hello
```

**Sortie attendue** :
```
:server 401 nick unknownuser :No such nick/channel
```

---

#### Test 15: PRIVMSG to Self
**Objectif** : Un utilisateur s'envoie un message à lui-même.

**Entrée (alice)** :
```
PRIVMSG alice :Note to self
```

**Sortie attendue** :
- Le message est délivré à alice
- Format : `:alice!alice@host PRIVMSG alice :Note to self`

---

#### Test 16: PRIVMSG Case Sensitivity (Nicknames)
**Objectif** : Vérifier si la recherche de pseudo est sensible à la casse.

**Setup** :
- bob est connecté (minuscules)

**Entrée** :
```
PRIVMSG BOB :Hello
PRIVMSG Bob :Hello
PRIVMSG bob :Hello
```

**Vérifications** :
- [ ] Tester si les 3 variantes fonctionnent
- [ ] IRC standard : insensible à la casse pour les pseudos

---

#### Test 17: PRIVMSG to User - Message Format
**Objectif** : Vérifier le format exact du message transmis.

**Entrée** :
```
PRIVMSG alice :Hello Alice!
```

**Format attendu** :
```
:sender_nick!sender_user@host PRIVMSG alice :Hello Alice!\r\n
```

**Composants** :
| Élément | Valeur |
|---------|--------|
| Préfixe | `:sender_nick!sender_user@host` |
| Commande | `PRIVMSG` |
| Cible | `alice` |
| Message | `:Hello Alice!` |
| Terminaison | `\r\n` |

---

### Tests de Contenu de Message (18-20)

#### Test 18: PRIVMSG with Special Characters
**Objectif** : Tester les caractères spéciaux dans le message.

**Entrées à tester** :
```
PRIVMSG #test :Message with 'quotes'
PRIVMSG #test :Message with "double quotes"
PRIVMSG #test :Message with : colons : inside
PRIVMSG #test :!@#$%^&*()_+-=[]{}|;
PRIVMSG #test :Émojis et accénts
```

**Vérifications** :
- [ ] Les caractères spéciaux sont préservés
- [ ] Pas de corruption du message
- [ ] Les colons internes au message sont conservés

---

#### Test 19: PRIVMSG with Long Message
**Objectif** : Tester avec un message très long.

**Entrée** :
```
PRIVMSG #test :[message de 500+ caractères]
```

**Vérifications** :
- [ ] Limite IRC : 512 octets total (incluant CRLF)
- [ ] Message tronqué ou rejeté si trop long
- [ ] Pas de crash serveur

---

#### Test 20: PRIVMSG Stress Test
**Objectif** : Tester l'envoi rapide de nombreux messages.

**Test** :
- Envoyer 100 messages rapidement
- Vérifier que tous sont délivrés
- Vérifier l'ordre de livraison

---

## Tests Manuels avec netcat

### Setup Initial
```bash
# Terminal 1 - Serveur
./ircserv 6667 testpass

# Terminal 2 - Client 1 (alice)
nc -C localhost 6667

# Terminal 3 - Client 2 (bob)
nc -C localhost 6667
```

### Test Manuel 1: Message de Canal
```bash
# Terminal 2 (alice)
PASS testpass
NICK alice
USER alice 0 * :Alice Smith
JOIN #general

# Terminal 3 (bob)
PASS testpass
NICK bob
USER bob 0 * :Bob Jones
JOIN #general

# Terminal 2 (alice)
PRIVMSG #general :Hello everyone!

# Vérifier: Terminal 3 reçoit le message
# Vérifier: Terminal 2 NE reçoit PAS son propre message
```

### Test Manuel 2: Message Privé
```bash
# Terminal 2 (alice) - après setup
PRIVMSG bob :Hi Bob, this is private!

# Vérifier: Terminal 3 (bob) reçoit:
# :alice!alice@host PRIVMSG bob :Hi Bob, this is private!
```

### Test Manuel 3: Erreurs
```bash
# Sans enregistrement
PRIVMSG #test :Hello
# Attendu: 451 :You have not registered

# Après enregistrement - canal inexistant
PRIVMSG #nonexistent :Hello
# Attendu: 403 #nonexistent :No such channel

# Utilisateur inexistant
PRIVMSG unknownuser :Hello
# Attendu: 401 unknownuser :No such nick/channel
```

---

## Tests avec irssi

```bash
# Connexion
/connect localhost 6667 testpass

# Définir le pseudo
/nick alice

# Rejoindre un canal
/join #general

# Envoyer un message au canal
/msg #general Hello everyone!

# Envoyer un message privé
/msg bob Hi Bob!

# Ouvrir une fenêtre de conversation privée
/query bob
Hello Bob, how are you?
```

---

## Instructions Valgrind

```bash
# Test de base
valgrind --leak-check=full ./test_privmsg_command

# Test détaillé
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./test_privmsg_command

# Test spécifique
valgrind --leak-check=full ./test_privmsg_command 7

# Vérifier la sortie pour:
# - "definitely lost: 0 bytes"
# - "indirectly lost: 0 bytes"
# - Pas d'erreurs "Invalid read/write"
```

---

## Critères d'Acceptation

### Fonctionnalités Obligatoires
- [x] Vérification de l'enregistrement du client
- [x] Envoi de message à un canal
- [x] Envoi de message privé à un utilisateur
- [x] Gestion ERR_NOSUCHCHANNEL (403)
- [x] Gestion ERR_NOSUCHNICK (401)
- [x] Gestion ERR_CANNOTSENDTOCHAN (404)
- [x] L'expéditeur ne reçoit pas son propre message de canal

### Améliorations Suggérées
- [ ] Distinguer ERR_NORECIPIENT (411) et ERR_NOTEXTTOSEND (412)
- [ ] Format d'erreur plus conforme RFC
- [ ] Gestion des messages multi-cibles (optionnel)

---

## Bugs Connus / Points d'Attention

### 1. Format d'Erreur 411
**Actuel** :
```cpp
this->sendError(411, "Wrong parameters");
```

**Conforme RFC** :
```cpp
this->sendError(411, ":No recipient given (PRIVMSG)");
```

### 2. Validation du Message
**Actuel** : Vérifie que `params[1][0] == ':'`

**Problème potentiel** : Si le parser a déjà traité le `:`, cette vérification peut échouer.

### 3. Distinction 411 vs 412
Le code actuel ne distingue pas clairement :
- 411 : Pas de destinataire
- 412 : Pas de texte à envoyer

---

## Checklist de Revue

Pour chaque test, vérifier :

- [ ] Le test compile sans erreur
- [ ] Le test s'exécute sans crash
- [ ] Le résultat correspond à l'attendu
- [ ] Pas de fuites mémoire (valgrind)
- [ ] Le format des messages est correct
- [ ] Les erreurs IRC ont le bon code

---

## Auteur et Date

- **Équipe** : ft_irc
- **Date** : 23 janvier 2026
- **Version** : 1.0
