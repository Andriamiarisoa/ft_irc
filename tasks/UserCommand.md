# Classe UserCommand - Description de la Tâche

## Vue d'ensemble
La classe `UserCommand` gère la commande USER pour définir le nom d'utilisateur et le nom réel d'un client. C'est la dernière étape de l'enregistrement du client (après PASS et NICK).

## Emplacement de la Classe
- **Header**: `includes/UserCommand.hpp`
- **Implémentation**: `src/UserCommand.cpp`

## Commande IRC
**Syntaxe**: `USER <username> <mode> <unused> :<realname>`

---

## Méthodes

### Constructeur
```cpp
UserCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande USER

**TODO**:
- [ ] Appeler le constructeur de la classe de base
- [ ] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Définir le nom d'utilisateur du client et terminer l'enregistrement

**TODO**:

#### Étape 1: Vérifier l'Authentification
- [ ] Vérifier que le client est authentifié (a passé PASS)
- [ ] Sinon: envoyer une erreur et retourner
  ```cpp
  if (!client->isAuthenticated()) {
      sendError(464, ":You must send PASS first");
      return;
  }
  ```

#### Étape 2: Valider les Paramètres
- [ ] Vérifier que params contient au moins 4 éléments
- [ ] Format: USER <username> <mode> <unused> :<realname>
- [ ] Si insuffisant: envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "USER :Not enough parameters");
  return;
  ```

#### Étape 3: Vérifier l'État d'Enregistrement
- [ ] Vérifier si le client est déjà enregistré
- [ ] Si oui: envoyer ERR_ALREADYREGISTRED (462)
  ```cpp
  if (client->isRegistered()) {
      sendError(462, ":You may not reregister");
      return;
  }
  ```

#### Étape 4: Définir le Nom d'Utilisateur
- [ ] Extraire le nom d'utilisateur de params[0]
- [ ] Appeler client->setUsername(params[0])
- [ ] Stocker le nom réel si nécessaire (params[3])
- [ ] Note: mode (params[1]) et unused (params[2]) typiquement ignorés

#### Étape 5: Vérifier la Complétion de l'Enregistrement
- [ ] Vérifier si le client a maintenant à la fois un surnom et un nom d'utilisateur
- [ ] Si oui: le client est entièrement enregistré!
- [ ] Envoyer la séquence de messages de bienvenue (001-004)

#### Étape 6: Envoyer les Messages de Bienvenue
```cpp
// 001 RPL_WELCOME
sendReply(001, ":Welcome to the IRC Network " + 
          client->getNickname() + "!" + client->getUsername() + "@host");

// 002 RPL_YOURHOST
sendReply(002, ":Your host is irc.server, running version 1.0");

// 003 RPL_CREATED
sendReply(003, ":This server was created " + getCreationDate());

// 004 RPL_MYINFO
sendReply(004, "irc.server 1.0 o itkol");
```

---

## Détails des Paramètres

### Format de la Commande USER
```
USER <username> <mode> <unused> :<realname>
```

### Paramètres
1. **username**: Nom d'utilisateur du client (identifiant)
2. **mode**: Drapeaux de mode utilisateur (typiquement "0" ou "8")
3. **unused**: Paramètre inutilisé (typiquement "*")
4. **realname**: Nom réel du client (peut contenir des espaces)

### Exemple
```
USER john 0 * :John Doe
  ↓    ↓  ↓ ↓  ↓
  user  |  | |  realname (paramètre trailing)
        |  | unused
        |  mode
        username
```

---

## Priorité d'Implémentation

### Phase 1 - Implémentation de Base
1. Validation du nombre de paramètres
2. Vérification si déjà enregistré
3. Définir le nom d'utilisateur sur le client

### Phase 2 - Complétion de l'Enregistrement
4. Vérifier si le surnom est également défini
5. Marquer le client comme enregistré
6. Envoyer les messages de bienvenue (001-004)

### Phase 3 - Vérification de l'Authentification
7. Vérifier que PASS a été envoyé en premier
8. Rejeter si non authentifié

### Phase 4 - Amélioration
9. Stocker le nom réel (optionnel)
10. Gérer le paramètre mode (optionnel)
11. Formater correctement les messages de bienvenue

---

## Liste de Vérification des Tests

- [ ] USER avec 4 paramètres définit le nom d'utilisateur
- [ ] USER avec < 4 paramètres envoie l'erreur 461
- [ ] USER sans PASS d'abord est rejeté
- [ ] USER après l'enregistrement envoie l'erreur 462
- [ ] USER + NICK complète l'enregistrement
- [ ] Messages de bienvenue envoyés lors de l'enregistrement
- [ ] Nom réel avec espaces géré correctement
- [ ] Ordre: PASS → NICK → USER fonctionne
- [ ] Ordre: PASS → USER → NICK fonctionne
- [ ] Client marqué comme enregistré après USER

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **461 ERR_NEEDMOREPARAMS**: "<command> :Not enough parameters"
- **462 ERR_ALREADYREGISTRED**: ":You may not reregister"
- **464 ERR_PASSWDMISMATCH**: ":Password incorrect" (pour PASS manquant)

