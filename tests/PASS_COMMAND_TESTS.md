# 🔐 Tests PassCommand - ft_irc

## Vue d'ensemble

Ce document décrit les tests unitaires pour la commande `PASS` du serveur IRC.  
Les tests vérifient l'authentification par mot de passe selon la RFC 1459.

---

## 📋 Prérequis

```bash
# Lancer le serveur avant les tests
./ircserv 6667 testpass
```

---

## 🚀 Exécution des tests

### Compilation
```bash
cd tests/
c++ -Wall -Wextra -Werror -std=c++98 test_pass_command.cpp -o test_pass_command
```

### Lancement
```bash
# Terminal 1: Lancer le serveur
./ircserv 6667 testpass

# Terminal 2: Lancer les tests
./tests/test_pass_command
```

---

## 📝 Liste des Tests

### Tests Basiques (Obligatoires)

| # | Test | Description | Résultat Attendu |
|---|------|-------------|------------------|
| 01 | `test_pass_correct_password` | PASS avec le bon mot de passe | Succès silencieux (pas de réponse) |
| 02 | `test_pass_wrong_password` | PASS avec le mauvais mot de passe | Erreur 464 (ERR_PASSWDMISMATCH) |
| 03 | `test_pass_no_parameter` | PASS sans paramètre | Erreur 461 (ERR_NEEDMOREPARAMS) |
| 04 | `test_pass_after_registration` | PASS après enregistrement complet | Erreur 462 (ERR_ALREADYREGISTRED) |
| 05 | `test_pass_multiple_attempts` | Plusieurs tentatives avant succès | Autorisé, succès à la dernière |
| 06 | `test_nick_after_successful_pass` | NICK après PASS réussi | NICK accepté |

### Tests Avancés (Complémentaires)

| # | Test | Description | Résultat Attendu |
|---|------|-------------|------------------|
| 07 | `test_nick_before_pass` | NICK avant PASS | Erreur 464 |
| 08 | `test_user_before_pass` | USER avant PASS | Erreur 464 |
| 09 | `test_pass_empty_password` | PASS avec paramètre vide | Erreur 461 ou 464 |
| 10 | `test_pass_case_sensitive` | Sensibilité à la casse | Erreur 464 (casse différente) |
| 11 | `test_pass_with_spaces` | PASS avec espaces | Premier mot utilisé |
| 12 | `test_pass_special_characters` | Caractères spéciaux | Erreur 464 (mauvais mdp) |
| 13 | `test_pass_double_success` | Double PASS réussi | Accepté avant enregistrement |
| 14 | `test_pass_after_nick_before_user` | PASS après NICK, avant USER | Comportement variable |
| 15 | `test_pass_lowercase_command` | Commande `pass` minuscule | Commande acceptée |
| 16 | `test_pass_mixed_case_command` | Commande `PaSs` mixte | Commande acceptée |
| 17 | `test_pass_very_long_password` | Mot de passe 512 chars | Erreur 464 |
| 18 | `test_full_registration_sequence` | Séquence PASS→NICK→USER | Message 001 de bienvenue |
| 19 | `test_multiple_connections` | Connexions simultanées | Gestion indépendante |
| 20 | `test_pass_with_prefix` | PASS avec préfixe IRC | Comportement variable |

NB : -test 03 always fail because the no param is arleady handled in Command parser and then return directly.
        -test 04 fail aswell because this version of the project does not contain the implementation of NICK yet so the bool variable is not changed.
        -test 09 fail because of the same reason for test 03

---

## 📊 Codes d'erreur IRC

| Code | Nom | Description |
|------|-----|-------------|
| 461 | ERR_NEEDMOREPARAMS | Paramètres insuffisants |
| 462 | ERR_ALREADYREGISTRED | Déjà enregistré |
| 464 | ERR_PASSWDMISMATCH | Mot de passe incorrect |

---

## 🔄 Séquence d'enregistrement IRC

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   Initial   │────▶│    PASS     │────▶│    NICK     │────▶│    USER     │
│             │     │ Authentifié │     │  A un nick  │     │ Enregistré  │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

---

## 🧪 Tests manuels avec netcat

### Test 1: Mot de passe correct
```bash
nc -C localhost 6667
PASS testpass
# Attendu: pas de réponse (succès silencieux)
```

### Test 2: Mot de passe incorrect
```bash
nc -C localhost 6667
PASS wrongpass
# Attendu: :server 464 * :Password incorrect
```

### Test 3: Sans paramètre
```bash
nc -C localhost 6667
PASS
# Attendu: :server 461 * PASS :Not enough parameters
```

### Test 4: Après enregistrement
```bash
nc -C localhost 6667
PASS testpass
NICK testuser
USER testuser 0 * :Test User
PASS testpass
# Attendu: :server 462 testuser :You may not reregister
```

### Test 5: Séquence complète
```bash
nc -C localhost 6667
PASS testpass
NICK testuser
USER testuser 0 * :Real Name
# Attendu: :server 001 testuser :Welcome to the IRC Network testuser
```

---

## 🐛 Problèmes courants

### Pas de réponse du serveur
- Vérifier que le serveur est lancé sur le bon port
- Vérifier que `executeCommand()` est implémenté
- Vérifier les includes de MessageParser

### Erreur 464 alors que le mot de passe est correct
- Vérifier la sensibilité à la casse
- Vérifier les espaces en fin de ligne
- Vérifier que `Server::getPassword()` retourne le bon mot de passe

### NICK/USER refusés après PASS
- Vérifier que `client->authenticate()` est appelé
- Vérifier que `client->isAuthenticated()` retourne true

---

## 📁 Fichiers associés

| Fichier | Rôle |
|---------|------|
| `includes/PassCommand.hpp` | Déclaration de la classe |
| `src/PassCommand.cpp` | Implémentation |
| `includes/Command.hpp` | Classe de base |
| `includes/Client.hpp` | État authenticated |
| `includes/Server.hpp` | getPassword() |
| `tests/test_pass_command.cpp` | Tests automatisés |

---

## ✅ Checklist avant validation

- [ ] Compilation sans warnings (`-Wall -Wextra -Werror`)
- [ ] Test 01: PASS correct → succès silencieux
- [ ] Test 02: PASS incorrect → erreur 464
- [ ] Test 03: PASS sans param → erreur 461
- [ ] Test 04: PASS après registration → erreur 462
- [ ] Test 05: Tentatives multiples → autorisées
- [ ] Test 06: NICK après PASS → accepté
- [ ] Test 18: Séquence complète → message 001

---

## 👥 Contributeurs

| Développeur | Tâche |
|-------------|-------|
| NERO | PassCommand |
| | |

---

## 📅 Dernière mise à jour

**Date**: 21 janvier 2026  
**Version**: 1.0
