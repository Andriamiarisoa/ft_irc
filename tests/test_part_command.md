# PartCommand Test Suite Documentation

> **Version:** 1.0  
> **Author:** ft_irc team  
> **Last Updated:** 2026-01-23  
> **File:** `tests/test_part_command.cpp`

---

## 📋 Overview

Ce document décrit tous les cas de tests pour `PartCommand.cpp`. La commande PART permet aux utilisateurs de quitter un ou plusieurs canaux IRC. Utilisez ce guide comme référence pour tester et valider l'implémentation.

---

## 🔧 Compilation

```bash
# Depuis la racine du projet
cd tests

# Compiler la suite de tests
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_part_command.cpp ../src/Server.cpp ../src/Client.cpp \
    ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
    ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
    -o test_part_command
```

---

## 🚀 Usage

```bash
# Lancer tous les tests
./test_part_command

# Lancer un test spécifique (1-20)
./test_part_command 5

# Afficher l'aide
./test_part_command help

# Instructions valgrind
./test_part_command valgrind

# Instructions tests manuels
./test_part_command manual
```

---

## ✅ Test Cases Summary

| # | Nom du Test | Type | Status | Dépendances |
|---|-------------|------|--------|-------------|
| 01 | PartCommand Construction | Unit | ⬜ | None |
| 02 | PartCommand Destruction | Unit | ⬜ | None |
| 03 | PART Sans Paramètre | Unit | ⬜ | None |
| 04 | PART Canal Inexistant | Unit | ⬜ | None |
| 05 | PART Non Membre du Canal | Unit | ⬜ | Channel |
| 06 | PART Canal Simple | Unit | ⬜ | Channel |
| 07 | PART Avec Raison | Unit | ⬜ | Channel |
| 08 | PART Canaux Multiples | Unit | ⬜ | Channel |
| 09 | PART Avant Enregistrement | Unit | ⬜ | None |
| 10 | PART Diffusion aux Membres | Unit | ⬜ | Channel |
| 11 | PART Supprime Canal Vide | Unit | ⬜ | Channel |
| 12 | PART Opérateur Quitte | Unit | ⬜ | Channel |
| 13 | PART Format Message Correct | Unit | ⬜ | Channel |
| 14 | PART Raison Caractères Spéciaux | Unit | ⬜ | Channel |
| 15 | PART Mixte (Succès/Erreurs) | Unit | ⬜ | Channel |
| 16 | PART Case Insensitive | Unit | ⬜ | Channel |
| 17 | PART Client Retiré de Tous Sets | Unit | ⬜ | Channel |
| 18 | PART Memory Leaks | Valgrind | ⬜ | None |
| 19 | PART Concurrent (Multi-Clients) | Manual | ⬜ | Channel |
| 20 | PART Stress Test | Unit | ⬜ | Channel |

**Légende:** 
- ⬜ Non testé | ✅ Passé | ❌ Échoué
- **Channel** = Nécessite la classe Channel entièrement implémentée

---

## 📝 Descriptions Détaillées des Tests

### TEST 01: PartCommand Construction
**Type:** Test Unitaire  
**Objectif:** Vérifier que PartCommand peut être construit correctement.

| Test | Description | Attendu |
|------|-------------|---------|
| 1.1 | Créer PartCommand avec params vides | Pas de crash |
| 1.2 | Créer PartCommand avec un canal | Objet créé |
| 1.3 | Créer PartCommand avec canal + raison | Objet créé |
| 1.4 | Créer PartCommand avec plusieurs canaux | Objet créé |

**Résultat Attendu:** Pas de crash, objet créé correctement.

---

### TEST 02: PartCommand Destruction
**Type:** Test Unitaire (+ Valgrind)  
**Objectif:** Vérifier que le destructeur libère correctement la mémoire.

| Test | Description | Attendu |
|------|-------------|---------|
| 2.1 | Destruction simple | Pas de crash |
| 2.2 | 100 cycles création/destruction | Pas de crash |
| 2.3 | Destruction après execute() | Pas de fuite mémoire |

**Commande Valgrind:**
```bash
valgrind --leak-check=full ./test_part_command 2
```

---

### TEST 03: PART Sans Paramètre
**Type:** Test Unitaire  
**Objectif:** Vérifier que PART sans canal retourne ERR_NEEDMOREPARAMS.

**Entrée:**
```
PART
```

**Sortie Attendue:**
```
:irc.example.com 461 nickname :PART :Not enough parameters
```

**Code Erreur:** 461 (ERR_NEEDMOREPARAMS)

