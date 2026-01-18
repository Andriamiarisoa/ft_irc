# Classe Command - Description de la Tâche

## Vue d'ensemble
La classe `Command` est une classe de base abstraite pour toutes les commandes IRC. Elle utilise le patron de conception Command pour encapsuler la logique d'exécution des commandes et fournir des fonctionnalités communes à toutes les commandes.

## Emplacement de la Classe
- **En-tête**: `includes/Command.hpp`
- **Implémentation**: `src/Command.cpp`

---

## Attributs Protégés

### Contexte
- `Server* server` - Pointeur vers l'instance du serveur
- `Client* client` - Pointeur vers le client qui a envoyé la commande
- `std::vector<std::string> params` - Paramètres de la commande

---

## Méthodes

### Constructeur
```cpp
Command(Server* srv, Client* cli, const std::vector<std::string>& params)
```

**Objectif**: Initialiser la commande avec le contexte

**TODO**:
- [ ] Stocker le pointeur du serveur dans la variable membre
- [ ] Stocker le pointeur du client dans la variable membre
- [ ] Copier le vecteur params dans la variable membre
- [ ] Valider que les pointeurs ne sont pas NULL

---

### Destructeur
```cpp
virtual ~Command()
```

**Objectif**: Destructeur virtuel pour le polymorphisme

**TODO**:
- [ ] Rendre le destructeur virtuel (déjà déclaré)
- [ ] Pas de nettoyage nécessaire (les pointeurs ne sont pas possédés par Command)
- [ ] S'assurer que les classes dérivées peuvent être supprimées via un pointeur de base

---

### execute() [Virtuel Pur]
```cpp
virtual void execute() = 0
```

**Objectif**: Exécuter la commande (implémenté par les classes dérivées)

**TODO**:
- [ ] Ceci est virtuel pur - pas d'implémentation dans la classe de base
- [ ] Chaque classe de commande dérivée doit implémenter ceci
- [ ] La logique de la commande va ici dans les classes dérivées

**Notes d'Implémentation pour les Classes Dérivées**:
- Vérifier si le client est enregistré (sauf PASS, NICK, USER)
- Valider le nombre de paramètres
- Valider le format des paramètres
- Effectuer la logique spécifique à la commande
- Envoyer les réponses/erreurs appropriées
- Mettre à jour l'état du serveur/client/canal

---

### sendReply(int code, const std::string& msg)
```cpp
void sendReply(int code, const std::string& msg)
```

**Objectif**: Envoyer une réponse numérique au client

**TODO**:
- [ ] Formater le message avec le format de réponse numérique IRC
- [ ] Format: ":servername code clientnick msg\r\n"
- [ ] Utiliser le nom d'hôte du serveur (peut être codé en dur pour l'instant)
- [ ] Obtenir le surnom du client depuis l'objet client
- [ ] Construire la chaîne de réponse complète
- [ ] Appeler client->sendMessage() avec la réponse formatée
- [ ] Gérer les clients non enregistrés (utiliser "*" comme surnom)

**Format de Réponse**:
```
:irc.server 001 nickname :Welcome to the IRC network
```

**Codes de Réponse Courants**:
- RPL_WELCOME (001): Message de bienvenue
- RPL_TOPIC (332): Sujet du canal
- RPL_NAMREPLY (353): Liste des noms
- Voir RFC 1459 pour la liste complète

---

### sendError(int code, const std::string& msg)
```cpp
void sendError(int code, const std::string& msg)
```

**Objectif**: Envoyer une réponse d'erreur au client

**TODO**:
- [ ] Formater le message avec le format d'erreur IRC
- [ ] Format: ":servername code clientnick :msg\r\n"
- [ ] Similaire à sendReply mais pour les erreurs
- [ ] Construire la chaîne d'erreur complète
- [ ] Appeler client->sendMessage() avec l'erreur formatée
- [ ] Gérer les clients non enregistrés (utiliser "*" comme surnom)

**Format d'Erreur**:
```
:irc.server 461 nickname NICK :Not enough parameters
```

**Codes d'Erreur Courants**:
- ERR_NEEDMOREPARAMS (461): Pas assez de paramètres
- ERR_ALREADYREGISTRED (462): Déjà enregistré
- ERR_PASSWDMISMATCH (464): Mot de passe incorrect
- ERR_NONICKNAMEGIVEN (431): Aucun surnom fourni
- ERR_ERRONEUSNICKNAME (432): Surnom invalide
- ERR_NICKNAMEINUSE (433): Surnom déjà utilisé
- ERR_NOTONCHANNEL (442): Pas sur ce canal
- ERR_CHANOPRIVSNEEDED (482): Pas opérateur du canal
- Voir RFC 1459 pour la liste complète

