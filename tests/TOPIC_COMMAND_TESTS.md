# TopicCommand Test Suite Documentation

> **Version:** 1.0  
> **Author:** ft_irc team  
> **Last Updated:** 2026-01-23  
> **File:** `tests/test_topic_command.cpp`

---

## 📋 Vue d'ensemble

Ce document décrit tous les cas de tests pour `TopicCommand.cpp`. La commande TOPIC permet aux utilisateurs de voir et définir le sujet (topic) d'un canal IRC. Les sujets peuvent être protégés avec le mode `+t` (opérateurs uniquement).

**Syntaxe IRC:**
- **Voir le sujet:** `TOPIC #canal`
- **Définir le sujet:** `TOPIC #canal :Nouveau message de sujet`
- **Effacer le sujet:** `TOPIC #canal :`

---

## 🔧 Compilation

```bash
# Depuis la racine du projet
cd tests

# Compiler la suite de tests
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_topic_command.cpp ../src/Server.cpp ../src/Client.cpp \
    ../src/Channel.cpp ../src/MessageParser.cpp ../src/Command.cpp \
    ../src/QuitCommand.cpp ../src/PassCommand.cpp ../src/NickCommand.cpp \
    ../src/UserCommand.cpp ../src/JoinCommand.cpp ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp ../src/KickCommand.cpp ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp ../src/ModeCommand.cpp \
    -o test_topic_command
```

---

## 🚀 Usage

```bash
# Lancer tous les tests
./test_topic_command

# Lancer un test spécifique (1-25)
./test_topic_command 5

# Afficher l'aide
./test_topic_command help

# Instructions tests manuels
./test_topic_command manual

# Instructions valgrind
./test_topic_command valgrind
```

---

## 📊 Codes IRC Utilisés

### Codes d'Erreur

| Code | Nom | Description |
|------|-----|-------------|
| 403 | ERR_NOSUCHCHANNEL | `<canal> :No such channel` |
| 442 | ERR_NOTONCHANNEL | `<canal> :You're not on that channel` |
| 451 | ERR_NOTREGISTERED | `:You have not registered` |
| 461 | ERR_NEEDMOREPARAMS | `<commande> :Not enough parameters` |
| 482 | ERR_CHANOPRIVSNEEDED | `<canal> :You're not channel operator` |

### Codes de Succès

| Code | Nom | Description |
|------|-----|-------------|
| 331 | RPL_NOTOPIC | `<canal> :No topic is set` |
| 332 | RPL_TOPIC | `<canal> :<sujet>` |
| 333 | RPL_TOPICWHOTIME | `<canal> <qui> <heure>` (optionnel) |

---

## ✅ Résumé des Tests

| # | Nom du Test | Type | Status | Dépendances |
|---|-------------|------|--------|-------------|
| 01 | TopicCommand Construction | Unit | ⬜ | None |
| 02 | TopicCommand Destruction | Unit | ⬜ | None |
| 03 | TOPIC Avant Enregistrement | Unit | ⬜ | None |
| 04 | TOPIC Sans Paramètre | Unit | ⬜ | None |
| 05 | TOPIC Mauvais Format Paramètres | Unit | ⬜ | None |
| 06 | TOPIC Canal Inexistant | Unit | ⬜ | Server |
| 07 | TOPIC Non Membre du Canal | Unit | ⬜ | Channel |
| 08 | TOPIC Voir - Pas de Sujet | Unit | ⬜ | Channel |
| 09 | TOPIC Voir - Sujet Existe | Unit | ⬜ | Channel |
| 10 | TOPIC Définir - Membre Normal | Unit | ⬜ | Channel |
| 11 | TOPIC Définir - Opérateur Normal | Unit | ⬜ | Channel |
| 12 | TOPIC Définir - Canal +t Non-Op | Unit | ⬜ | Channel |
| 13 | TOPIC Définir - Canal +t Opérateur | Unit | ⬜ | Channel |
| 14 | TOPIC Effacer - Sujet Vide | Unit | ⬜ | Channel |
| 15 | TOPIC Diffusion à Tous les Membres | Unit | ⬜ | Channel |
| 16 | TOPIC Préservation Multi-Mots | Unit | ⬜ | Channel |
| 17 | TOPIC Caractères Spéciaux | Unit | ⬜ | Channel |
| 18 | TOPIC Avec Deux-Points Initial | Unit | ⬜ | Channel |
| 19 | TOPIC Sujet Très Long | Unit | ⬜ | Channel |
| 20 | TOPIC Vérification Format Message | Unit | ⬜ | None |
| 21 | TOPIC Changements Consécutifs | Unit | ⬜ | Channel |
| 22 | TOPIC Fuites Mémoire | Valgrind | ⬜ | None |
| 23 | TOPIC Multi-Client Concurrent | Manual | ⬜ | Server |
| 24 | TOPIC Test de Stress | Unit | ⬜ | Channel |
| 25 | TOPIC Intégration avec JOIN | Unit | ⬜ | Channel |

