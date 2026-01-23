# INVITE Command - Documentation des Tests

## Vue d'ensemble

La commande **INVITE** permet d'inviter des utilisateurs dans des canaux. C'est une fonctionnalité essentielle pour les canaux sur invitation uniquement (+i).

**Syntaxe IRC** : `INVITE <pseudo> <canal>`

---

## Codes d'Erreur IRC

| Code | Nom | Description |
|------|-----|-------------|
| 341 | RPL_INVITING | `<channel> <nick>` - Confirmation d'invitation |
| 401 | ERR_NOSUCHNICK | `<nick> :No such nick/channel` - Utilisateur inexistant |
| 403 | ERR_NOSUCHCHANNEL | `<channel> :No such channel` - Canal inexistant |
| 442 | ERR_NOTONCHANNEL | `<channel> :You're not on that channel` - Pas membre |
| 443 | ERR_USERONCHANNEL | `<user> <channel> :is already on channel` - Déjà présent |
| 451 | ERR_NOTREGISTERED | `:You have not registered` - Non enregistré |
| 461 | ERR_NEEDMOREPARAMS | `<command> :Not enough parameters` - Paramètres manquants |
| 482 | ERR_CHANOPRIVSNEEDED | `<channel> :You're not channel operator` - Pas opérateur |

---

## Compilation

```bash
# Depuis le dossier tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_invite_command.cpp ../src/Server.cpp ../src/Client.cpp \
    ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
    ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
    -o test_invite_command
```

---

## Exécution

```bash
# Tous les tests
./test_invite_command

# Test spécifique (1-20)
./test_invite_command 5

# Aide
./test_invite_command help

# Instructions manuelles
./test_invite_command manual

# Instructions valgrind
./test_invite_command valgrind
```

---

## Liste des Tests

### Tests d'Enregistrement (1-2)

#### Test 01: INVITE Before Registration (ERR_NOTREGISTERED 451)
**Objectif** : Vérifier qu'un client non enregistré ne peut pas inviter.

**Préconditions** :
- Client connecté mais non enregistré

**Entrée** :
```
INVITE bob #general
```

**Sortie attendue** :
```
:server 451 * :You have not registered
```

**Vérifications** :
- [ ] Erreur 451 retournée
- [ ] Invitation non effectuée

---

#### Test 02: INVITE After Partial Registration
**Objectif** : Vérifier le comportement après PASS mais avant NICK/USER complet.

**Préconditions** :
- Client a envoyé PASS mais registration incomplète

**Entrée** :
```
PASS testpass
INVITE bob #general
```

**Sortie attendue** :
```
:server 451 * :You have not registered
```

---

### Tests de Validation des Paramètres (3-5)

#### Test 03: INVITE Without Parameters (ERR_NEEDMOREPARAMS 461)
**Objectif** : Vérifier l'erreur quand aucun paramètre n'est fourni.

**Entrée** :
```
INVITE
```

**Sortie attendue** :
```
:server 461 nick INVITE :Not enough parameters
```

**Note** : L'implémentation actuelle retourne "JOIN :Not enough parameters" - à corriger.

---

#### Test 04: INVITE With Only Nickname
**Objectif** : Vérifier l'erreur quand seul le pseudo est fourni.

**Entrée** :
```
INVITE bob
```

**Sortie attendue** :
```
:server 461 nick INVITE :Not enough parameters
```

---

#### Test 05: INVITE With Empty Parameters
**Objectif** : Vérifier le comportement avec des paramètres vides.

**Entrée** :
```
INVITE "" #general
INVITE bob ""
```

**Sortie attendue** :
- Erreur appropriée pour paramètre invalide

---

### Tests de Validation du Canal (6-8)

#### Test 06: INVITE to Non-existent Channel (ERR_NOSUCHCHANNEL 403)
**Objectif** : Inviter vers un canal qui n'existe pas.

**Entrée** :
```
INVITE bob #nonexistent
```

**Sortie attendue** :
```
:server 403 nick #nonexistent :No such channel
```

---

#### Test 07: INVITE When Not Member (ERR_NOTONCHANNEL 442)
**Objectif** : Inviter depuis un canal dont on n'est pas membre.

**Préconditions** :
- Canal #private existe
- Client n'est PAS membre de #private

**Entrée** :
```
INVITE bob #private
```

**Sortie attendue** :
```
:server 442 nick #private :You're not on that channel
```

---

#### Test 08: INVITE to Channel with & Prefix
**Objectif** : Tester l'invitation vers un canal local (&).

**Entrée** :
```
INVITE bob &local
```

**Sortie attendue** :
- Si &local existe et membre → invitation réussie
- Sinon → ERR_NOSUCHCHANNEL ou ERR_NOTONCHANNEL

---

### Tests de Permissions (9-12)

#### Test 09: INVITE on Regular Channel - Member
**Objectif** : Membre non-opérateur invite sur un canal normal.