### Codes de Succès (Séquence de Bienvenue)
- **001 RPL_WELCOME**: ":Welcome to the IRC Network <nick>!<user>@<host>"
- **002 RPL_YOURHOST**: ":Your host is <server>, running version <ver>"
- **003 RPL_CREATED**: ":This server was created <date>"
- **004 RPL_MYINFO**: "<server> <version> <usermodes> <channelmodes>"

### Exemple d'Utilisation
```
Client: USER john 0 * :John Doe
Server: :irc.server 001 john :Welcome to the IRC Network john!john@host
Server: :irc.server 002 john :Your host is irc.server, running version 1.0
Server: :irc.server 003 john :This server was created Jan 12 2026
Server: :irc.server 004 john irc.server 1.0 o itkol

Client: USER
Server: :irc.server 461 * USER :Not enough parameters

Client: USER anotheruser 0 * :Another Name
Server: :irc.server 462 john :You may not reregister
```

---

## Problèmes Courants

### Problème: Messages de bienvenue non envoyés
- Vérifier que le surnom ET le nom d'utilisateur sont définis
- Vérifier l'implémentation de isRegistered()
- S'assurer que les messages de bienvenue sont envoyés dans execute()

### Problème: USER avant NICK n'enregistre pas
- C'est correct! Il faut à la fois NICK et USER
- Vérifier la logique de complétion de l'enregistrement
- La bienvenue devrait se déclencher quand les deux sont définis

### Problème: Impossible d'analyser le nom réel avec des espaces
- S'assurer que MessageParser gère le paramètre trailing (après :)
- params[3] devrait contenir "John Doe" et non juste "John"

---

## Fichiers Associés
- `Server.hpp/cpp` - Infos du serveur pour les messages de bienvenue
- `Client.hpp/cpp` - setUsername(), isRegistered()
- `PassCommand.cpp` - Doit être envoyé en premier
- `NickCommand.cpp` - Également requis pour l'enregistrement
- `MessageParser.cpp` - Analyser le paramètre trailing

---

## Flux d'Enregistrement

### Séquence Complète
```
1. Client: PASS secretpass
   Server: (succès silencieux)
   
2. Client: NICK john
   Server: (succès silencieux)
   
3. Client: USER john 0 * :John Doe
   Server: 001 RPL_WELCOME
   Server: 002 RPL_YOURHOST
   Server: 003 RPL_CREATED
   Server: 004 RPL_MYINFO
   [Client maintenant enregistré!]
```

### État Après Chaque Commande
```
Après PASS:
  authenticated: true
  nickname: ""
  username: ""
  registered: false

Après NICK:
  authenticated: true
  nickname: "john"
  username: ""
  registered: false

Après USER:
  authenticated: true
  nickname: "john"
  username: "john"
  registered: true ← Messages de bienvenue envoyés!
```

---

## Méthodes Client Supplémentaires Nécessaires

### Dans Client.hpp/cpp
```cpp
// Définir le nom d'utilisateur et vérifier l'enregistrement
void setUsername(const std::string& user) {
    username = user;
    // Vérifier si nous sommes maintenant entièrement enregistrés
    if (!nickname.empty() && !username.empty() && authenticated) {
        registered = true;
    }
}

// Optionnel: Stocker le nom réel
void setRealname(const std::string& real);
std::string getRealname() const;
```

---

## Détails des Messages de Bienvenue

### RPL_WELCOME (001)
```
:irc.server 001 nickname :Welcome to the IRC Network nickname!username@hostname
```
- Inclure nickname!username@hostname
- C'est l'identité IRC complète du client

### RPL_YOURHOST (002)
```
:irc.server 002 nickname :Your host is irc.server, running version 1.0
```
- Nom et version du serveur
- Peut être codé en dur pour ce projet

### RPL_CREATED (003)
```
:irc.server 003 nickname :This server was created Jan 12 2026
```
- Date de création du serveur
- Peut être l'heure de démarrage du serveur

### RPL_MYINFO (004)
```
:irc.server 004 nickname irc.server 1.0 o itkol
```
- Format: `<server> <version> <usermodes> <channelmodes>`
- Usermodes: "o" (opérateur)
- Channelmodes: "itkol" (invite, topic, key, operator, limit)

---

## Paramètre Mode (Optionnel)

### Valeurs de Mode Utilisateur
- **0**: Par défaut
- **4**: Wallops
- **8**: Invisible

Pour ce projet, le paramètre mode peut être ignoré ou stocké pour un usage futur.

---

## Cas Spéciaux

### Nom Réel avec des Deux-Points
```
USER john 0 * :John: The Great
```
- Tout après le premier `:` est le nom réel
- "John: The Great" incluant les deux-points

### Restrictions sur le Nom d'Utilisateur
- Pas d'espaces autorisés
- Typiquement en minuscules
- Peut contenir underscore, tiret
- Longueur max varie (souvent 8-10 caractères)

### Enregistrement Sans NICK
- USER sans NICK préalable
- Stocker le nom d'utilisateur, mais ne pas envoyer la bienvenue
- Attendre la commande NICK
- Envoyer la bienvenue quand les deux sont présents
