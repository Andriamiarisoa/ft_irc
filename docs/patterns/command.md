# Design Pattern : Command

## 📚 Source
Basé sur [Refactoring.Guru - Command Pattern](https://refactoring.guru/design-patterns/command)

---

## 🎯 Objectif

Le **Command** est un pattern comportemental qui transforme une requête en un objet autonome contenant toute l'information sur la requête. Cette transformation permet de paramétrer des méthodes avec différentes requêtes, de mettre en file d'attente ou de journaliser les requêtes, et de supporter les opérations annulables.

---

## 🤔 Problème

**Dans notre projet IRC**, nous recevons des messages texte comme :
```
PRIVMSG #channel :Hello world
KICK #channel user :reason
MODE #channel +i
```

Sans pattern Command, notre code ressemblerait à :

```cpp
void handleMessage(std::string msg, Server* srv, Client* cli) {
    if (msg.startswith("PRIVMSG")) {
        // 50 lignes de code PRIVMSG
    } else if (msg.startswith("KICK")) {
        // 40 lignes de code KICK
    } else if (msg.startswith("MODE")) {
        // 100 lignes de code MODE
    }
    // ... 10+ commandes
}
```

**Problèmes** :
- Fonction gigantesque et illisible
- Impossible de tester les commandes séparément
- Difficile d'ajouter de nouvelles commandes
- Pas de réutilisation du code

---

## ✅ Solution

Encapsuler chaque commande dans sa propre classe avec une méthode `execute()` :

```cpp
class Command {
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

class PrivmsgCommand : public Command {
public:
    void execute() override {
        // Logique PRIVMSG ici
    }
};
```

---

## 📝 Structure

```
┌─────────────────────┐
│     Command         │◄────── Interface
├─────────────────────┤
│ + execute()         │
└─────────────────────┘
         △
         │
    ┌────┴────┬────────────┬──────────┐
    │         │            │          │
┌───────┐ ┌───────┐  ┌───────┐  ┌───────┐
│ Pass  │ │ Nick  │  │ Join  │  │ Kick  │
│Command│ │Command│  │Command│  │Command│
└───────┘ └───────┘  └───────┘  └───────┘

┌─────────────────────┐
│    Invoker          │
├─────────────────────┤
│ - command: Command  │
├─────────────────────┤
│ + setCommand()      │
│ + executeCommand()  │
└─────────────────────┘
```

---

## 💻 Implémentation C++

### 1. Interface Command (Classe de Base)

```cpp
// Command.hpp
class Command {
protected:
    Server* server;
    Client* client;
    std::vector<std::string> params;

public:
    Command(Server* srv, Client* cli, const std::vector<std::string>& p)
        : server(srv), client(cli), params(p) {}
    
    virtual ~Command() = default;
    
    // Méthode principale à implémenter
    virtual void execute() = 0;
    
    // Méthodes utilitaires communes
    void sendReply(int code, const std::string& message) {
        std::string reply = ":" + server->getName() + " " + 
                           std::to_string(code) + " " + 
                           client->getNickname() + " " + message + "\r\n";
        client->sendMessage(reply);
    }
    
    void sendError(int code, const std::string& message) {
        sendReply(code, message);
    }
};
```

### 2. Commandes Concrètes

```cpp
// PassCommand.hpp
class PassCommand : public Command {
public:
    PassCommand(Server* srv, Client* cli, const std::vector<std::string>& p)
        : Command(srv, cli, p) {}
    
    void execute() override {
        // Validation
        if (params.empty()) {
            sendError(461, "PASS :Not enough parameters");
            return;
        }
        
        if (client->isRegistered()) {
            sendError(462, ":You may not reregister");
            return;
        }
        
        // Exécution
        if (params[0] == server->getPassword()) {
            client->setAuthenticated(true);
        } else {
            sendError(464, ":Password incorrect");
        }
    }
};

// NickCommand.hpp
class NickCommand : public Command {
public:
    NickCommand(Server* srv, Client* cli, const std::vector<std::string>& p)
        : Command(srv, cli, p) {}
    
    void execute() override {
        if (params.empty()) {
            sendError(431, ":No nickname given");
            return;
        }
        
        std::string newNick = params[0];
        
        // Validation du nickname
        if (!isValidNickname(newNick)) {
            sendError(432, newNick + " :Erroneous nickname");
            return;
        }
        
        // Vérifier si déjà utilisé
        if (server->nicknameInUse(newNick)) {
            sendError(433, newNick + " :Nickname is already in use");
            return;
        }
        
        // Définir le nickname
        client->setNickname(newNick);
        
        // Si enregistrement complet
        if (client->isFullyRegistered()) {
            sendWelcome();
        }
    }

private:
    bool isValidNickname(const std::string& nick) {
        // Implémentation validation
        return true;
    }
    
    void sendWelcome() {
        sendReply(001, ":Welcome to the IRC Network");
    }
};
```

### 3. Invoker (MessageParser)

```cpp
// MessageParser.cpp
Command* MessageParser::parse(const std::string& line, Server* srv, Client* cli) {
    std::vector<std::string> tokens = split(line);
    if (tokens.empty()) return nullptr;
    
    std::string cmdName = tokens[0];
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    
    // Création via Factory
    return CommandFactory::createCommand(cmdName, srv, cli, params);
}
```

### 4. Utilisation

```cpp
// Dans Server::handleClientMessage()
void Server::handleClientMessage(Client* client, const std::string& message) {
    // Parser crée la commande appropriée
    Command* cmd = MessageParser::parse(message, this, client);
    
    if (cmd != nullptr) {
        // Exécution de la commande
        cmd->execute();
        
        // Nettoyage
        delete cmd;
    } else {
        // Commande inconnue
        client->sendError(421, "Unknown command");
    }
}
```

---

## ⚠️ Application dans notre Projet IRC

### Hiérarchie des Commandes

```
Command (abstract)
├── PassCommand      (authentification)
├── NickCommand      (pseudonyme)
├── UserCommand      (informations utilisateur)
├── JoinCommand      (rejoindre canal)
├── PartCommand      (quitter canal)
├── PrivmsgCommand   (messages)
├── KickCommand      (expulser)
├── InviteCommand    (inviter)
├── TopicCommand     (sujet)
├── ModeCommand      (modes)
└── QuitCommand      (déconnexion)
```

Chaque commande :
1. Hérite de `Command`
2. Implémente `execute()`
3. A accès à `server`, `client`, `params`
4. Utilise `sendReply()` et `sendError()`

---

## ✅ Avantages

1. **Single Responsibility** - Chaque commande dans sa propre classe
2. **Open/Closed** - Ajouter des commandes sans modifier le code existant
3. **Testabilité** - Tester chaque commande indépendamment
4. **Réutilisabilité** - Code commun dans la classe de base
5. **Découplage** - L'invocateur ne connaît pas les détails des commandes
6. **Extensibilité** - Facile d'ajouter undo, logging, queuing

---

## ❌ Inconvénients

1. **Nombre de classes** - Une classe par commande (11+ dans notre projet)
2. **Complexité** - Plus de fichiers à gérer
3. **Overhead** - Création/destruction d'objets

---

## 🎓 Quand l'utiliser ?

✅ **Utilisez Command quand** :
- Vous voulez paramétrer des objets avec des opérations
- Vous voulez mettre des opérations en file d'attente
- Vous voulez supporter undo/redo
- **Notre cas : Gérer différentes commandes IRC de manière propre**

❌ **N'utilisez PAS Command quand** :
- Les opérations sont très simples
- Vous n'avez que 2-3 opérations
- Les opérations ne changent jamais

---

## 🔄 Extensions Possibles

### 1. Commandes Annulables (Undo)

```cpp
class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;  // Annuler la commande
};

class KickCommand : public Command {
private:
    std::string kickedUser;
    Channel* channel;

public:
    void execute() override {
        kickedUser = params[0];
        channel->removeMember(client);
    }
    
    void undo() override {
        // Re-ajouter l'utilisateur
        channel->addMember(client);
    }
};
```

### 2. Macro Commandes

```cpp
class MacroCommand : public Command {
private:
    std::vector<Command*> commands;

public:
    void addCommand(Command* cmd) {
        commands.push_back(cmd);
    }
    
    void execute() override {
        for (Command* cmd : commands) {
            cmd->execute();
        }
    }
};
```

### 3. Logging des Commandes

```cpp
class LoggingCommand : public Command {
private:
    Command* wrappedCmd;

public:
    void execute() override {
        std::cout << "Executing: " << typeid(*wrappedCmd).name() << std::endl;
        wrappedCmd->execute();
        std::cout << "Finished." << std::endl;
    }
};
```

---

## 📊 Command vs Function Pointers

| Aspect | Command Pattern | Function Pointers |
|--------|----------------|-------------------|
| État | ✅ Peut stocker des données | ❌ Pas d'état |
| Undo/Redo | ✅ Facile | ❌ Difficile |
| Polymorphisme | ✅ Oui | ⚠️ Limité |
| Lisibilité | ✅ Excellent | ⚠️ Moyen |

---

## 🏋️ Exercice Pratique

**Implémentez 3 commandes** :

1. **PassCommand** - Authentification
2. **NickCommand** - Définir pseudonyme  
3. **JoinCommand** - Rejoindre un canal

Pour chaque :
- Créez la classe héritant de `Command`
- Implémentez `execute()`
- Gérez les erreurs avec `sendError()`
- Testez avec différents paramètres

```cpp
// Test
Server srv(6667, "pass");
Client cli(5);
std::vector<std::string> params = {"secret"};

PassCommand cmd(&srv, &cli, params);
cmd.execute();

// Vérifier que le client est authentifié
assert(cli.isAuthenticated());
```

---

## 💡 Conseils d'Implémentation

1. **Commencez simple** - Implémentez execute() d'abord
2. **Factorisez le code commun** - Mettez les méthodes utilitaires dans Command
3. **Validez toujours** - Vérifiez les paramètres avant d'exécuter
4. **Gérez les erreurs** - Utilisez les codes d'erreur IRC standards
5. **Testez séparément** - Chaque commande doit être testable isolément

---

## 📖 Pour aller plus loin

- [Refactoring.Guru - Command](https://refactoring.guru/design-patterns/command)
- [IRC RFC 1459](https://tools.ietf.org/html/rfc1459) - Spécification des commandes
- [Command Pattern in C++](https://sourcemaking.com/design_patterns/command/cpp/1)

---

## ⏱️ Temps d'apprentissage estimé

- **Lecture** : 30 min
- **Compréhension** : 20 min
- **Implémentation pratique** : 40 min
- **Total** : ~1h30