---

### TEST 04: PART Canal Inexistant
**Type:** Test Unitaire  
**Objectif:** Vérifier que PART sur un canal inexistant retourne ERR_NOSUCHCHANNEL.

**Entrée:**
```
PART #nonexistent
```

**Sortie Attendue:**
```
:irc.example.com 403 nickname #nonexistent :No such channel
```

**Code Erreur:** 403 (ERR_NOSUCHCHANNEL)

---

### TEST 05: PART Non Membre du Canal
**Type:** Test Unitaire  
**Objectif:** Vérifier que PART sur un canal dont on n'est pas membre retourne ERR_NOTONCHANNEL.

**Pré-requis:** Canal #test existe mais client n'en fait pas partie

**Entrée:**
```
PART #test
```

**Sortie Attendue:**
```
:irc.example.com 442 nickname #test :You're not on that channel
```

**Code Erreur:** 442 (ERR_NOTONCHANNEL)

---

### TEST 06: PART Canal Simple
**Type:** Test Unitaire  
**Objectif:** Vérifier qu'un PART basique fonctionne correctement.

**Pré-requis:** Client est membre de #general

**Entrée:**
```
PART #general
```

**Sortie Attendue (diffusée à tous):**
```
:nickname!username@host PART #general
```

**Vérifications:**
- ✅ Client retiré des membres du canal
- ✅ Canal retiré de la liste des canaux du client
- ✅ Message diffusé à tous les membres (y compris le partant)

---

### TEST 07: PART Avec Raison
**Type:** Test Unitaire  
**Objectif:** Vérifier que la raison de départ est incluse dans le message.

**Entrée:**
```
PART #general :Going to sleep
```

**Sortie Attendue:**
```
:nickname!username@host PART #general :Going to sleep
```

---

### TEST 08: PART Canaux Multiples
**Type:** Test Unitaire  
**Objectif:** Vérifier que PART peut quitter plusieurs canaux simultanément.

**Pré-requis:** Client est membre de #foo, #bar, #baz

**Entrée:**
```
PART #foo,#bar,#baz :Leaving all
```

**Sorties Attendues:**
```
:nickname!username@host PART #foo :Leaving all
:nickname!username@host PART #bar :Leaving all
:nickname!username@host PART #baz :Leaving all
```

**Vérifications:**
- ✅ Chaque canal traité indépendamment
- ✅ La même raison appliquée à tous
- ✅ Erreurs individuelles n'arrêtent pas le traitement

---

### TEST 09: PART Avant Enregistrement
**Type:** Test Unitaire  
**Objectif:** Vérifier que PART avant enregistrement complet retourne ERR_NOTREGISTERED.

**Pré-requis:** Client n'a pas complété PASS/NICK/USER

**Entrée:**
```
PART #general
```

**Sortie Attendue:**
```
:irc.example.com 451 * ::You have not registered
```

**Code Erreur:** 451 (ERR_NOTREGISTERED)

---

### TEST 10: PART Diffusion aux Membres
**Type:** Test Unitaire  
**Objectif:** Vérifier que le message PART est diffusé à TOUS les membres.

**Scénario:**
1. Alice, Bob, Charlie sont dans #general
2. Alice exécute PART #general

**Vérifications:**
- ✅ Alice reçoit le message PART
- ✅ Bob reçoit le message PART
- ✅ Charlie reçoit le message PART
- ✅ Le message est envoyé AVANT de retirer Alice

---

### TEST 11: PART Supprime Canal Vide
**Type:** Test Unitaire  
**Objectif:** Vérifier que le canal est supprimé quand le dernier membre part.

**Scénario:**
1. Alice est seule dans #private
2. Alice exécute PART #private

**Vérifications:**
- ✅ Canal #private supprimé du serveur
- ✅ Mémoire libérée (vérifier avec valgrind)
- ✅ server->getChannel("#private") retourne NULL

---

### TEST 12: PART Opérateur Quitte
**Type:** Test Unitaire  
**Objectif:** Vérifier le comportement quand un opérateur quitte.

**Scénario:**
1. Alice (op) et Bob dans #test
2. Alice exécute PART #test

**Vérifications:**
- ✅ Alice retirée des opérateurs
- ✅ Alice retirée des membres
- ✅ Le canal reste (Bob est encore là)
- ✅ Bob pourrait devenir op (comportement optionnel)

---

### TEST 13: PART Format Message Correct
**Type:** Test Unitaire  
**Objectif:** Vérifier le format exact du message PART selon RFC.