**Préconditions** :
- Canal #general sans mode +i
- alice est membre (non-opérateur)
- bob n'est pas membre

**Entrée (alice)** :
```
INVITE bob #general
```

**Sortie attendue** :
```
:server 341 alice bob #general
```
Et bob reçoit :
```
:alice!alice@host INVITE bob #general
```

**Vérification** :
- [ ] N'importe quel membre peut inviter sur un canal normal

---

#### Test 10: INVITE on +i Channel - Operator
**Objectif** : Opérateur invite sur un canal +i.

**Préconditions** :
- Canal #private avec mode +i
- alice est opérateur
- bob n'est pas membre

**Entrée (alice)** :
```
INVITE bob #private
```

**Sortie attendue** :
```
:server 341 alice bob #private
```
Et bob reçoit le message INVITE.

---

#### Test 11: INVITE on +i Channel - Non-Operator (ERR_CHANOPRIVSNEEDED 482)
**Objectif** : Non-opérateur tente d'inviter sur canal +i.

**Préconditions** :
- Canal #private avec mode +i
- charlie est membre mais PAS opérateur
- bob n'est pas membre

**Entrée (charlie)** :
```
INVITE bob #private
```

**Sortie attendue** :
```
:server 482 charlie #private :You're not channel operator
```

---

#### Test 12: INVITE User Already on Channel (ERR_USERONCHANNEL 443)
**Objectif** : Inviter quelqu'un déjà présent dans le canal.

**Préconditions** :
- bob est déjà membre de #general

**Entrée** :
```
INVITE bob #general
```

**Sortie attendue** :
```
:server 443 nick bob #general :is already on channel
```

---

### Tests de Validation de l'Utilisateur (13-15)

#### Test 13: INVITE Non-existent User (ERR_NOSUCHNICK 401)
**Objectif** : Inviter un utilisateur qui n'existe pas.

**Entrée** :
```
INVITE unknownuser #general
```

**Sortie attendue** :
```
:server 401 nick unknownuser :No such nick/channel
```

---

#### Test 14: INVITE Self
**Objectif** : S'inviter soi-même (cas limite).

