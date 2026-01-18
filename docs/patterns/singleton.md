# Design Pattern : Singleton

## 📚 Source
Basé sur [Refactoring.Guru - Singleton](https://refactoring.guru/design-patterns/singleton)

---

## 🎯 Objectif

Le **Singleton** est un pattern créationnel qui garantit qu'une classe n'a qu'une seule instance et fournit un point d'accès global à cette instance.

---

## 🤔 Problème

Parfois, vous devez vous assurer qu'une classe n'a qu'une seule instance. Par exemple :
- Une connexion à une base de données
- Un gestionnaire de configuration
- **Dans notre projet : Le serveur IRC (Server)**

Pourquoi ? Car avoir plusieurs instances du serveur causerait :
- Conflits de ports
- États incohérents
- Gaspillage de ressources

---

## ✅ Solution

Le Singleton résout ce problème en :

1. **Rendant le constructeur privé** - empêche l'instanciation directe
2. **Créant une méthode statique `getInstance()`** - retourne toujours la même instance
3. **Stockant l'instance dans une variable statique**

---

## 📝 Structure

```
┌─────────────────────────────┐
│       Singleton             │
├─────────────────────────────┤
│ - static instance: Singleton│
├─────────────────────────────┤
│ - Singleton()               │ ← Constructeur privé
│ + static getInstance()      │ ← Méthode d'accès
└─────────────────────────────┘
```

---

## 💻 Implémentation C++

### Version Simple

```cpp
class Server {
private:
    static Server* instance;
    int port;
    
    // Constructeur privé
    Server(int p) : port(p) {
        std::cout << "Server created on port " << port << std::endl;
    }
    
    // Empêcher la copie
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    static Server* getInstance(int port = 6667) {
        if (instance == nullptr) {
            instance = new Server(port);
        }
        return instance;
    }
    
    void start() {
        std::cout << "Server starting on port " << port << std::endl;
    }
};

// Initialisation de la variable statique
Server* Server::instance = nullptr;
```

### Utilisation

```cpp
int main() {
    Server* server1 = Server::getInstance(6667);
    server1->start();
    
    Server* server2 = Server::getInstance(8080); // Ignore le port, retourne server1
    
    // server1 et server2 pointent vers la même instance
    std::cout << (server1 == server2) << std::endl; // Affiche: 1 (true)
    
    return 0;
}
```

---

## 🔒 Version Thread-Safe (C++11+)

```cpp
class Server {
private:
    int port;
    
    Server(int p) : port(p) {}
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    static Server& getInstance(int port = 6667) {
        // Thread-safe depuis C++11 (magic static)
        static Server instance(port);
        return instance;
    }
    
    void start() {
        std::cout << "Server starting on port " << port << std::endl;
    }
};
```

Cette version utilise les **magic statics** de C++11 qui garantissent l'initialisation thread-safe.

---

## ⚠️ Application dans notre Projet IRC

Dans notre serveur IRC, le Singleton peut être utilisé pour :

### Server (Principal)
```cpp
class Server {
private:
    static Server* instance;
    int serverSocket;
    std::map<int, Client*> clients;
    
    Server(int port, const std::string& password);
    
public:
    static Server* getInstance(int port, const std::string& password);
    void start();
    void stop();
};
```

**Pourquoi ?**
- Un seul serveur IRC doit tourner
- Point d'accès global pour les commandes
- Évite les conflits de port

---

## ✅ Avantages

1. **Instance unique garantie** - impossible de créer plusieurs instances
2. **Accès global** - accessible de partout
3. **Initialisation paresseuse** - créée uniquement quand nécessaire
4. **Contrôle strict** - le constructeur privé empêche l'instanciation externe

---

## ❌ Inconvénients

1. **Violation du principe de responsabilité unique** - gère sa création ET sa logique
2. **Difficile à tester** - état global, difficile de mock
3. **Peut masquer une mauvaise conception** - dépendances cachées
4. **Problèmes en multithread** - sans précautions

---

## 🎓 Quand l'utiliser ?

✅ **Utilisez Singleton quand** :
- Vous devez contrôler l'accès à une ressource partagée (DB, fichier, socket)
- Une seule instance doit exister dans tout le programme
- L'instance doit être accessible globalement

❌ **N'utilisez PAS Singleton quand** :
- Vous pouvez passer l'objet en paramètre
- Vous avez besoin de plusieurs instances dans le futur
- Vous voulez faciliter les tests unitaires

---

## 🔄 Alternatives

1. **Injection de dépendances** - Passer l'instance en paramètre
2. **Factory** - Contrôler la création via une fabrique
3. **Monostate** - Instance unique mais état partagé

---

## 📊 Singleton vs Global Variable

| Aspect | Singleton | Variable Globale |
|--------|-----------|------------------|
| Contrôle | ✅ Constructeur privé | ❌ Aucun contrôle |
| Initialisation | ✅ Lazy loading | ❌ Au démarrage |
| Encapsulation | ✅ Méthodes privées | ❌ Accès direct |
| Tests | ⚠️ Difficile | ❌ Très difficile |

---

## 🏋️ Exercice Pratique

**Pour votre serveur IRC** :

1. Rendez le constructeur de `Server` privé
2. Ajoutez une méthode statique `getInstance()`
3. Testez que deux appels retournent la même instance
4. Essayez de créer un `Server` normalement → doit échouer à la compilation

```cpp
// Dans main.cpp
Server* srv1 = Server::getInstance(6667, "password");
Server* srv2 = Server::getInstance(8080, "other");

// srv1 == srv2 devrait être vrai
// Le port et password de srv2 sont ignorés
```

---

## 📖 Pour aller plus loin

- [Refactoring.Guru - Singleton](https://refactoring.guru/design-patterns/singleton)
- [C++ Core Guidelines - Avoid singletons](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i3-avoid-singletons)
- [Modern C++ Singleton](https://stackoverflow.com/questions/1008019/c-singleton-design-pattern)

---

## ⏱️ Temps d'apprentissage estimé

- **Lecture** : 20 min
- **Compréhension** : 15 min
- **Implémentation pratique** : 25 min
- **Total** : ~1h