**Légende:** 
- ⬜ Non testé | ✅ Passé | ❌ Échoué
- **Channel** = Nécessite la classe Channel
- **Server** = Nécessite la classe Server
- **Manual** = Test manuel requis

---

## 📝 Descriptions Détaillées des Tests

### TEST 01: TopicCommand Construction
**Type:** Test Unitaire  
**Objectif:** Vérifier que TopicCommand peut être construit correctement.

| Test | Description | Attendu |
|------|-------------|---------|
| 1.1 | Créer TopicCommand avec params vides | Pas de crash |
| 1.2 | Créer TopicCommand avec un canal (view) | Objet créé |
| 1.3 | Créer TopicCommand avec canal + sujet (set) | Objet créé |
| 1.4 | Créer TopicCommand avec canal + ":" (clear) | Objet créé |

---

### TEST 02: TopicCommand Destruction
**Type:** Test Unitaire (+ Valgrind)  
**Objectif:** Vérifier que le destructeur libère correctement la mémoire.

**Commande Valgrind:**
```bash
valgrind --leak-check=full ./test_topic_command 2
```

---

### TEST 03: TOPIC Avant Enregistrement
**Type:** Test Unitaire  
**Objectif:** Vérifier que TOPIC échoue si le client n'est pas enregistré.

**Entrée:**
```
TOPIC #test
```

**Sortie Attendue:**
```
:server 451 * :You have not registered
```

**Code Erreur:** 451 (ERR_NOTREGISTERED)

---

### TEST 04: TOPIC Sans Paramètre
**Type:** Test Unitaire  
**Objectif:** Vérifier que TOPIC sans canal retourne ERR_NEEDMOREPARAMS.

**Entrée:**
```
TOPIC
```

**Sortie Attendue:**
```
:server 461 nick TOPIC :Not enough parameters
```

**Code Erreur:** 461 (ERR_NEEDMOREPARAMS)

---

### TEST 05: TOPIC Mauvais Format Paramètres
**Type:** Test Unitaire  
**Objectif:** Vérifier que TOPIC avec un sujet sans ':' initial est rejeté.

**Entrée:**
```
TOPIC #test sujet_sans_colon
```

**Sortie Attendue:**
```
:server 461 nick TOPIC :Wrong parameters
```

**Code Erreur:** 461

---

### TEST 06: TOPIC Canal Inexistant
**Type:** Test Unitaire  
**Objectif:** Vérifier que TOPIC sur un canal inexistant retourne ERR_NOSUCHCHANNEL.

**Entrée:**
```
TOPIC #nonexistent
```

**Sortie Attendue:**
```
:server 403 nick #nonexistent :No such channel
```

**Code Erreur:** 403 (ERR_NOSUCHCHANNEL)

---

### TEST 07: TOPIC Non Membre du Canal
**Type:** Test Unitaire  
**Objectif:** Vérifier que TOPIC échoue si le client n'est pas membre du canal.

**Entrée:**
```
TOPIC #otherchannel
```

**Sortie Attendue:**
```
:server 442 nick #otherchannel :You're not on that channel
```

**Code Erreur:** 442 (ERR_NOTONCHANNEL)

---

### TEST 08: TOPIC Voir - Pas de Sujet
**Type:** Test Unitaire  
**Objectif:** Vérifier RPL_NOTOPIC quand aucun sujet n'est défini.

**Préconditions:**
- Client membre du canal
- Canal sans sujet défini

