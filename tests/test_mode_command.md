# MODE Command Test Suite Documentation

## Vue d'ensemble

Ce document décrit la suite de tests isolés pour la classe `ModeCommand` du projet ft_irc.  
Les tests sont **autonomes** et ne nécessitent pas de serveur en cours d'exécution.

---

## Compilation

```bash
cd tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_mode_command.cpp \
    ../src/ModeCommand.cpp \
    ../src/Command.cpp \
    ../src/Channel.cpp \
    ../src/Client.cpp \
    ../src/Server.cpp \
    ../src/MessageParser.cpp \
    ../src/JoinCommand.cpp \
    ../src/PassCommand.cpp \
    ../src/NickCommand.cpp \
    ../src/UserCommand.cpp \
    ../src/PartCommand.cpp \
    ../src/PrivmsgCommand.cpp \
    ../src/InviteCommand.cpp \
    ../src/TopicCommand.cpp \
    ../src/KickCommand.cpp \
    ../src/QuitCommand.cpp \
    ../src/PingCommand.cpp \
    ../src/PongCommand.cpp \
    ../src/NoticeCommand.cpp \
    -o test_mode_command
```

## Utilisation

```bash
# Exécuter tous les tests
./test_mode_command

# Exécuter un test spécifique (ex: test 5)
./test_mode_command 5
```

---

## Cas de Test

### Tests des Modes Simples (sans paramètres)

| # | Nom du Test | Description | Comportement Attendu | Code Erreur |
|---|-------------|-------------|----------------------|-------------|
| 01 | MODE +i | `MODE #channel +i` | Canal devient invite-only | - |
| 02 | MODE -i | `MODE #channel -i` | Canal n'est plus invite-only | - |
| 03 | MODE +t | `MODE #channel +t` | Restriction topic activée | - |
| 04 | MODE -t | `MODE #channel -t` | Restriction topic désactivée | - |

### Tests des Modes avec Paramètres

| # | Nom du Test | Description | Comportement Attendu | Code Erreur |
|---|-------------|-------------|----------------------|-------------|
| 05 | MODE +k | `MODE #channel +k password` | Clé du canal définie | - |
| 06 | MODE -k | `MODE #channel -k` | Clé du canal supprimée | - |
| 07 | MODE +l | `MODE #channel +l 50` | Limite d'utilisateurs à 50 | - |
| 08 | MODE -l | `MODE #channel -l` | Limite supprimée | - |
| 09 | MODE +o | `MODE #channel +o nick` | Utilisateur devient opérateur | - |
| 10 | MODE -o | `MODE #channel -o nick` | Utilisateur n'est plus opérateur | - |

### Tests de Permissions

| # | Nom du Test | Description | Comportement Attendu | Code Erreur |
|---|-------------|-------------|----------------------|-------------|
| 11 | Non-Operator | Non-op essaie de changer mode | Erreur envoyée | 482 (ERR_CHANOPRIVSNEEDED) |
| 12 | No Channel | `MODE #inexistant +i` | Erreur envoyée | 403 (ERR_NOSUCHCHANNEL) |
| 13 | Not on Channel | Non-membre essaie MODE | Erreur envoyée | 442 (ERR_NOTONCHANNEL) |
| 14 | Not Registered | Client non enregistré | Erreur envoyée | 451 (ERR_NOTREGISTERED) |
| 15 | Unknown Mode | `MODE #channel +x` | Erreur envoyée | 472 (ERR_UNKNOWNMODE) |

### Tests des Modes Multiples

| # | Nom du Test | Description | Comportement Attendu |
|---|-------------|-------------|----------------------|
| 16 | MODE +it | Deux modes simples | Les deux modes appliqués |
| 17 | MODE +kl | Modes avec params | Clé et limite définis |
| 18 | MODE -ik | Suppression multiple | Modes retirés |
| 19 | MODE +l 0 | Limite invalide | Ignoré |
| 20 | MODE +l abc | Limite non-numérique | Ignoré |
| 21 | MODE +k (no param) | +k sans clé | Ignoré |
| 22 | MODE +o-o | Toggle opérateur | Appliqué correctement |

### Tests Avancés

| # | Nom du Test | Description | Comportement Attendu |
|---|-------------|-------------|----------------------|
| 23 | MODE View | `MODE #channel` seul | Affiche les modes actuels |
| 24 | Stress Test | 50 changements consécutifs | Pas de crash |
| 25 | NULL Safety | +o sur user inexistant | Pas de crash |

---

## Référence des Codes d'Erreur

| Code | Nom | Description |
|------|-----|-------------|
| 403 | ERR_NOSUCHCHANNEL | Le canal n'existe pas |
| 442 | ERR_NOTONCHANNEL | L'utilisateur n'est pas dans le canal |
| 451 | ERR_NOTREGISTERED | Client non enregistré (PASS/NICK/USER) |
| 461 | ERR_NEEDMOREPARAMS | Paramètres manquants |
| 472 | ERR_UNKNOWNMODE | Caractère de mode inconnu |
| 482 | ERR_CHANOPRIVSNEEDED | L'utilisateur n'est pas opérateur |

---

## Syntaxe de la Commande MODE

```
MODE <channel> [+/-<modes>] [paramètres...]
```

### Paramètres
- `<channel>` - Le canal concerné (ex: `#general`)
- `<modes>` - Chaîne de modes (ex: `+it`, `-k`, `+kl`)
- `paramètres` - Paramètres des modes (clé, limite, nickname)

### Modes Supportés

