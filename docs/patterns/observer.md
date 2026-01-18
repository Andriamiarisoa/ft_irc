# Design Pattern : Observer

## 📚 Source
Basé sur [Refactoring.Guru - Observer](https://refactoring.guru/design-patterns/observer)

---

## 🎯 Objectif

L'**Observer** est un pattern comportemental qui définit un mécanisme de souscription permettant de notifier plusieurs objets des événements qui se produisent sur l'objet qu'ils observent.

---

## 🤔 Problème

**Dans un serveur IRC**, quand un événement se produit dans un canal (message, kick, changement de topic), tous les membres doivent être notifiés.

Sans Observer :
```cpp
void Channel::sendMessage(std::string msg, Client* sender) {
    // Notifier manuellement chaque membre
    for (Client* member : members) {
        if (member != sender) {
            member->sendMessage(msg);
        }
    }
}

void Channel::kickUser(Client* kicked) {
    for (Client* member : members) {
        member->sendMessage("User kicked");
    }
}
```

**Problème** : Code répétitif, couplage fort.

---

## ✅ Solution

```cpp
class Observer {
public:
    virtual void update(const std::string& event, const std::string& data) = 0;
};

class Subject {
private:
    std::vector<Observer*> observers;
public:
    void attach(Observer* obs) { observers.push_back(obs); }
    void detach(Observer* obs) { /* remove */ }
    void notify(const std::string& event, const std::string& data) {
        for (Observer* obs : observers) {
            obs->update(event, data);
        }
    }
};
```

---

## 💻 Application IRC

```cpp
// Client est un Observer
class Client : public Observer {
public:
    void update(const std::string& event, const std::string& data) override {
        if (event == "MESSAGE") {
            sendMessage(data);
        } else if (event == "KICK") {
            // Gérer kick
        }
    }
};

// Channel est un Subject
class Channel : public Subject {
public:
    void broadcast(const std::string& msg, Client* sender) {
        std::string formattedMsg = ":" + sender->getNick() + " " + msg;
        notify("MESSAGE", formattedMsg);
    }
};
```

---

## ✅ Avantages

- **Découplage** : Subject ne connaît pas les détails des observers
- **Dynamique** : Ajouter/retirer des observers à runtime
- **Broadcast** : Notifier plusieurs objets facilement

---

## ⏱️ Temps d'apprentissage : ~1h

