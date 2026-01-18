# Design Pattern : Facade

## 📚 Source
Basé sur [Refactoring.Guru - Facade](https://refactoring.guru/design-patterns/facade)

---

## 🎯 Objectif

Le **Facade** est un pattern structurel qui fournit une interface simplifiée à une bibliothèque, un framework ou un ensemble complexe de classes.

---

## 🤔 Problème

**Rejoindre un canal IRC** nécessite plusieurs étapes :
```cpp
// Code complexe sans Facade
Channel* chan = server->getOrCreateChannel("#general");
if (chan->isInviteOnly() && !chan->isInvited(client)) {
    client->sendError(473, "Cannot join +i channel");
    return;
}
if (chan->hasKey() && key != chan->getKey()) {
    client->sendError(475, "Bad channel key");
    return;
}
if (chan->isFull()) {
    client->sendError(471, "Channel is full");
    return;
}
chan->addMember(client);
chan->broadcast(joinMsg);
client->addChannel(chan);
chan->sendTopic(client);
chan->sendNames(client);
```

Trop complexe !

---

## ✅ Solution

```cpp
class ChannelFacade {
public:
    static bool joinChannel(
        Client* client, 
        const std::string& channelName, 
        const std::string& key = ""
    ) {
        Channel* chan = getOrCreate(channelName);
        
        // Toutes les vérifications encapsulées
        if (!canJoin(client, chan, key)) {
            return false;
        }
        
        // Toutes les actions encapsulées
        performJoin(client, chan);
        return true;
    }

private:
    static bool canJoin(Client* c, Channel* ch, const std::string& key);
    static void performJoin(Client* c, Channel* ch);
};
```

Utilisation simplifiée :
```cpp
if (ChannelFacade::joinChannel(client, "#general", "password")) {
    // Succès
}
```

---

## ⚠️ Application IRC

**Server peut servir de Facade** :
```cpp
class Server {
public:
    // Facade pour l'authentification complète
    bool authenticateClient(Client* client, 
                           const std::string& pass,
                           const std::string& nick, 
                           const std::string& user) {
        if (!checkPassword(pass)) return false;
        if (!setNickname(client, nick)) return false;
        if (!setUsername(client, user)) return false;
        sendWelcomeMessages(client);
        return true;
    }
    
    // Facade pour envoyer un message
    void sendMessage(Client* from, 
                    const std::string& target, 
                    const std::string& msg) {
        if (target[0] == '#') {
            sendToChannel(from, target, msg);
        } else {
            sendToUser(from, target, msg);
        }
    }
};
```

---

## ✅ Avantages

- **Simplifie l'utilisation** de systèmes complexes
- **Réduit les dépendances** entre clients et sous-systèmes
- **Point d'entrée unique** pour des opérations complexes

---

## ⏱️ Temps d'apprentissage : ~50min