---

## Patron de Conception

### Avantages du Patron Command
- **Encapsulation**: Chaque commande encapsule sa logique
- **Polymorphisme**: Le serveur appelle execute() sans connaître le type de commande
- **Extensibilité**: Facile d'ajouter de nouvelles commandes
- **Testabilité**: Les commandes peuvent être testées indépendamment

### Flux d'Utilisation
1. Le client envoie un message: "JOIN #channel"
2. MessageParser::parse() crée un objet JoinCommand
3. Le serveur appelle command->execute()
4. JoinCommand implémente la logique spécifique JOIN
5. La commande est supprimée après l'exécution

---

## Priorité d'Implémentation

### Phase 1 - Implémentation de Base
1. Constructeur - stocker le contexte
2. Destructeur - s'assurer qu'il est virtuel
3. sendReply() - formatage de base
4. sendError() - formatage de base

### Phase 2 - Amélioration
5. Ajouter la configuration du nom d'hôte
6. Ajouter la gestion appropriée des surnoms pour les clients non enregistrés
7. Ajouter la journalisation pour les réponses/erreurs

---

## Liste de Vérification pour les Tests

- [ ] Le constructeur stocke tous les paramètres correctement
- [ ] Le destructeur est virtuel (tester la suppression polymorphe)
- [ ] sendReply formate les messages correctement
- [ ] sendError formate les erreurs correctement
- [ ] Le format de réponse correspond au standard IRC
- [ ] Les messages se terminent par \r\n
- [ ] Les clients non enregistrés sont gérés (surnom = *)

---

## Notes sur le Protocole IRC

### Format de Message
```
:prefix command params :trailing
```

### Format de Réponse Numérique
```
:servername code nickname :message
```

### Exemples
```
:irc.server 001 john :Welcome to the IRC network
:irc.server 332 john #general :This is the topic
:irc.server 461 john JOIN :Not enough parameters
:irc.server 482 john #general :You're not channel operator
```

### Codes de Réponse Importants

#### Réponses de Succès (001-099)
- 001 RPL_WELCOME
- 002 RPL_YOURHOST
- 003 RPL_CREATED
- 004 RPL_MYINFO

#### Réponses de Commande (200-399)
- 331 RPL_NOTOPIC
- 332 RPL_TOPIC
- 341 RPL_INVITING
- 353 RPL_NAMREPLY
- 366 RPL_ENDOFNAMES

#### Erreurs (400-599)
- 401 ERR_NOSUCHNICK
- 403 ERR_NOSUCHCHANNEL
- 431 ERR_NONICKNAMEGIVEN
- 432 ERR_ERRONEUSNICKNAME
- 433 ERR_NICKNAMEINUSE
- 441 ERR_USERNOTINCHANNEL
- 442 ERR_NOTONCHANNEL
- 461 ERR_NEEDMOREPARAMS
- 462 ERR_ALREADYREGISTRED
- 464 ERR_PASSWDMISMATCH
- 482 ERR_CHANOPRIVSNEEDED

---

## Fichiers Associés
- `MessageParser.hpp/cpp` - Crée les objets Command
- `Server.hpp/cpp` - Exécute les commandes
- `Client.hpp/cpp` - Cible de la commande
- Toutes les implémentations de commandes (PassCommand, NickCommand, etc.)

---

## Exemple d'Implémentation (pour les classes dérivées)

```cpp
class PassCommand : public Command {
public:
    PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
        : Command(srv, cli, params) {}
    
    void execute() {
        // 1. Vérifier les paramètres
        if (params.size() < 1) {
            sendError(461, "PASS :Not enough parameters");
            return;
        }
        
        // 2. Vérifier l'état
        if (client->isRegistered()) {
            sendError(462, ":You may not reregister");
            return;
        }
        
        // 3. Effectuer l'action
        if (params[0] == server->getPassword()) {
            client->authenticate();
        } else {
            sendError(464, ":Password incorrect");
        }
    }
};
```

---

## Vue d'ensemble des Classes Dérivées

### Commandes d'Authentification
- **PassCommand**: Authentification par mot de passe
- **NickCommand**: Définir le surnom
- **UserCommand**: Définir le nom d'utilisateur

### Commandes de Canal
- **JoinCommand**: Rejoindre un canal
- **PartCommand**: Quitter un canal
- **TopicCommand**: Voir/définir le sujet
- **InviteCommand**: Inviter un utilisateur
- **KickCommand**: Expulser un utilisateur

### Messagerie
- **PrivmsgCommand**: Envoyer un message

### Modes de Canal
- **ModeCommand**: Définir les modes du canal

### Connexion
- **QuitCommand**: Déconnecter le client