**Entrée (alice dans #general)** :
```
INVITE alice #general
```

**Sortie attendue** :
```
:server 443 alice alice #general :is already on channel
```

---

#### Test 15: INVITE Case Sensitivity (Nicknames)
**Objectif** : Tester la sensibilité à la casse des pseudos.

**Setup** :
- bob est connecté (minuscules)

**Entrée** :
```
INVITE BOB #general
INVITE Bob #general
INVITE bob #general
```

**Vérification** :
- [ ] IRC standard : insensible à la casse pour les pseudos

---

### Tests de Fonctionnalité (16-18)

#### Test 16: INVITE Success - Message Format
**Objectif** : Vérifier le format exact des messages.

**Après INVITE bob #general** :

**À l'inviteur (RPL_INVITING 341)** :
```
:server 341 alice bob #general
```

**À l'invité** :
```
:alice!alice@host INVITE bob #general\r\n
```

**Composants à vérifier** :
| Élément | Valeur |
|---------|--------|
| Préfixe | `:alice!alice@host` |
| Commande | `INVITE` |
| Target | `bob` |
| Channel | `#general` |
| Terminaison | `\r\n` |

---

#### Test 17: INVITE and JOIN - Integration
**Objectif** : L'utilisateur invité peut rejoindre un canal +i.

**Setup** :
1. Canal #private avec mode +i
2. alice est opérateur
3. bob n'est pas membre

**Test** :
```
# alice invite bob
INVITE bob #private

# bob essaie de rejoindre
JOIN #private
```

**Vérifications** :
- [ ] bob reçoit le message INVITE
- [ ] bob peut rejoindre #private après invitation
- [ ] Sans invitation, bob recevrait ERR_INVITEONLYCHAN (473)

---

#### Test 18: INVITE Clears After JOIN
**Objectif** : L'invitation est effacée après utilisation.

**Test** :
1. alice invite bob dans #private (+i)
2. bob JOIN #private → succès
3. bob PART #private
4. bob JOIN #private → échec (473)

**Vérification** :
- [ ] L'invitation est à usage unique

---

### Tests Avancés (19-20)

#### Test 19: Multiple Invitations Same User
**Objectif** : Inviter le même utilisateur plusieurs fois.

**Test** :
```
INVITE bob #general
INVITE bob #general
INVITE bob #general
```

**Comportement attendu** :
- Chaque INVITE envoie un message à bob
- Pas d'erreur (ou comportement défini)

---

#### Test 20: INVITE Stress Test
**Objectif** : Tester les invitations sous stress.

**Test** :
- Inviter 50 utilisateurs rapidement
- Vérifier que toutes les invitations sont traitées
- Pas de crash ou fuite mémoire

---

## Tests Manuels avec netcat

### Setup Initial
```bash
# Terminal 1 - Serveur
./ircserv 6667 testpass

# Terminal 2 - Client 1 (alice - opérateur)
nc -C localhost 6667

# Terminal 3 - Client 2 (bob - invité)
nc -C localhost 6667

# Terminal 4 - Client 3 (charlie - membre)
nc -C localhost 6667
```

### Test Manuel 1: Invitation Simple
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

# Terminal 2 (alice) - invite bob
INVITE bob #general

# Vérifier sur Terminal 2: Reçoit RPL_INVITING (341)
# Vérifier sur Terminal 3: Reçoit le message INVITE

# Terminal 3 (bob) - rejoint après invitation
JOIN #general
# Vérifier: bob rejoint avec succès
```

### Test Manuel 2: Canal Invite-Only (+i)
```bash
# Terminal 2 (alice) - créer canal +i
JOIN #private
# alice est automatiquement opérateur

# Définir le mode +i (nécessite ModeCommand implémenté)
MODE #private +i

# Terminal 3 (bob) - essayer de rejoindre sans invitation
JOIN #private
# Attendu: 473 #private :Cannot join channel (+i)

# Terminal 2 (alice) - inviter bob
INVITE bob #private
# Attendu sur Terminal 2: 341 alice bob #private
# Attendu sur Terminal 3: :alice!alice@host INVITE bob #private

# Terminal 3 (bob) - rejoindre après invitation
JOIN #private
# Attendu: bob rejoint avec succès
```

### Test Manuel 3: Erreurs de Permissions
```bash
# Terminal 4 (charlie) - rejoint #private comme membre
# (après avoir été invité par alice)

# Terminal 4 (charlie) - essayer d'inviter (non-opérateur)
INVITE bob #private
# Attendu: 482 charlie #private :You're not channel operator
```

### Test Manuel 4: Erreurs Diverses
```bash
# Utilisateur inexistant
INVITE unknownuser #general
# Attendu: 401 nick unknownuser :No such nick/channel

# Canal inexistant
INVITE bob #nonexistent
# Attendu: 403 nick #nonexistent :No such channel

# Pas membre du canal
INVITE bob #otherchannel
# Attendu: 442 nick #otherchannel :You're not on that channel

# Utilisateur déjà dans le canal
INVITE alice #general
# Attendu: 443 nick alice #general :is already on channel
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

# Inviter un utilisateur
/invite bob #general

# Créer un canal invite-only
/join #private
/mode #private +i
/invite bob #private
```

---

## Instructions Valgrind

```bash
# Test de base
valgrind --leak-check=full ./test_invite_command

# Test détaillé
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./test_invite_command

# Test spécifique
valgrind --leak-check=full ./test_invite_command 10

# Vérifier la sortie pour:
# - "definitely lost: 0 bytes"
# - "indirectly lost: 0 bytes"
# - Pas d'erreurs "Invalid read/write"
```

---

## Critères d'Acceptation

### Fonctionnalités Obligatoires
- [x] Vérification de l'enregistrement du client
- [x] Validation des paramètres (pseudo + canal)
- [x] Vérification de l'existence du canal
- [x] Vérification que l'inviteur est membre
- [x] Vérification des permissions (+i → opérateur requis)
- [x] Vérification que la cible existe
- [x] Vérification que la cible n'est pas déjà membre
- [x] Ajout à la liste des invités
- [x] Envoi RPL_INVITING à l'inviteur
- [x] Envoi message INVITE à l'invité

### Points d'Amélioration
- [ ] Message d'erreur 461 devrait mentionner "INVITE" pas "JOIN"

---

## Bugs Connus / Points d'Attention

### 1. Message d'Erreur 461
**Actuel** :
```cpp
sendError(461, "JOIN :Not enough parameters");
```

**Corrigé** :
```cpp
sendError(461, "INVITE :Not enough parameters");
```

### 2. Double Point-virgule
**Actuel** :
```cpp
std::string targetNick = params[0];;  // Double ;;
```

**Corrigé** :
```cpp
std::string targetNick = params[0];
```

---

## Matrice de Permissions

| Mode Canal | Rôle | Peut Inviter? |
|------------|------|---------------|
| Normal | Opérateur | ✅ Oui |
| Normal | Membre | ✅ Oui |
| +i | Opérateur | ✅ Oui |
| +i | Membre | ❌ Non (482) |

---

## Checklist de Revue

Pour chaque test, vérifier :

- [ ] Le test compile sans erreur
- [ ] Le test s'exécute sans crash
- [ ] Le résultat correspond à l'attendu
- [ ] Pas de fuites mémoire (valgrind)
- [ ] Le format des messages est correct
- [ ] Les erreurs IRC ont le bon code
- [ ] L'invitation est bien ajoutée à la liste
- [ ] L'utilisateur invité peut rejoindre le canal +i

---

## Auteur et Date

- **Équipe** : ft_irc
- **Date** : 23 janvier 2026
- **Version** : 1.0
