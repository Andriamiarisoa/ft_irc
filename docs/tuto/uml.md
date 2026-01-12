# Tutoriel UML - Comprendre les Diagrammes de Classes

## Table des Matières
1. [Introduction à l'UML](#1-introduction-à-luml)
2. [Qu'est-ce qu'un Diagramme de Classes ?](#2-quest-ce-quun-diagramme-de-classes)
3. [Anatomie d'une Classe UML](#3-anatomie-dune-classe-uml)
4. [Les Visibilités](#4-les-visibilités)
5. [Les Relations entre Classes](#5-les-relations-entre-classes)
6. [Les Multiplicités](#6-les-multiplicités)
7. [Comprendre votre Diagramme IRC](#7-comprendre-votre-diagramme-irc)
8. [Exercices Pratiques](#8-exercices-pratiques)

---

## 1. Introduction à l'UML

### Qu'est-ce que l'UML ?

**UML** signifie **Unified Modeling Language** (Langage de Modélisation Unifié). C'est un langage visuel standardisé utilisé pour :
- **Concevoir** des systèmes logiciels
- **Documenter** l'architecture d'un programme
- **Communiquer** entre développeurs
- **Planifier** avant de coder

### Pourquoi utiliser l'UML ?

✅ **Visualisation** : Voir la structure complète d'un projet d'un coup d'œil  
✅ **Communication** : Partager sa vision avec l'équipe  
✅ **Conception** : Planifier avant d'écrire du code  
✅ **Documentation** : Créer une référence durable du système  

### Les différents types de diagrammes UML

UML propose 14 types de diagrammes, mais les plus courants sont :
- **Diagramme de Classes** (structure) ← Ce qu'on utilise ici
- Diagramme de Séquence (comportement)
- Diagramme de Cas d'Utilisation (fonctionnalités)
- Diagramme d'États (états d'un objet)

---

## 2. Qu'est-ce qu'un Diagramme de Classes ?

Un **diagramme de classes** représente la **structure statique** d'un système orienté objet :
- Les **classes** qui composent le système
- Leurs **attributs** (variables membres)
- Leurs **méthodes** (fonctions membres)
- Les **relations** entre ces classes

### Exemple Simple

```
┌─────────────────┐
│     Voiture     │
├─────────────────┤
│ - marque        │
│ - vitesse       │
├─────────────────┤
│ + demarrer()    │
│ + accelerer()   │
└─────────────────┘
```

Cette boîte représente une classe `Voiture` avec :
- 2 attributs : `marque` et `vitesse`
- 2 méthodes : `demarrer()` et `accelerer()`

---

## 3. Anatomie d'une Classe UML

Une classe UML est divisée en **3 compartiments** :

```
┌──────────────────────────────────┐
│         NOM DE LA CLASSE         │ ← Compartiment 1 : Nom
├──────────────────────────────────┤
│ - attribut1 : type               │
│ - attribut2 : type               │ ← Compartiment 2 : Attributs
│ # attribut3 : type               │
├──────────────────────────────────┤
│ + methode1() : type_retour       │
│ + methode2(param) : void         │ ← Compartiment 3 : Méthodes
│ - methode3() : type              │
└──────────────────────────────────┘
```

### 3.1 Le Nom de la Classe

- Écrit en **gras** et **centré**
- Commence par une **majuscule**
- Si la classe est **abstraite**, on ajoute `<<abstract>>` ou on écrit le nom en *italique*

**Exemple :**
```
┌──────────────────────┐
│   <<abstract>>       │
│      Command         │
└──────────────────────┘
```

### 3.2 Les Attributs

Format : `visibilité nom : type`

**Exemple :**
```
- port : int
- password : string
- serverSocket : int
```

Signification :
- `port` est un entier privé
- `password` est une chaîne privée
- `serverSocket` est un entier privé

### 3.3 Les Méthodes

Format : `visibilité nom(paramètres) : type_retour`

**Exemple :**
```
+ start() : void
+ getPort() : int
- setupSocket() : void
```

Signification :
- `start()` est publique, ne retourne rien
- `getPort()` est publique, retourne un int
- `setupSocket()` est privée, ne retourne rien

---

## 4. Les Visibilités

Les symboles devant les attributs et méthodes indiquent leur **niveau d'accès** :

| Symbole | Visibilité | Signification | Équivalent C++ |
|---------|------------|---------------|----------------|
| `+` | **Public** | Accessible partout | `public:` |
| `-` | **Private** | Accessible uniquement dans la classe | `private:` |
| `#` | **Protected** | Accessible dans la classe et ses sous-classes | `protected:` |
| `~` | **Package** | Accessible dans le même package (rare en C++) | — |

### Exemples Concrets

```cpp
class Client {
private:                    // - en UML
    int fd;
    std::string nickname;

protected:                  // # en UML
    std::string buffer;

public:                     // + en UML
    Client(int fd);
    int getFd();
    void sendMessage(std::string msg);
};
```

En UML :
```
┌────────────────────────────┐
│          Client            │
├────────────────────────────┤
│ - fd : int                 │
│ - nickname : string        │
│ # buffer : string          │
├────────────────────────────┤
│ + Client(fd)               │
│ + getFd() : int            │
│ + sendMessage(msg) : void  │
└────────────────────────────┘
```

---

## 5. Les Relations entre Classes

C'est la partie la plus importante ! Les relations montrent comment les classes interagissent.

### 5.1 Association Simple

**Symbole :** Ligne simple `────────`

**Signification :** Une classe "connaît" une autre classe

**Exemple :**
```
Client ──────── Channel
```
Un Client connaît un Channel

### 5.2 Agrégation

**Symbole :** Losange vide `◇────────`

**Signification :** Relation "a un" (has-a) **faible**. Les objets peuvent exister indépendamment.

**Exemple :**
```
University ◇──────── Student
```
Une université a des étudiants, mais si l'université ferme, les étudiants existent toujours.

### 5.3 Composition

**Symbole :** Losange plein `◆────────`

**Signification :** Relation "a un" (has-a) **forte**. Si le conteneur est détruit, les objets contenus le sont aussi.

**Exemple :**
```
Server ◆──────── Client
```
Le serveur possède des clients. Si le serveur s'arrête, les connexions clients sont fermées.

### 5.4 Héritage (Généralisation)

**Symbole :** Flèche triangulaire vide `────────▷`

**Signification :** Relation "est un" (is-a). Une classe hérite d'une autre.

**Exemple :**
```
PassCommand ────────▷ Command
```
`PassCommand` hérite de `Command` (PassCommand EST UN Command)

**En C++ :**
```cpp
class Command { /*...*/ };
class PassCommand : public Command { /*...*/ };
```

### 5.5 Dépendance

**Symbole :** Flèche en pointillés `- - - - ->`

**Signification :** Une classe utilise temporairement une autre (paramètre, variable locale)

**Exemple :**
```
MessageParser - - - - -> Command
             "creates"
```
MessageParser crée des objets Command mais ne les stocke pas.

### Tableau Récapitulatif

| Relation | Symbole | Lecture | Exemple |
|----------|---------|---------|---------|
| Association | `────` | "connaît" | Client ──── Channel |
| Agrégation | `◇────` | "a un" (faible) | Team ◇──── Player |
| Composition | `◆────` | "possède" (forte) | House ◆──── Room |
| Héritage | `────▷` | "est un" | Dog ────▷ Animal |
| Dépendance | `- - ->` | "utilise" | Parser - - -> Token |

---

## 6. Les Multiplicités

Les **multiplicités** indiquent combien d'instances d'une classe sont liées à une autre.

### Notation

```
ClasseA ──────── ClasseB
   1         *
```

Lecture : "1 instance de ClasseA est associée à plusieurs (0 ou plus) instances de ClasseB"

### Symboles de Multiplicité

| Symbole | Signification |
|---------|---------------|
| `1` | Exactement un |
| `0..1` | Zéro ou un |
| `*` | Zéro ou plusieurs |
| `1..*` | Un ou plusieurs |
| `n` | Exactement n |
| `n..m` | Entre n et m |

### Exemples Concrets

```
Server ◆────────── Client
   1           *
```
**Lecture :** Un serveur possède plusieurs (0 à n) clients.

```
Client ──────────── Channel
   *            *
```
**Lecture :** Un client peut être dans plusieurs channels, et un channel peut avoir plusieurs clients (relation many-to-many).

```
Person ────────── Passport
   1         0..1
```
**Lecture :** Une personne a 0 ou 1 passeport.

---

## 7. Comprendre votre Diagramme IRC

Analysons maintenant votre diagramme `uml.drawio` étape par étape.

### 7.1 Vue d'Ensemble

Votre système IRC se compose de **5 classes principales** :

1. **Server** - Le serveur IRC
2. **Client** - Un utilisateur connecté
3. **Channel** - Un salon de discussion
4. **Command** (abstraite) - Commande IRC abstraite
5. **MessageParser** - Analyseur de messages

Et **10 commandes concrètes** qui héritent de Command.

### 7.2 Classe Server

```
┌────────────────────────────────────┐
│            Server                  │
├────────────────────────────────────┤
│ - port : int                       │
│ - password : string                │
│ - serverSocket : int               │
│ - fds : vector                     │
│ - clients : map                    │
│ - channels : map                   │
├────────────────────────────────────┤
│ + Server(port, password)           │
│ + ~Server()                        │
│ + start() : void                   │
│ + stop() : void                    │
│ - setupSocket() : void             │
│ - handlePoll() : void              │
│ - acceptNewClient() : void         │
│ - handleClientMessage(fd) : void   │
│ - disconnectClient(fd) : void      │
│ - getClientByNick(nick) : Client   │
│ - getOrCreateChannel(name) : Chan  │
│ - executeCommand(client, cmd) : vo │
└────────────────────────────────────┘
```

**Analyse :**

**Attributs privés :**
- `port` : port d'écoute du serveur
- `password` : mot de passe requis pour se connecter
- `serverSocket` : file descriptor du socket serveur
- `fds` : vecteur de pollfd pour gérer les I/O non-bloquants
- `clients` : map des clients connectés (probablement `map<int, Client*>`)
- `channels` : map des channels existants (probablement `map<string, Channel*>`)

**Méthodes publiques :**
- Constructeur et destructeur
- `start()` et `stop()` : démarrer/arrêter le serveur

**Méthodes privées :**
- Méthodes internes pour gérer les sockets, les clients, etc.

### 7.3 Classe Client

```
┌────────────────────────────────┐
│          Client                │
├────────────────────────────────┤
│ - fd : int                     │
│ - nickname : string            │
│ - username : string            │
│ - realname : string            │
│ - hostname : string            │
│ - authenticated : bool         │
│ - registered : bool            │
│ - buffer : string              │
│ - channels : set               │
├────────────────────────────────┤
│ + Client(fd)                   │
│ + ~Client()                    │
│ + getFd() : int                │
│ + getNickname() : string       │
│ + getUsername() : string       │
│ + isAuthenticated() : bool     │
│ + isRegistered() : bool        │
│ + setNickname(nick) : void     │
│ + setUsername(user) : void     │
│ + authenticate() : void        │
│ + addToChannel(channel) : void │
│ + removeFromChannel(chan) : vo │
│ + appendToBuffer(data) : void  │
│ + extractCommand() : string    │
│ + sendMessage(msg) : void      │
└────────────────────────────────┘
```

**Analyse :**

**Attributs privés :**
- `fd` : file descriptor du socket client
- `nickname`, `username`, `realname` : informations utilisateur IRC
- `authenticated` : a-t-il fourni le bon mot de passe ?
- `registered` : a-t-il complété l'enregistrement (NICK + USER) ?
- `buffer` : buffer pour gérer les messages partiels TCP
- `channels` : ensemble des channels auxquels il appartient

**Note importante :** Le `buffer` est crucial ! TCP envoie des flux d'octets, pas des messages complets. On doit stocker les données partielles jusqu'à recevoir `\r\n`.

### 7.4 Classe Channel

```
┌────────────────────────────────────┐
│           Channel                  │
├────────────────────────────────────┤
│ - name : string                    │
│ - topic : string                   │
│ - key : string                     │
│ - userLimit : int                  │
│ - members : set                    │
│ - operators : set                  │
│ - invitedUsers : set               │
│ - inviteOnly : bool                │
│ - topicRestricted : bool           │
├────────────────────────────────────┤
│ + Channel(name)                    │
│ + ~Channel()                       │
│ + getName() : string               │
│ + getTopic() : string              │
│ + setTopic(topic, client) : void   │
│ + setKey(key) : void               │
│ + hasKey() : bool                  │
│ + checkKey(key) : bool             │
│ + addMember(client) : void         │
│ + removeMember(client) : void      │
│ + addOperator(client) : void       │
│ + removeOperator(client) : void    │
│ + isOperator(client) : bool        │
│ + isMember(client) : bool          │
│ + broadcast(msg, exclude) : void   │
│ + setInviteOnly(mode) : void       │
│ + setTopicRestricted(mode) : void  │
│ + setUserLimit(limit) : void       │
│ + inviteUser(client) : void        │
│ + isInvited(client) : bool         │
│ + kickMember(client, reason) : vo  │
└────────────────────────────────────┘
```

**Analyse :**

Un Channel gère :
- **Membres** : utilisateurs dans le channel
- **Opérateurs** : utilisateurs avec des privilèges
- **Modes** : restrictions d'accès et de comportement
  - `i` (invite-only) : invitation requise
  - `t` (topic restricted) : seuls les opérateurs peuvent changer le topic
  - `k` (key) : mot de passe requis
  - `l` (limit) : limite d'utilisateurs

### 7.5 Hiérarchie Command

```
                  ┌─────────────┐
                  │   Command   │ (abstract)
                  └──────┬──────┘
                         │
          ┌──────────────┼──────────────┬─────────────┐
          │              │              │             │
    ┌─────▼─────┐  ┌────▼────┐   ┌────▼────┐  ┌─────▼─────┐
    │   Pass    │  │  Nick   │   │  User   │  │   Join    │
    └───────────┘  └─────────┘   └─────────┘  └───────────┘
          │              │              │             │
    ┌─────▼─────┐  ┌────▼────┐   ┌────▼────┐  ┌─────▼─────┐
    │   Part    │  │ Privmsg │   │  Kick   │  │  Invite   │
    └───────────┘  └─────────┘   └─────────┘  └───────────┘
          │              │
    ┌─────▼─────┐  ┌────▼────┐
    │   Topic   │  │  Mode   │
    └───────────┘  └─────────┘
          │
    ┌─────▼─────┐
    │   Quit    │
    └───────────┘
```

**Pattern de conception :** C'est le **Command Pattern** !

Chaque commande IRC (PASS, NICK, JOIN, etc.) est une classe qui hérite de `Command` et implémente `execute()`.

**Avantages :**
- ✅ Facile d'ajouter de nouvelles commandes
- ✅ Chaque commande est isolée et testable
- ✅ Code propre et organisé

### 7.6 Les Relations Détaillées

#### 7.6.1 Server ◆──── Client (Composition 1 → *)

```
Server ◆────────── Client
   1           *
```

**Signification :**
- Le serveur **possède** ses clients
- Un serveur a plusieurs clients (0 à n)
- Si le serveur s'arrête, les clients sont déconnectés

**En code :**
```cpp
class Server {
private:
    std::map<int, Client*> clients;  // Le serveur possède les clients
};
```

#### 7.6.2 Client ──── Channel (Association * ↔ *)

```
Client ──────────── Channel
   *            *
```

**Signification :**
- Relation **many-to-many**
- Un client peut être dans plusieurs channels
- Un channel peut avoir plusieurs clients
- Ils existent indépendamment

**En code :**
```cpp
class Client {
private:
    std::set<Channel*> channels;  // Liste des channels du client
};

class Channel {
private:
    std::set<Client*> members;    // Liste des membres du channel
};
```

#### 7.6.3 MessageParser - - -> Command (Dépendance)

```
MessageParser - - - - -> Command
             "creates"
```

**Signification :**
- MessageParser **utilise** Command
- Il crée des instances de Command
- Il ne les stocke pas (dépendance temporaire)

**En code :**
```cpp
class MessageParser {
public:
    static Command* parse(std::string line, Server* srv, Client* cli) {
        // Analyse la ligne et crée la bonne commande
        if (cmd == "PASS")
            return new PassCommand(srv, cli, params);
        if (cmd == "NICK")
            return new NickCommand(srv, cli, params);
        // etc.
    }
};
```

#### 7.6.4 PassCommand ──▷ Command (Héritage)

```
PassCommand ────────▷ Command
```

**Signification :**
- PassCommand **hérite** de Command
- PassCommand **est un** Command
- Elle doit implémenter `execute()`

**En code :**
```cpp
class Command {  // Abstraite
protected:
    Server* server;
    Client* client;
    std::vector<std::string> params;
public:
    virtual void execute() = 0;  // Méthode virtuelle pure
};

class PassCommand : public Command {
public:
    void execute() override {
        // Implémentation spécifique de PASS
    }
};
```

---

## 8. Exercices Pratiques

### Exercice 1 : Lecture de Diagramme

Regardez votre diagramme et répondez :

1. Combien d'attributs privés la classe `Server` a-t-elle ?
   <details>
   <summary>Réponse</summary>
   6 attributs privés : port, password, serverSocket, fds, clients, channels
   </details>

2. Quelle classe possède la méthode `broadcast()` ?
   <details>
   <summary>Réponse</summary>
   Channel
   </details>

3. Combien de classes héritent de `Command` ?
   <details>
   <summary>Réponse</summary>
   10 classes : PassCommand, NickCommand, UserCommand, JoinCommand, PartCommand, PrivmsgCommand, KickCommand, InviteCommand, TopicCommand, ModeCommand, QuitCommand
   </details>

### Exercice 2 : Interprétation des Relations

1. Pourquoi la relation Server-Client est une **composition** et non une simple association ?
   <details>
   <summary>Réponse</summary>
   Car le serveur gère le cycle de vie des clients. Quand le serveur s'arrête, les connexions clients sont fermées. C'est une propriété forte.
   </details>

2. Pourquoi la relation Client-Channel est **bidirectionnelle** (* ↔ *) ?
   <details>
   <summary>Réponse</summary>
   Car un client doit connaître ses channels (pour savoir où envoyer des messages), et un channel doit connaître ses membres (pour broadcaster les messages).
   </details>

### Exercice 3 : Traduction en Code

Traduisez cette classe UML en C++ :

```
┌─────────────────────┐
│      Message        │
├─────────────────────┤
│ - content : string  │
│ - sender : Client*  │
│ - timestamp : time_t│
├─────────────────────┤
│ + Message(content)  │
│ + getSender()       │
│ + getContent()      │
└─────────────────────┘
```

<details>
<summary>Réponse</summary>

```cpp
class Message {
private:
    std::string content;
    Client* sender;
    time_t timestamp;

public:
    Message(const std::string& content)
        : content(content), sender(nullptr), timestamp(time(nullptr)) {}
    
    Client* getSender() const { return sender; }
    std::string getContent() const { return content; }
};
```
</details>

---

## 9. Conseils pour Créer vos Propres Diagrammes

### 9.1 Outils Recommandés

- **draw.io** (gratuit, en ligne ou desktop) ← Vous l'utilisez déjà !
- **PlantUML** (format texte, idéal pour Git)
- **Lucidchart** (en ligne, collaboratif)
- **Visual Paradigm** (professionnel)
- **StarUML** (desktop)

### 9.2 Bonnes Pratiques

1. **Commencez simple** : Identifiez d'abord les classes principales
2. **Ajoutez les relations** : Connectez les classes entre elles
3. **Détaillez progressivement** : Ajoutez attributs et méthodes
4. **Ne surchargez pas** : Trop de détails = illisible
5. **Utilisez des notes** : Pour expliquer les choix importants
6. **Gardez-le à jour** : Le diagramme doit refléter le code

### 9.3 Que Mettre (ou Pas) dans un Diagramme

**À inclure :**
- ✅ Classes principales du système
- ✅ Attributs et méthodes importants
- ✅ Relations significatives
- ✅ Multiplicités sur les associations
- ✅ Notes pour clarifier les concepts

**À éviter :**
- ❌ Toutes les méthodes getters/setters (sauf si pertinent)
- ❌ Détails d'implémentation (std::vector vs std::list)
- ❌ Classes utilitaires triviales
- ❌ Trop de relations qui créent un plat de spaghetti

---

## 10. Récapitulatif Final

### Ce que vous avez appris

1. ✅ **UML** = langage visuel pour modéliser des systèmes
2. ✅ **Diagramme de classes** = structure statique (classes + relations)
3. ✅ **3 compartiments** = Nom / Attributs / Méthodes
4. ✅ **Visibilités** = `+` public, `-` private, `#` protected
5. ✅ **Relations** :
   - Association `────` : "connaît"
   - Composition `◆────` : "possède"
   - Héritage `────▷` : "est un"
   - Dépendance `- - ->` : "utilise"
6. ✅ **Multiplicités** = combien d'instances sont liées

### Appliquer à votre projet IRC

Votre diagramme montre :
- **Architecture globale** : Server gère des Clients et des Channels
- **Pattern Command** : Chaque commande IRC = une classe
- **Relations claires** : Qui possède quoi, qui hérite de quoi
- **Détails importants** : Modes, buffers, authentification

### Prochaines étapes

1. 📖 Relisez le diagramme en appliquant ce tutoriel
2. 🔍 Identifiez chaque symbole et relation
3. 💻 Comparez le diagramme avec votre code
4. ✏️ Mettez à jour le diagramme si votre code évolue
5. 🎯 Utilisez-le comme référence pendant le développement

---

## 11. Ressources Supplémentaires

### Documentation Officielle
- [UML 2.5 Specification](https://www.omg.org/spec/UML/)
- [Draw.io Documentation](https://www.diagrams.net/doc/)

### Tutoriels
- [Visual Paradigm - UML Class Diagram](https://www.visual-paradigm.com/guide/uml-unified-modeling-language/uml-class-diagram-tutorial/)
- [Lucidchart - UML Tutorial](https://www.lucidchart.com/pages/uml-class-diagram)

### Livres
- "UML 2.0 en action" - Pascal Roques
- "Design Patterns" - Gang of Four (utilise UML)

---

## 12. Glossaire

| Terme | Définition |
|-------|------------|
| **UML** | Unified Modeling Language - langage de modélisation standardisé |
| **Classe** | Modèle pour créer des objets (attributs + méthodes) |
| **Attribut** | Variable membre d'une classe |
| **Méthode** | Fonction membre d'une classe |
| **Visibilité** | Niveau d'accès (public, private, protected) |
| **Association** | Relation "connaît" entre classes |
| **Composition** | Relation "possède" forte (cycle de vie dépendant) |
| **Agrégation** | Relation "a un" faible (cycle de vie indépendant) |
| **Héritage** | Relation "est un" (sous-classe hérite de super-classe) |
| **Dépendance** | Utilisation temporaire d'une classe |
| **Multiplicité** | Nombre d'instances dans une relation |
| **Classe abstraite** | Classe qui ne peut pas être instanciée |
| **Interface** | Classe 100% abstraite (uniquement des méthodes virtuelles pures) |

---

**Bon courage pour votre projet IRC ! 🚀**

*N'hésitez pas à enrichir ce document au fur et à mesure de votre apprentissage.*