| Mode | Paramètre | Description |
|------|-----------|-------------|
| `i` | Non | Canal sur invitation uniquement |
| `t` | Non | Seuls les ops peuvent changer le topic |
| `k` | Oui (+k) | Clé (mot de passe) du canal |
| `l` | Oui (+l) | Limite d'utilisateurs |
| `o` | Oui | Donner/retirer le statut opérateur |

### Exemples

```
MODE #general                    → Affiche les modes actuels
MODE #general +i                 → Active invite-only
MODE #general -i                 → Désactive invite-only
MODE #general +it                → Active invite-only ET topic restriction
MODE #general +k secretkey       → Définit la clé "secretkey"
MODE #general -k                 → Supprime la clé
MODE #general +l 50              → Limite à 50 utilisateurs
MODE #general -l                 → Supprime la limite
MODE #general +o Bob             → Donne opérateur à Bob
MODE #general -o Bob             → Retire opérateur de Bob
MODE #general +kl password 50    → Clé + limite en une commande
MODE #general +o-o Alice Bob     → +o Alice, -o Bob
```

---

## Architecture du ModeCommand

### Diagramme de Flux

```
┌─────────────────────────────────────┐
│           ModeCommand               │
├─────────────────────────────────────┤
│ execute()                           │
│   ├── checkErrorModes()             │
│   │     ├── Vérifie registration    │
│   │     ├── Vérifie params          │
│   │     ├── Vérifie canal existe    │
│   │     └── Vérifie membre          │
│   ├── showModes() (si pas de mode)  │
│   ├── Vérifie opérateur             │
│   └── processModes()                │
│         ├── handleModeI()           │
│         ├── handleModeT()           │
│         ├── handleModeK()           │
│         ├── handleModeO()           │
│         └── handleModeL()           │
└─────────────────────────────────────┘
```

### Fonctions Helpers

| Fonction | Paramètres | Description |
|----------|------------|-------------|
| `checkErrorModes()` | - | Vérifie toutes les erreurs préliminaires |
| `showModes(channel)` | Channel* | Affiche les modes actuels (324) |
| `handleModeI(channel, adding, modes)` | - | Gère +i / -i |
| `handleModeT(channel, adding, modes)` | - | Gère +t / -t |
| `handleModeK(channel, adding, index, modes, params)` | - | Gère +k / -k |
| `handleModeO(channel, adding, index, modes, params)` | - | Gère +o / -o |
| `handleModeL(channel, adding, index, modes, params)` | - | Gère +l / -l |
| `processModes(channel)` | Channel* | Parse et applique les modes |

---

## Légende des Résultats

| Symbole | Signification |
|---------|---------------|
| `[PASS]` | Test réussi |
| `[FAIL]` | Test échoué - bug détecté |
| `[SKIP]` | Test ignoré - fonctionnalité non implémentée |
| `[INFO]` | Information - comportement variable |

---

## Notes pour les Développeurs

### Erreurs "Bad file descriptor" Attendues

Les tests utilisent des **descripteurs de fichier factices** (10, 11, 12...).  
Quand `broadcast()` ou `sendMessage()` essaie d'envoyer via `send()`, cela échoue.  
**C'est un comportement attendu** - les tests vérifient la logique, pas l'I/O réseau.

### Points Importants d'Implémentation

1. **Ordre des vérifications** :
   - Registration → Params → Channel exists → Is member → Is operator

2. **Modes avec paramètres** :
   - `+k` : Nécessite un paramètre (clé)
   - `+l` : Nécessite un paramètre (limite)
   - `+o` : Nécessite un paramètre (nickname)
   - `-k` : Ne nécessite pas de paramètre
   - `-l` : Ne nécessite pas de paramètre
   - `-o` : Nécessite un paramètre (nickname)

3. **Validation des limites** :
   - Limite ≤ 0 : Ignorée
   - Limite non-numérique : Ignorée

4. **Broadcast** :
   - Les changements de mode sont broadcastés à tous les membres du canal

---

## Format des Messages MODE

### Réponse RPL_CHANNELMODEIS (324)

```
:servername 324 nick #channel +modes [params]
```

Exemple :
```
:irc.server 324 Alice #general +itk secretkey
```

### Broadcast de Changement

```
:nick!user@host MODE #channel +modes [params]
```

Exemple :
```
:Alice!alice@host MODE #general +it
:Alice!alice@host MODE #general +o Bob
:Alice!alice@host MODE #general +kl secretkey 50
```

---

## Fichiers Associés

| Fichier | Description |
|---------|-------------|
| `includes/ModeCommand.hpp` | Déclaration de la classe |
| `src/ModeCommand.cpp` | Implémentation |
| `tests/test_mode_command.cpp` | Suite de tests |
| `tasks/ALL/ModeCommand.md` | Documentation de la tâche |

---

## Checklist des Tests

### Modes Simples
- [x] MODE +i active invite-only
- [x] MODE -i désactive invite-only
- [x] MODE +t active topic restriction
- [x] MODE -t désactive topic restriction

### Modes avec Paramètres
- [x] MODE +k définit la clé
- [x] MODE -k supprime la clé
- [x] MODE +l définit la limite
- [x] MODE -l supprime la limite
- [x] MODE +o donne opérateur
- [x] MODE -o retire opérateur

### Gestion des Erreurs
- [x] Non-opérateur → 482
- [x] Canal inexistant → 403
- [x] Non-membre → 442
- [x] Non enregistré → 451
- [x] Mode inconnu → 472

### Modes Multiples
- [x] +it fonctionne
- [x] +kl avec params fonctionne
- [x] -ik fonctionne
- [x] +o-o toggle fonctionne

### Robustesse
- [x] Limite 0 ignorée
- [x] Limite non-numérique ignorée
- [x] +k sans param ignoré
- [x] +o sur user inexistant → pas de crash
- [x] 50 changements → pas de crash