**Format Sans Raison:**
```
:nick!user@host PART #channel\r\n
```

**Format Avec Raison:**
```
:nick!user@host PART #channel :reason here\r\n
```

**Vérifications:**
- ✅ Préfixe correct (:nick!user@host)
- ✅ Commande PART en majuscules
- ✅ Nom du canal inclus
- ✅ Raison précédée de ":"
- ✅ Terminé par \r\n

---

### TEST 14: PART Raison Caractères Spéciaux
**Type:** Test Unitaire  
**Objectif:** Vérifier que les caractères spéciaux dans la raison sont gérés.

**Entrées à tester:**
```
PART #test :Goodbye! 🎉
PART #test :Raison avec "quotes"
PART #test :Line with : colons
PART #test :   Spaces around   
```

**Vérifications:**
- ✅ Caractères préservés dans le message
- ✅ Pas de corruption de données
- ✅ Pas de crash

---

### TEST 15: PART Mixte (Succès/Erreurs)
**Type:** Test Unitaire  
**Objectif:** Vérifier le comportement avec des canaux valides et invalides.

**Pré-requis:** Client membre de #foo uniquement

**Entrée:**
```
PART #foo,#nonexistent,#notmember
```

**Sorties Attendues:**
```
:nick!user@host PART #foo
:irc.example.com 403 nick #nonexistent :No such channel
:irc.example.com 442 nick #notmember :You're not on that channel
```

**Vérifications:**
- ✅ #foo quitté avec succès
- ✅ Erreur 403 pour #nonexistent
- ✅ Erreur 442 pour #notmember
- ✅ Traitement continue après erreurs

---

### TEST 16: PART Case Insensitive
**Type:** Test Unitaire  
**Objectif:** Vérifier que les noms de canaux sont insensibles à la casse.

**Scénario:** Client membre de #General

**Entrées équivalentes:**
```
PART #General
PART #general
PART #GENERAL
PART #gEnErAl
```

**Vérifications:**
- ✅ Toutes les variantes trouvent le canal
- ✅ Client correctement retiré

---

### TEST 17: PART Client Retiré de Tous Sets
**Type:** Test Unitaire  
**Objectif:** Vérifier que le client est retiré de tous les ensembles.

**Vérifications après PART:**
- ✅ Retiré de channel->members
- ✅ Retiré de channel->operators (si applicable)
- ✅ Retiré de channel->invitedUsers (si applicable)
- ✅ Channel retiré de client->channels

---

### TEST 18: PART Memory Leaks
**Type:** Valgrind  
**Objectif:** Vérifier qu'il n'y a pas de fuites mémoire.

**Commande:**
```bash
valgrind --leak-check=full --show-leak-kinds=all \
    --track-origins=yes ./test_part_command
```

**Vérifications:**
- ✅ 0 bytes definitely lost
- ✅ 0 bytes indirectly lost
- ✅ Pas d'erreurs d'accès mémoire

---

### TEST 19: PART Concurrent (Multi-Clients)
**Type:** Test Manuel  
**Objectif:** Vérifier le comportement avec plusieurs clients.

**Scénario:**
1. Client1 et Client2 dans #test
2. Les deux exécutent PART #test simultanément

**Vérifications:**
- ✅ Pas de race condition
- ✅ Pas de crash
- ✅ Les deux clients reçoivent les messages appropriés

---

### TEST 20: PART Stress Test
**Type:** Test Unitaire  
**Objectif:** Tester les limites de la commande PART.

**Tests:**
| Test | Description | Attendu |
|------|-------------|---------|
| 20.1 | PART de 50 canaux simultanément | Tous traités |
| 20.2 | Raison de 500 caractères | Gérée (peut être tronquée) |
| 20.3 | 100 JOIN/PART cycles rapides | Pas de fuite mémoire |
| 20.4 | Nom de canal maximum (50 chars) | Accepté |

---

## 📊 Codes d'erreur IRC

| Code | Nom | Description |
|------|-----|-------------|
| 403 | ERR_NOSUCHCHANNEL | Canal inexistant |
| 442 | ERR_NOTONCHANNEL | Pas membre du canal |
| 451 | ERR_NOTREGISTERED | Client non enregistré |
| 461 | ERR_NEEDMOREPARAMS | Paramètres manquants |

---

## 🧪 Tests Manuels avec Netcat

### Préparation
```bash
# Terminal 1: Lancer le serveur
./ircserv 6667 testpass

# Terminal 2: Client de test
nc -C localhost 6667
```

