# Documentation des Tests - NickCommand

## Vue d'ensemble

Ce document décrit la stratégie de test et les cas de test pour la classe `NickCommand` du projet ft_irc.

---

## Table des matières

1. [Introduction](#introduction)
2. [Fichiers de test](#fichiers-de-test)
3. [Prérequis](#prérequis)
4. [Compilation et exécution](#compilation-et-exécution)
5. [Couverture des tests](#couverture-des-tests)
6. [Description détaillée des tests](#description-détaillée-des-tests)
7. [Codes d'erreur IRC testés](#codes-derreur-irc-testés)
8. [Limitations connues](#limitations-connues)
9. [Ajout de nouveaux tests](#ajout-de-nouveaux-tests)

---

## Introduction

La commande `NICK` est une commande fondamentale du protocole IRC qui permet de :
- Définir le pseudonyme initial d'un client
- Changer le pseudonyme d'un client déjà enregistré

### Syntaxe IRC
```
NICK <pseudonyme>
```

### Référence RFC 1459
La validation du nickname suit les règles RFC 1459 :
- Doit commencer par une lettre (a-z, A-Z)
- Peut contenir : lettres, chiffres, caractères spéciaux (`-[]\\`^{}`)
- Longueur maximale : 9 caractères

---

## Fichiers de test

| Fichier | Description |
|---------|-------------|
| `tests/test_nickcommand.cpp` | Tests unitaires C++ |
| `tests/test_nickcommand.md` | Cette documentation |

---

## Prérequis

- Compilateur C++ avec support C++98
- Système POSIX (Linux/MacOS) pour `socketpair()`
- Projet ft_irc compilable

---

## Compilation et exécution

### Compilation manuelle

```bash
cd /home/yandriam/ft_irc

# Compiler les tests NickCommand
c++ -Wall -Wextra -Werror -std=c++98 \
    tests/test_nickcommand.cpp \
    src/NickCommand.cpp \
    src/Command.cpp \
    src/Client.cpp \
    src/Server.cpp \
    src/Channel.cpp \
    -o test_nickcommand

# Exécuter les tests
./test_nickcommand
```

### Avec Makefile (suggestion)

Ajouter au Makefile :

```makefile
test_nick:
	@$(CXX) $(CXXFLAGS) tests/test_nickcommand.cpp \
		src/NickCommand.cpp src/Command.cpp src/Client.cpp \
		src/Server.cpp src/Channel.cpp -o test_nickcommand
	@./test_nickcommand
```

---

## Couverture des tests

### Résumé

| Catégorie | Tests | Couverture |
|-----------|-------|------------|
| Constructeur | 3 | ✅ 100% |
| Authentification | 2 | ✅ 100% |
| Validation paramètres | 2 | ✅ 100% |
| Validation format | 8 | ✅ 100% |
| Enregistrement | 2 | ✅ 100% |
| Changement de nick | 3 | ✅ 100% |
| Collision | 1 | ⚠️ 50% (dépend de getClientByNick) |
| Format messages | 1 | ✅ 100% |
| **TOTAL** | **22** | **~95%** |

### Matrice de couverture

| Fonctionnalité | Test associé | État |
|----------------|--------------|------|
| Constructeur valide | `test_constructor_valid()` | ✅ |
| Server NULL | `test_constructor_null_server()` | ✅ |
| Client NULL | `test_constructor_null_client()` | ✅ |
| NICK avant PASS | `test_nick_before_pass()` | ✅ |
| NICK après PASS | `test_nick_after_pass()` | ✅ |
| Paramètre manquant | `test_nick_no_parameter()` | ✅ |
| Paramètre vide | `test_nick_empty_parameter()` | ✅ |
| Nick simple valide | `test_nick_valid_simple()` | ✅ |
| Nick avec chiffres | `test_nick_valid_with_numbers()` | ✅ |
| Nick caractères spéciaux | `test_nick_valid_special_chars()` | ✅ |
| Nick longueur max | `test_nick_valid_max_length()` | ✅ |
| Nick commence par chiffre | `test_nick_invalid_starts_with_number()` | ✅ |
| Nick commence par tiret | `test_nick_invalid_starts_with_dash()` | ✅ |
| Nick trop long | `test_nick_invalid_too_long()` | ✅ |
| Nick chars interdits | `test_nick_invalid_special_chars()` | ✅ |
| Complétion enregistrement | `test_nick_completes_registration()` | ✅ |
| NICK seul (sans USER) | `test_nick_only_not_registered()` | ✅ |
| Changement de nick | `test_nick_change_when_registered()` | ✅ |
| Format message NICK | `test_nick_change_format()` | ✅ |
| NICK identique | `test_nick_change_same_nick()` | ✅ |
| Nick déjà utilisé | `test_nick_already_in_use()` | ⚠️ |
| CRLF en fin de message | `test_nick_response_ends_with_crlf()` | ✅ |

---

## Description détaillée des tests

### 1. Tests du Constructeur

#### `test_constructor_valid()`
**Objectif** : Vérifier que le constructeur initialise correctement l'objet.

**Entrée** :
- Server valide
- Client valide
- params = ["TestNick"]

**Sortie attendue** :
- Pas d'exception
- Objet créé correctement

---

#### `test_constructor_null_server()`
**Objectif** : Vérifier que le constructeur lève une exception si Server est NULL.

**Entrée** :
- Server = NULL
- Client valide

**Sortie attendue** :
- `std::invalid_argument` levée

---

#### `test_constructor_null_client()`
**Objectif** : Vérifier que le constructeur lève une exception si Client est NULL.

**Entrée** :
- Server valide
- Client = NULL

**Sortie attendue** :
- `std::invalid_argument` levée

---

### 2. Tests d'Authentification

#### `test_nick_before_pass()`
**Objectif** : Vérifier que NICK est rejeté si le client n'a pas envoyé PASS.

**Entrée** :
- Client non authentifié
- NICK TestNick

**Sortie attendue** :
- ERR 464 (Password mismatch)
- Nickname non défini

---

#### `test_nick_after_pass()`
**Objectif** : Vérifier que NICK est accepté après PASS.

**Entrée** :
- Client authentifié
- NICK TestNick

**Sortie attendue** :
- Pas d'erreur 464
- Nickname = "TestNick"

---

### 3. Tests de Validation des Paramètres

#### `test_nick_no_parameter()`
**Objectif** : Vérifier le comportement sans paramètre.

**Entrée** :
- params = []

**Sortie attendue** :
- ERR 431 (No nickname given)

---

#### `test_nick_empty_parameter()`
**Objectif** : Vérifier le comportement avec paramètre vide.

**Entrée** :
- params = [""]

**Sortie attendue** :
- ERR 431 ou ERR 432

---

### 4. Tests de Validation du Format (RFC 1459)

#### Nicknames valides testés

| Test | Nickname | Règle RFC 1459 |
|------|----------|----------------|
| `test_nick_valid_simple()` | "Alice" | Lettres uniquement |
| `test_nick_valid_with_numbers()` | "Bob123" | Lettres + chiffres |
| `test_nick_valid_special_chars()` | "a-[]" | Caractères spéciaux |
| `test_nick_valid_max_length()` | "Abcdefghi" | 9 caractères max |

#### Nicknames invalides testés

| Test | Nickname | Erreur attendue |
|------|----------|-----------------|
| `test_nick_invalid_starts_with_number()` | "123abc" | ERR 432 |
| `test_nick_invalid_starts_with_dash()` | "-test" | ERR 432 |
| `test_nick_invalid_too_long()` | "Abcdefghij" | ERR 432 |
| `test_nick_invalid_special_chars()` | "test@user" | ERR 432 |

---

### 5. Tests d'Enregistrement

#### `test_nick_completes_registration()`
**Objectif** : Vérifier que NICK + USER complète l'enregistrement.

**Entrée** :
- Client avec username déjà défini
- NICK TestNick

**Sortie attendue** :
- Messages 001, 002, 003, 004 envoyés
- Client.isRegistered() = true

**Messages attendus** :
```
:irc.example.com 001 TestNick :Welcome to the IRC Network...
:irc.example.com 002 TestNick :Your host is...
:irc.example.com 003 TestNick :This server was created...
:irc.example.com 004 TestNick irc.example.com 1.0 o o
```

---

#### `test_nick_only_not_registered()`
**Objectif** : Vérifier que NICK seul ne complète pas l'enregistrement.

**Entrée** :
- Client sans username
- NICK TestNick

**Sortie attendue** :
- Pas de messages 001-004
- Client.isRegistered() = false
- Nickname défini

---

### 6. Tests de Changement de Nickname

#### `test_nick_change_when_registered()`
**Objectif** : Vérifier le changement de nick pour un client enregistré.

**Entrée** :
- Client enregistré avec nick "OldNick"
- NICK NewNick

**Sortie attendue** :
- Message NICK diffusé
- Nickname = "NewNick"

---

#### `test_nick_change_format()`
**Objectif** : Vérifier le format du message de changement.

**Entrée** :
- Client "John" avec user "john" et host "localhost"
- NICK Johnny

**Sortie attendue** :
```
:John!john@localhost NICK :Johnny
```

---

#### `test_nick_change_same_nick()`
**Objectif** : Vérifier le comportement si le nouveau nick est identique.

**Entrée** :
- Client avec nick "TestNick"
- NICK TestNick

**Sortie attendue** :
- Pas d'erreur 433
- Opération silencieuse ou message de confirmation

---

### 7. Tests de Collision

#### `test_nick_already_in_use()`
**Objectif** : Vérifier le rejet d'un nickname déjà utilisé.

**Note** : Ce test est limité car `Server::getClientByNick()` n'est pas implémenté.

**Sortie attendue (quand implémenté)** :
- ERR 433 (Nickname is already in use)

---

### 8. Tests de Format des Messages

#### `test_nick_response_ends_with_crlf()`
**Objectif** : Vérifier que toutes les réponses se terminent par `\r\n`.

**Sortie attendue** :
- Derniers caractères = `\r\n`

---

## Codes d'erreur IRC testés

| Code | Nom | Message | Tests |
|------|-----|---------|-------|
| 431 | ERR_NONICKNAMEGIVEN | ":No nickname given" | 2 |
| 432 | ERR_ERRONEUSNICKNAME | "<nick> :Erroneous nickname" | 4 |
| 433 | ERR_NICKNAMEINUSE | "<nick> :Nickname is already in use" | 1 |
| 464 | ERR_PASSWDMISMATCH | ":You must send PASS first" | 1 |

### Codes de succès testés

| Code | Nom | Tests |
|------|-----|-------|
| 001 | RPL_WELCOME | 1 |
| 002 | RPL_YOURHOST | 1 |
| 003 | RPL_CREATED | 1 |
| 004 | RPL_MYINFO | 1 |

---

## Limitations connues

### 1. Dépendance à `Server::getClientByNick()`

La méthode `Server::getClientByNick()` retourne actuellement toujours `NULL`, ce qui signifie que :
- La détection de collision de nickname ne fonctionne pas
- Le test `test_nick_already_in_use()` ne peut pas valider complètement le comportement

**Solution** : Implémenter `Server::getClientByNick()` pour compléter les tests.

### 2. Comparaison case-insensitive

Les tests ne vérifient pas actuellement la comparaison insensible à la casse car cette fonctionnalité n'est pas implémentée.

**Tests à ajouter** :
```cpp
void test_nick_case_insensitive() {
    // "John" et "john" devraient être considérés comme identiques
}
```

### 3. Diffusion aux canaux

Les tests ne vérifient pas la diffusion du changement de nick aux autres membres des canaux car cela nécessiterait :
- Des clients supplémentaires
- Des canaux configurés
- Un serveur fonctionnel

---

## Ajout de nouveaux tests

### Template de test

```cpp
void test_nick_new_feature() {
    TEST("Description du test");
    
    int readFd, writeFd;
    if (createTestSocketPair(readFd, writeFd) != 0) {
        FAIL("Impossible de créer la paire de sockets");
        return;
    }
    
    Server server(6667, "password");
    Client client(writeFd);
    client.authenticate();
    
    std::vector<std::string> params;
    params.push_back("TestValue");
    
    NickCommand cmd(&server, &client, params);
    cmd.execute();
    
    std::string response = readFromSocket(readFd);
    
    // Assertions
    if (/* condition de succès */) {
        PASS();
    } else {
        FAIL("Message d'erreur détaillé");
    }
    
    close(readFd);
    close(writeFd);
}
```

### Fonctions utilitaires disponibles

| Fonction | Description |
|----------|-------------|
| `createTestSocketPair(readFd, writeFd)` | Crée une paire de sockets pour les tests |
| `readFromSocket(fd, maxLen)` | Lit les données du socket |
| `containsCode(response, code)` | Vérifie si la réponse contient un code numérique |
| `containsText(response, text)` | Vérifie si la réponse contient un texte |

---

## Résumé d'exécution

### Sortie attendue (succès)

```
╔═══════════════════════════════════════════════════════════╗
║           TESTS UNITAIRES - NickCommand                   ║
║                   ft_irc - 42                             ║
╚═══════════════════════════════════════════════════════════╝

=== CONSTRUCTEUR ===

Testing Constructeur avec paramètres valides... PASS
Testing Constructeur avec Server NULL... PASS
Testing Constructeur avec Client NULL... PASS

=== AUTHENTIFICATION (PASS requis) ===

Testing NICK avant PASS envoie ERR 464... PASS
Testing NICK après PASS accepté... PASS

[... autres tests ...]

═══════════════════════════════════════════════════════════
  Résultats: 22 PASSED, 0 FAILED
═══════════════════════════════════════════════════════════
```

---

## Contact

**Auteur** : YASSER  
**Projet** : ft_irc - 42  
**Date** : Janvier 2026

---

## Historique des modifications

| Date | Version | Description |
|------|---------|-------------|
| 2026-01-21 | 1.0 | Création initiale |
