# Command Class Test Suite Documentation

## Vue d'ensemble

Ce document décrit la suite de tests isolés pour la classe `Command` du projet ft_irc.  
Les tests sont **autonomes** et ne nécessitent pas de serveur en cours d'exécution.

---

## Compilation

```bash
cd tests/
g++ -Wall -Wextra -Werror -std=c++98 -I../includes \
    test_command.cpp \
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
    ../src/ModeCommand.cpp \
    ../src/KickCommand.cpp \
    ../src/QuitCommand.cpp \
    ../src/PingCommand.cpp \
    ../src/PongCommand.cpp \
    ../src/NoticeCommand.cpp \
    -o test_command
```

## Utilisation

```bash
# Exécuter tous les tests
./test_command

# Exécuter un test spécifique (ex: test 5)
./test_command 5
```

---

## Cas de Test

### Tests du Constructeur

| # | Nom du Test | Description | Comportement Attendu |
|---|-------------|-------------|----------------------|
| 01 | Constructor Stores Server | Vérifier stockage du pointeur server | Pointeur server stocké correctement |
| 02 | Constructor Stores Client | Vérifier stockage du pointeur client | Pointeur client stocké correctement |
| 03 | Constructor Stores Params | Vérifier stockage du vecteur params | Vecteur params copié correctement |
| 04 | Empty Params | Constructeur avec params vide | Vecteur vide accepté |
| 05 | NULL Server | Constructeur avec server NULL | Exception `std::invalid_argument` lancée |
| 06 | NULL Client | Constructeur avec client NULL | Exception `std::invalid_argument` lancée |

### Tests de Polymorphisme

| # | Nom du Test | Description | Comportement Attendu |
|---|-------------|-------------|----------------------|
| 07 | Execute Polymorphism | Appel execute() via pointeur base | Méthode dérivée appelée correctement |
| 08 | Virtual Destructor | Suppression via pointeur base | Pas de fuite mémoire |

### Tests de Robustesse

| # | Nom du Test | Description | Comportement Attendu |
|---|-------------|-------------|----------------------|
| 09 | Special Characters | Params avec caractères spéciaux | Caractères préservés |
| 10 | Many Params | 100 paramètres | Tous stockés correctement |
| 11 | formatCode | Formatage des codes numériques | Padding à 3 chiffres (001, 042, 451) |
| 12 | getClientNick Registered | Nickname client enregistré | Retourne le nickname |
| 13 | getClientNick Unregistered | Client non enregistré | Retourne "*" |
| 14 | Multiple Commands | Plusieurs commandes même client | Isolation correcte |
| 15 | Extended ASCII | Caractères ASCII étendus | Gestion correcte |

---

## Classe TestCommand

Pour tester la classe abstraite `Command`, nous utilisons une classe concrète de test :

```cpp
class TestCommand : public Command {
public:
    bool executed;
    
    TestCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params), executed(false) {}
    
    void execute() {
        executed = true;
    }
    
    // Expose protected members for testing
    Server* getServer() { return server; }
    Client* getClient() { return client; }
    std::vector<std::string> getParams() { return params; }
};
```

Cette classe nous permet de :
- Accéder aux membres protégés (`server`, `client`, `params`)
- Vérifier que `execute()` est appelé correctement
- Tester le polymorphisme

---

## Architecture de la Classe Command

### Diagramme UML

```
┌─────────────────────────────────────────┐
│              Command                     │
├─────────────────────────────────────────┤
│ # server: Server*                        │
│ # client: Client*                        │
│ # params: std::vector<std::string>       │
├─────────────────────────────────────────┤
│ + Command(srv, cli, params)              │
│ + virtual ~Command()                     │
│ + virtual execute() = 0                  │
│ # formatCode(int): std::string           │
│ # getClientNick(): std::string           │
└─────────────────────────────────────────┘
              △
              │ hérite
    ┌─────────┼─────────┬─────────┐
    │         │         │         │
┌───┴───┐ ┌───┴───┐ ┌───┴───┐ ┌───┴───┐
│ Pass  │ │ Nick  │ │ Join  │ │ Mode  │
│Command│ │Command│ │Command│ │Command│
└───────┘ └───────┘ └───────┘ └───────┘
```

### Membres Protégés

| Membre | Type | Description |
|--------|------|-------------|
| `server` | `Server*` | Pointeur vers l'instance du serveur |
| `client` | `Client*` | Pointeur vers le client exécutant la commande |
| `params` | `std::vector<std::string>` | Paramètres de la commande |

### Méthodes Protégées

| Méthode | Retour | Description |
|---------|--------|-------------|
| `formatCode(int)` | `std::string` | Formate un code numérique IRC (ex: 1 → "001") |
| `getClientNick()` | `std::string` | Retourne le nickname ou "*" si non enregistré |

---

## Patron de Conception Command

### Avantages

1. **Encapsulation** : Chaque commande encapsule sa propre logique
2. **Polymorphisme** : Le serveur appelle `execute()` sans connaître le type
3. **Extensibilité** : Facile d'ajouter de nouvelles commandes
4. **Testabilité** : Commandes testables indépendamment

### Flux d'Utilisation

```
1. Client envoie: "JOIN #channel"
2. MessageParser crée: JoinCommand(server, client, params)
3. Serveur appelle: command->execute()
4. JoinCommand exécute la logique spécifique
5. Commande supprimée après exécution
```

---

## Vérifications de Sécurité

### Validation du Constructeur

```cpp
Command::Command(Server* srv, Client* cli, const std::vector<std::string>& params)
    : server(srv), client(cli), params(params) {
    if (srv == NULL || cli == NULL) {
        throw std::invalid_argument("Server or Client pointer is null");
    }
}
```

### Pourquoi c'est important

- Évite les segfaults dans les méthodes
- Détection précoce des erreurs de programmation
- Facilite le débogage

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
Quand `sendMessage()` essaie d'envoyer via `send()`, cela échoue.  
**C'est un comportement attendu** - les tests vérifient la logique, pas l'I/O réseau.

### Extension de la Classe Command

Pour ajouter une nouvelle commande :

```cpp
class NewCommand : public Command {
public:
    NewCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params) {}
    
    void execute() {
        // 1. Vérifier l'enregistrement
        // 2. Valider les paramètres
        // 3. Exécuter la logique
        // 4. Envoyer les réponses
    }
};
```

---

## Fichiers Associés

| Fichier | Description |
|---------|-------------|
| `includes/Command.hpp` | Déclaration de la classe |
| `src/Command.cpp` | Implémentation |
| `tests/test_command.cpp` | Suite de tests |
| `tasks/YASSER/Command.md` | Documentation de la tâche |

---

## Checklist des Tests

- [x] Constructeur stocke server correctement
- [x] Constructeur stocke client correctement
- [x] Constructeur stocke params correctement
- [x] Params vide géré
- [x] NULL server lance exception
- [x] NULL client lance exception
- [x] Polymorphisme fonctionne
- [x] Destructeur virtuel
- [x] Caractères spéciaux préservés
- [x] Grand nombre de params géré
- [x] formatCode fonctionne
- [x] getClientNick enregistré
- [x] getClientNick non enregistré
- [x] Commandes multiples isolées
- [x] ASCII étendu géré