### Test 1: PART Basique
```bash
PASS testpass
NICK alice
USER alice 0 * :Alice
JOIN #test
PART #test
# Attendu: :alice!alice@host PART #test
```

### Test 2: PART Avec Raison
```bash
PASS testpass
NICK bob
USER bob 0 * :Bob
JOIN #general
PART #general :See you later!
# Attendu: :bob!bob@host PART #general :See you later!
```

### Test 3: PART Canal Inexistant
```bash
PASS testpass
NICK charlie
USER charlie 0 * :Charlie
PART #nonexistent
# Attendu: :irc.example.com 403 charlie #nonexistent :No such channel
```

### Test 4: PART Canaux Multiples
```bash
PASS testpass
NICK dave
USER dave 0 * :Dave
JOIN #foo
JOIN #bar
PART #foo,#bar :Leaving both
# Attendu: Messages PART pour les deux canaux
```

### Test 5: PART Sans Être Membre
```bash
# Terminal 2 (après que terminal 3 ait créé #private)
PASS testpass
NICK eve
USER eve 0 * :Eve
PART #private
# Attendu: :irc.example.com 442 eve #private :You're not on that channel
```

### Test 6: Vérifier Diffusion Multi-Client
```bash
# Terminal 2: Alice
PASS testpass
NICK alice
USER alice 0 * :Alice
JOIN #team

# Terminal 3: Bob
PASS testpass
NICK bob
USER bob 0 * :Bob
JOIN #team

# Terminal 2: Alice quitte
PART #team :Goodbye team!
# Attendu sur Terminal 3: :alice!alice@host PART #team :Goodbye team!
```

---

## 🔄 Différences PART vs QUIT

| Aspect | PART | QUIT |
|--------|------|------|
| Portée | Canaux spécifiques | Tous les canaux |
| Connexion | Maintenue | Fermée |
| Message | Par canal | Global |
| Raison | Optionnelle | Optionnelle |
| Nettoyage | Canal uniquement | Complet |

---

## ⚠️ Problèmes Courants

### Le message PART n'est pas reçu par le client qui part
- **Cause:** broadcast() appelé APRÈS removeMember()
- **Solution:** Appeler broadcast() AVANT removeMember()

### Le canal n'est pas supprimé après le départ du dernier membre
- **Cause:** Vérification du count manquante
- **Solution:** Ajouter `if (channel->getMembers().empty()) server->removeChannel()`

### Erreur de segmentation avec canal NULL
- **Cause:** getChannel() retourne NULL non vérifié
- **Solution:** Toujours vérifier si channel != NULL

### Fuite mémoire sur les canaux vides
- **Cause:** delete non appelé avant erase()
- **Solution:** `delete channel; channels.erase(it);`

---

## 📁 Fichiers Associés

| Fichier | Rôle |
|---------|------|
| `PartCommand.hpp/cpp` | Implémentation de la commande |
| `Server.hpp/cpp` | getChannel(), removeChannel() |
| `Client.hpp/cpp` | removeFromChannel() |
| `Channel.hpp/cpp` | removeMember(), isMember(), broadcast() |
| `JoinCommand.cpp` | Commande inverse (rejoindre) |
| `QuitCommand.cpp` | Quitter le serveur entier |

---

## ✏️ Checklist de Review

Utilisez cette checklist lors de la review du code :

### Fonctionnalité
- [ ] PART sans param → Erreur 461
- [ ] PART canal inexistant → Erreur 403
- [ ] PART non membre → Erreur 442
- [ ] PART avant registration → Erreur 451
- [ ] PART canal simple → Succès + broadcast
- [ ] PART avec raison → Raison dans message
- [ ] PART multi-canaux → Tous traités

### Qualité du Code
- [ ] Pas de memory leaks (valgrind clean)
- [ ] Gestion des pointeurs NULL
- [ ] Messages formatés selon RFC
- [ ] Noms de canaux case-insensitive
- [ ] Canaux vides supprimés

### Tests Passés
- [ ] Tous les tests unitaires passent
- [ ] Tests manuels validés
- [ ] Valgrind sans erreurs

---

## 🎯 Critères d'Acceptation

La commande PART est considérée **COMPLÈTE** quand :

1. ✅ Tous les tests 01-20 passent
2. ✅ Valgrind rapporte 0 fuites
3. ✅ Compatible avec un client IRC réel (irssi, HexChat)
4. ✅ Review code approuvée par l'équipe
5. ✅ Tests manuels validés sur 3 scénarios minimum