**Entrée:**
```
TOPIC #test
```

**Sortie Attendue:**
```
:server 331 nick #test :No topic is set
```

**Code Succès:** 331 (RPL_NOTOPIC)

---

### TEST 09: TOPIC Voir - Sujet Existe
**Type:** Test Unitaire  
**Objectif:** Vérifier RPL_TOPIC quand un sujet existe.

**Préconditions:**
- Client membre du canal
- Canal avec sujet "Welcome!"

**Entrée:**
```
TOPIC #test
```

**Sortie Attendue:**
```
:server 332 nick #test :Welcome!
```

**Code Succès:** 332 (RPL_TOPIC)

---

### TEST 10: TOPIC Définir - Membre Normal (Canal sans +t)
**Type:** Test Unitaire  
**Objectif:** Vérifier qu'un membre normal peut définir le sujet sur un canal sans mode +t.

**Préconditions:**
- Client membre mais pas opérateur
- Canal sans mode +t

**Entrée:**
```
TOPIC #general :New Topic
```

**Sortie Attendue:**
```
:nick!user@host TOPIC #general :New Topic
```

**Vérifications:**
- [ ] Sujet changé dans le canal
- [ ] Message diffusé à tous les membres

---

### TEST 11: TOPIC Définir - Opérateur (Canal sans +t)
**Type:** Test Unitaire  
**Objectif:** Vérifier qu'un opérateur peut définir le sujet.

**Résultat Attendu:** Succès - Sujet défini et diffusé.

---

### TEST 12: TOPIC Définir - Canal +t Non-Opérateur
**Type:** Test Unitaire  
**Objectif:** Vérifier qu'un non-opérateur ne peut pas modifier le sujet sur un canal +t.

**Préconditions:**
- Canal avec mode +t actif
- Client membre mais pas opérateur

**Entrée:**
```
TOPIC #protected :New Topic
```

**Sortie Attendue:**
```
:server 482 nick #protected :You're not channel operator
```

**Code Erreur:** 482 (ERR_CHANOPRIVSNEEDED)

---

### TEST 13: TOPIC Définir - Canal +t Opérateur
**Type:** Test Unitaire  
**Objectif:** Vérifier qu'un opérateur peut modifier le sujet sur un canal +t.

**Préconditions:**
- Canal avec mode +t actif
- Client est opérateur du canal

**Entrée:**
```
TOPIC #protected :Ops Topic
```

**Sortie Attendue:**
```
:nick!user@host TOPIC #protected :Ops Topic
```

---

### TEST 14: TOPIC Effacer - Sujet Vide
**Type:** Test Unitaire  
**Objectif:** Vérifier que `TOPIC #canal :` efface le sujet.

**Entrée:**
```
TOPIC #test :
```

**Sortie Attendue:**
```
:nick!user@host TOPIC #test :
```

**Vérifications:**
- [ ] Sujet effacé (chaîne vide)
- [ ] Effacement diffusé à tous les membres

---

### TEST 15: TOPIC Diffusion à Tous les Membres
**Type:** Test Unitaire  
**Objectif:** Vérifier que le changement de sujet est diffusé à tous les membres.

**Préconditions:**
- Canal avec 3 membres: alice, bob, charlie
- alice change le sujet

**Sortie Attendue:**
- alice reçoit: `:alice!alice@host TOPIC #test :New Topic`
- bob reçoit: `:alice!alice@host TOPIC #test :New Topic`
- charlie reçoit: `:alice!alice@host TOPIC #test :New Topic`

---

### TEST 16: TOPIC Préservation Multi-Mots
**Type:** Test Unitaire  
**Objectif:** Vérifier que les sujets avec plusieurs mots sont préservés.

**Entrée:**
```
TOPIC #test :This is a topic with many words
```

**Vérification:**
```
TOPIC #test
```

**Sortie Attendue:**
```
:server 332 nick #test :This is a topic with many words
```

---

### TEST 17: TOPIC Caractères Spéciaux
**Type:** Test Unitaire  
**Objectif:** Vérifier que les caractères spéciaux sont préservés.

