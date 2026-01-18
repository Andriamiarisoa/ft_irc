# Design Pattern : Factory Method

## 📚 Source
Basé sur [Refactoring.Guru - Factory Method](https://refactoring.guru/design-patterns/factory-method)

---

## 🎯 Objectif

Le **Factory Method** est un pattern créationnel qui fournit une interface pour créer des objets dans une classe parent, mais permet aux sous-classes de modifier le type d'objets créés.

---

## 🤔 Problème

Vous avez du code qui doit créer des objets, mais vous ne savez pas à l'avance quel type exact d'objet sera nécessaire.

**Dans notre projet IRC** : Le `MessageParser` doit créer différents types de commandes (PassCommand, NickCommand, etc.) selon le message reçu.

Sans Factory :
```cpp
Command* cmd;
if (message == "PASS") {
    cmd = new PassCommand(srv, cli, params);
} else if (message == "NICK") {
    cmd = new NickCommand(srv, cli, params);
} else if (message == "USER") {
    cmd = new UserCommand(srv, cli, params);
}
// ... 10+ else if ...
```

Problèmes :
- Code répétitif et long
- Difficile à maintenir
- Viole le principe Open/Closed

---

## ✅ Solution

Créer une **méthode factory** qui encapsule la logique de création :

```cpp
class CommandFactory {
public:
    static Command* createCommand(
        const std::string& cmdName,
        Server* srv,
        Client* cli,
        const std::vector<std::string>& params
    );
};
```

---

## 📝 Structure

```
┌──────────────────────────┐
│      Creator             │
├──────────────────────────┤
│ + factoryMethod()        │◄────── Méthode factory
│ + someOperation()        │
└──────────────────────────┘
         △
         │
┌────────┴─────────┐
│                  │
┌─────────────────────┐  ┌─────────────────────┐
│ ConcreteCreatorA    │  │ ConcreteCreatorB    │
├─────────────────────┤  ├─────────────────────┤
│ + factoryMethod()   │  │ + factoryMethod()   │
└─────────────────────┘  └─────────────────────┘
         │                        │
         ▼                        ▼
┌─────────────────┐      ┌─────────────────┐
│  ProductA       │      │  ProductB       │
└─────────────────┘      └─────────────────┘
```

---

## 💻 Implémentation C++

### Version Simple (Factory Statique)

```cpp
// Command.hpp - Interface produit
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

// CommandFactory.hpp
class CommandFactory {
public:
    static Command* createCommand(
        const std::string& cmdName,
        Server* srv,
        Client* cli,
        const std::vector<std::string>& params
    ) {
        if (cmdName == "PASS") {
            return new PassCommand(srv, cli, params);
        }
        else if (cmdName == "NICK") {
            return new NickCommand(srv, cli, params);
        }
        else if (cmdName == "USER") {
            return new UserCommand(srv, cli, params);
        }
        else if (cmdName == "JOIN") {
            return new JoinCommand(srv, cli, params);
        }
        else if (cmdName == "PRIVMSG") {
            return new PrivmsgCommand(srv, cli, params);
        }
        // ... autres commandes
        
        return nullptr; // Commande inconnue
    }
};
```

### Utilisation

```cpp
// Dans MessageParser::parse()
std::string cmdName = extractCommand(line);
std::vector<std::string> params = extractParams(line);

Command* cmd = CommandFactory::createCommand(cmdName, server, client, params);

if (cmd != nullptr) {
    cmd->execute();
    delete cmd;
} else {
    // Commande inconnue - erreur
    client->sendError("421", cmdName + " :Unknown command");
}
```

---

## 🎨 Version Avancée : Factory avec Map

Plus élégant et extensible :

```cpp
#include <map>
#include <functional>

class CommandFactory {
private:
    // Type pour la fonction de création
    using CommandCreator = std::function<Command*(Server*, Client*, const std::vector<std::string>&)>;
    
    // Map: nom de commande → fonction de création
    static std::map<std::string, CommandCreator> creators;
    
public:
    // Enregistrer une commande
    static void registerCommand(const std::string& name, CommandCreator creator) {
        creators[name] = creator;
    }
    
    // Créer une commande
    static Command* createCommand(
        const std::string& cmdName,
        Server* srv,
        Client* cli,
        const std::vector<std::string>& params
    ) {
        auto it = creators.find(cmdName);
        if (it != creators.end()) {
            return it->second(srv, cli, params);
        }
        return nullptr;
    }
};

// Initialisation de la map statique
std::map<std::string, CommandFactory::CommandCreator> CommandFactory::creators;
```

### Enregistrement des commandes

```cpp
// Dans l'initialisation du serveur
void Server::registerCommands() {
    CommandFactory::registerCommand("PASS", 
        [](Server* s, Client* c, const std::vector<std::string>& p) {
            return new PassCommand(s, c, p);
        });
    
    CommandFactory::registerCommand("NICK",
        [](Server* s, Client* c, const std::vector<std::string>& p) {
            return new NickCommand(s, c, p);
        });
    
    // ... autres commandes
}
```

---

## ⚠️ Application dans notre Projet IRC

### MessageParser comme Factory

```cpp
class MessageParser {
public:
    // Factory method principale
    static Command* parse(
        const std::string& message,
        Server* server,
        Client* client
    ) {
        std::vector<std::string> tokens = splitMessage(message);
        if (tokens.empty()) return nullptr;
        
        std::string cmdName = tokens[0];
        std::vector<std::string> params(tokens.begin() + 1, tokens.end());
        
        // Utilise la factory pour créer la commande
        return CommandFactory::createCommand(cmdName, server, client, params);
    }

private:
    static std::vector<std::string> splitMessage(const std::string& msg);
};
```

---

## ✅ Avantages

1. **Évite le couplage** entre le code de création et les classes concrètes
2. **Principe Open/Closed** - facile d'ajouter de nouvelles commandes
3. **Single Responsibility** - code de création centralisé
4. **Code plus propre** - pas de longues chaînes de if/else

---

## ❌ Inconvénients

1. **Complexité accrue** - plus de classes
2. **Peut être excessif** pour peu de types différents
3. **Besoin de sous-classer** pour chaque nouveau produit

---

## 🎓 Quand l'utiliser ?

✅ **Utilisez Factory Method quand** :
- Vous ne connaissez pas à l'avance les types exacts d'objets
- Vous voulez déléguer la création aux sous-classes
- Vous voulez éviter les longues chaînes de if/else pour la création
- **Notre cas : Créer des commandes IRC selon le message**

❌ **N'utilisez PAS Factory Method quand** :
- Vous n'avez que 2-3 types d'objets
- La logique de création est très simple
- Les types ne changeront jamais

---

## 🔄 Patterns Liés

- **Abstract Factory** - Créer des familles d'objets liés
- **Builder** - Construire des objets complexes étape par étape
- **Prototype** - Cloner des objets existants

---

## 📊 Factory vs if/else

| Aspect | Factory Method | if/else Chain |
|--------|---------------|---------------|
| Lisibilité | ✅ Excellent | ❌ Mauvais avec 10+ types |
| Maintenabilité | ✅ Facile | ❌ Difficile |
| Extensibilité | ✅ Oui | ❌ Modifier le code |
| Performance | ⚠️ Légèrement plus lent | ✅ Plus rapide |

---

## 🏋️ Exercice Pratique

**Pour MessageParser** :

1. Créez une classe `CommandFactory`
2. Implémentez `createCommand()` pour 5 commandes
3. Modifiez `MessageParser::parse()` pour utiliser la factory
4. Ajoutez une nouvelle commande - devrait être facile !

```cpp
// Test
Command* cmd = CommandFactory::createCommand("PRIVMSG", srv, cli, params);
if (cmd) {
    cmd->execute();
    delete cmd;
}
```

---

## 💡 Bonus : Factory avec Smart Pointers

Version moderne C++ :

```cpp
class CommandFactory {
public:
    static std::unique_ptr<Command> createCommand(
        const std::string& cmdName,
        Server* srv,
        Client* cli,
        const std::vector<std::string>& params
    ) {
        if (cmdName == "PASS") {
            return std::make_unique<PassCommand>(srv, cli, params);
        }
        // ...
        return nullptr;
    }
};

// Utilisation - pas besoin de delete !
auto cmd = CommandFactory::createCommand("NICK", srv, cli, params);
if (cmd) {
    cmd->execute();
} // Destruction automatique
```

---

## 📖 Pour aller plus loin

- [Refactoring.Guru - Factory Method](https://refactoring.guru/design-patterns/factory-method)
- [C++ Factory Pattern Examples](https://www.tutorialspoint.com/design_pattern/factory_pattern.htm)
- [Modern C++ Factory with lambdas](https://www.cppstories.com/2018/06/factory/)

---

## ⏱️ Temps d'apprentissage estimé

- **Lecture** : 25 min
- **Compréhension** : 20 min
- **Implémentation pratique** : 35 min
- **Total** : ~1h20