| Entrée | Résultat Attendu |
|--------|------------------|
| `TOPIC #test :Hello! @everyone` | Préservé |
| `TOPIC #test :URL: http://test.com` | Préservé |
| `TOPIC #test :!@#$%^&*()` | Préservé |
| `TOPIC #test ::)` | Préservé |

---

### TEST 18: TOPIC Avec Deux-Points Initial
**Type:** Test Unitaire  
**Objectif:** Vérifier que `TOPIC #test ::content` préserve le `:` initial.

**Entrée:**
```
TOPIC #test ::This starts with colon
```

**Sujet Résultant:** `:This starts with colon`

---

### TEST 19: TOPIC Sujet Très Long
**Type:** Test Unitaire  
**Objectif:** Tester le comportement avec des sujets très longs.

**Note:** La limite standard IRC est d'environ 390 caractères (optionnel).

**Vérifications:**
- [ ] Serveur accepte le sujet long
- [ ] OU serveur tronque proprement
- [ ] Pas de crash ou comportement indéfini

---

### TEST 20: TOPIC Vérification Format Message
**Type:** Test Unitaire  
**Objectif:** Vérifier que tous les formats de message sont corrects.

| Type | Format |
|------|--------|
| RPL_NOTOPIC | `:server 331 nick #channel :No topic is set\r\n` |
| RPL_TOPIC | `:server 332 nick #channel :topic text\r\n` |
| Broadcast | `:nick!user@host TOPIC #channel :topic text\r\n` |
| ERR 403 | `:server 403 nick #channel :No such channel\r\n` |
| ERR 442 | `:server 442 nick #channel :You're not on that channel\r\n` |
| ERR 451 | `:server 451 nick :You have not registered\r\n` |
| ERR 461 | `:server 461 nick TOPIC :Not enough parameters\r\n` |
| ERR 482 | `:server 482 nick #channel :You're not channel operator\r\n` |

---

### TEST 21: TOPIC Changements Consécutifs
**Type:** Test Unitaire  
**Objectif:** Vérifier que les changements rapides de sujet fonctionnent.

**Séquence:**
```
TOPIC #test :Topic 1
TOPIC #test :Topic 2
TOPIC #test :Topic 3
TOPIC #test :Topic 4
TOPIC #test :Topic 5
```

**Vérification:** Le sujet final doit être "Topic 5".

---

### TEST 22: TOPIC Fuites Mémoire
**Type:** Valgrind  
**Objectif:** Vérifier l'absence de fuites mémoire.

**Commande:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./test_topic_command 22
```

**Attendu:**
```
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
```

---

### TEST 23: TOPIC Multi-Client Concurrent
**Type:** Test Manuel  
**Objectif:** Vérifier le comportement avec plusieurs clients simultanés.

**Procédure:**
1. Démarrer le serveur: `./ircserv 6667 pass`
2. Terminal 2: Client Alice
3. Terminal 3: Client Bob
4. Terminal 4: Client Charlie
5. Tous rejoignent #test
6. Alice: `TOPIC #test :New Announcement`
7. Vérifier que Bob et Charlie reçoivent le message

---

### TEST 24: TOPIC Test de Stress
**Type:** Test Unitaire  
**Objectif:** Tester la stabilité sous charge.

**Actions:**
- 1000 changements de sujet rapides
- Sujets de longueurs variées (1 à 400 caractères)

---

### TEST 25: TOPIC Intégration avec JOIN
**Type:** Test Unitaire  
**Objectif:** Vérifier que les nouveaux membres voient le sujet.

**Scénario:**
1. Alice crée #welcome et définit le sujet
2. Bob rejoint #welcome
3. Bob doit recevoir RPL_TOPIC (332)

**Sortie Attendue pour Bob:**
```
:server 332 bob #welcome :Welcome to our channel!
```

---

## 🔍 Matrice des Permissions

### Qui Peut Définir le Sujet ?

| Mode du Canal | Type d'Utilisateur | Peut Définir ? |
|---------------|-------------------|----------------|
| Normal (-t) | Opérateur | ✅ Oui |
| Normal (-t) | Membre | ✅ Oui |
| Protégé (+t) | Opérateur | ✅ Oui |
| Protégé (+t) | Membre | ❌ Non (482) |

### Qui Peut Voir le Sujet ?

| Statut | Peut Voir ? |
|--------|-------------|
| Membre du canal | ✅ Oui |
| Non-membre | ❌ Non (442) |

---

## 🧪 Tests Manuels avec netcat

### Setup Initial
```bash
# Terminal 1 - Serveur
./ircserv 6667 testpass

# Terminal 2 - Client
nc -C localhost 6667
```

### Test Complet
```
PASS testpass
NICK alice
USER alice 0 * :Alice
JOIN #test
TOPIC #test
TOPIC #test :Welcome to the Test Channel!
TOPIC #test
TOPIC #test :
TOPIC #test
```

### Résultats Attendus
```
# Après TOPIC #test (initial)
:server 331 alice #test :No topic is set

# Après TOPIC #test :Welcome...
:alice!alice@localhost TOPIC #test :Welcome to the Test Channel!

# Après TOPIC #test (view)
:server 332 alice #test :Welcome to the Test Channel!

# Après TOPIC #test : (clear)
:alice!alice@localhost TOPIC #test :

# Après TOPIC #test (after clear)
:server 331 alice #test :No topic is set
```

---

## ⚠️ Problèmes Connus et Solutions

### Problème 1: Sujet Non Diffusé
**Symptôme:** Le sujet change mais les autres membres ne voient pas la mise à jour.
**Solution:** Vérifier que `channel->broadcast()` est appelé après `setTopic()`.

### Problème 2: Sujet Multi-Mots Tronqué
**Symptôme:** "Hello World" devient "Hello".
**Solution:** Joindre tous les params à partir de l'index 1 ou utiliser le parsing du `:`.

### Problème 3: Opérateur Rejeté sur +t
**Symptôme:** Même l'opérateur reçoit l'erreur 482.
**Solution:** Vérifier la logique `isTopicProtected() && !isOperator(client)`.

### Problème 4: Erreur d'Include
**Symptôme:** `#include "../includes/Channelhpp"` (manque le `.`).
**Solution:** Corriger en `#include "../includes/Channel.hpp"`.

---

## 📁 Fichiers Associés

| Fichier | Description |
|---------|-------------|
| `includes/TopicCommand.hpp` | Déclaration de la classe |
| `src/TopicCommand.cpp` | Implémentation |
| `includes/Channel.hpp` | Méthodes getTopic(), setTopic() |
| `src/Channel.cpp` | Implémentation du canal |
| `tests/test_topic_command.cpp` | Suite de tests |
| `tests/TOPIC_COMMAND_TESTS.md` | Cette documentation |

---

## 📋 Checklist Finale

- [ ] TEST 01: Construction sans crash
- [ ] TEST 02: Destruction sans fuite
- [ ] TEST 03: 451 si non enregistré
- [ ] TEST 04: 461 si pas de paramètre
- [ ] TEST 05: 461 si mauvais format
- [ ] TEST 06: 403 si canal inexistant
- [ ] TEST 07: 442 si non membre
- [ ] TEST 08: 331 si pas de sujet
- [ ] TEST 09: 332 avec le sujet
- [ ] TEST 10: Membre peut définir sur -t
- [ ] TEST 11: Opérateur peut définir
- [ ] TEST 12: 482 pour non-op sur +t
- [ ] TEST 13: Op peut définir sur +t
- [ ] TEST 14: Effacement fonctionne
- [ ] TEST 15: Broadcast à tous
- [ ] TEST 16: Multi-mots préservés
- [ ] TEST 17: Chars spéciaux OK
- [ ] TEST 18: Colon initial préservé
- [ ] TEST 19: Long sujet géré
- [ ] TEST 20: Formats corrects
- [ ] TEST 21: Changements rapides OK
- [ ] TEST 22: Pas de fuites mémoire
- [ ] TEST 23: Multi-client OK
- [ ] TEST 24: Stress test OK
- [ ] TEST 25: JOIN affiche le sujet

---

## 🏁 Conclusion

Cette suite de tests couvre tous les aspects de la commande TOPIC selon le protocole IRC. Exécutez régulièrement ces tests pendant le développement pour garantir la conformité et la stabilité de l'implémentation.

**Contact:** En cas de questions, consultez la documentation IRC RFC 2812 ou contactez l'équipe.
