# Classe NickCommand - Description de la Tâche

## Vue d'ensemble
La classe `NickCommand` gère la commande NICK pour définir ou changer le pseudonyme d'un client. C'est une étape obligatoire dans l'enregistrement du client.

## Emplacement de la Classe
- **En-tête** : `includes/NickCommand.hpp`
- **Implémentation** : `src/NickCommand.cpp`

## Commande IRC
**Syntaxe** : `NICK <pseudonyme>`

---

## Méthodes

### Constructeur
```cpp
NickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande NICK

**TODO** :
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Définir ou changer le pseudonyme du client

**TODO** :

#### Étape 1 : Vérifier l'Authentification
- [x] Vérifier que le client est authentifié (a passé PASS)
- [x] Si non authentifié : envoyer ERR_PASSWDMISMATCH (464) ou déconnecter
  ```cpp
  if (!client->isAuthenticated()) {
      sendError(464, ":You must send PASS first");
      return;
  }
  ```

#### Étape 2 : Valider les Paramètres
- [x] Vérifier si le vecteur params a au moins 1 élément
- [x] Si non : envoyer ERR_NONICKNAMEGIVEN (431)
  ```cpp
  sendError(431, ":No nickname given");
  return;
  ```

#### Étape 3 : Valider le Format du Pseudonyme
- [x] Vérifier que le pseudonyme respecte les exigences IRC :
  - Doit commencer par une lettre (a-z, A-Z)
  - Peut contenir des lettres, chiffres, caractères spéciaux : - [ ] \ ` ^ { }
  - Maximum 9 caractères
- [x] Si invalide : envoyer ERR_ERRONEUSNICKNAME (432)
  ```cpp
  sendError(432, params[0] + " :Erroneous nickname");
  return;
  ```

#### Étape 4 : Vérifier la Disponibilité du Pseudonyme
- [x] Appeler server->getClientByNick(params[0])
- [x] Vérifier si le client retourné est différent du client actuel
- [x] Si le pseudonyme est déjà utilisé : envoyer ERR_NICKNAMEINUSE (433)
  ```cpp
  sendError(433, params[0] + " :Nickname is already in use");
  return;
  ```

#### Étape 5 : Définir le Pseudonyme
- [x] Stocker l'ancien pseudonyme si le client en avait un
- [x] Appeler client->setNickname(params[0])
- [x] Si le client est déjà enregistré (changement de pseudo) :
  - Diffuser le changement NICK à tous les canaux
  - Format : ":oldnick!user@host NICK :newnick"
- [x] Si cela complète l'enregistrement (a aussi le nom d'utilisateur) :
  - Envoyer les messages de bienvenue (001-004)

#### Étape 6 : Complétion de l'Enregistrement
- [x] Vérifier si le client est maintenant complètement enregistré
- [x] Si oui, envoyer la séquence de bienvenue :
  ```
  001 RPL_WELCOME: :Welcome to the IRC Network <nick>!<user>@<host>
  002 RPL_YOURHOST: :Your host is <servername>, running version <ver>
  003 RPL_CREATED: :This server was created <datetime>
  004 RPL_MYINFO: <servername> <version> <usermodes> <channelmodes>
  ```

---

## Validation du Pseudonyme

### Règles de Pseudonyme Valide (RFC 1459)
- **Premier caractère** : Lettre (a-z, A-Z)
- **Autres caractères** : Lettres, chiffres, spéciaux : `-[]\\`^{}`
- **Longueur** : 1-9 caractères
- **Casse** : Comparaison insensible à la casse

### Fonction de Validation
```cpp
bool isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9)
        return false;
    
    // Le premier caractère doit être une lettre
    if (!isalpha(nick[0]))
        return false;
    
    // Autres caractères : lettres, chiffres, caractères spéciaux
    for (size_t i = 1; i < nick.length(); i++) {
        char c = nick[i];
        if (!isalnum(c) && c != '-' && c != '[' && c != ']' && 
            c != '\\' && c != '`' && c != '^' && c != '{' && c != '}')
            return false;
    }
    
    return true;
}
```

---

## Priorité d'Implémentation

### Phase 1 - Implémentation de Base
1. Validation des paramètres (pseudonyme manquant)
2. Validation simple du format du pseudonyme
3. Définir le pseudonyme sur le client

### Phase 2 - Détection des Collisions
4. Vérifier la disponibilité du pseudonyme
5. Gérer ERR_NICKNAMEINUSE

### Phase 3 - Enregistrement
6. Vérifier le statut d'authentification
7. Compléter l'enregistrement si prêt
8. Envoyer les messages de bienvenue

### Phase 4 - Changements de Pseudonyme
9. Diffuser les changements NICK aux canaux
10. Gérer les clients déjà enregistrés

### Phase 5 - Validation Avancée
11. Validation complète du pseudonyme RFC 1459
12. Comparaison insensible à la casse

---

## Liste de Vérification des Tests

- [x] NICK avec un pseudonyme valide définit le pseudonyme du client
- [x] NICK sans paramètre envoie l'erreur 431
- [x] NICK avec un format invalide envoie l'erreur 432
- [x] NICK avec un pseudonyme déjà pris envoie l'erreur 433
- [x] NICK avant PASS est rejeté
- [x] NICK + USER complète l'enregistrement
- [x] Le changement NICK est diffusé aux canaux
- [x] Comparaison de pseudonyme insensible à la casse
- [x] Maximum de 9 caractères appliqué
- [x] Caractères spéciaux gérés correctement

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **431 ERR_NONICKNAMEGIVEN** : ":No nickname given"
- **432 ERR_ERRONEUSNICKNAME** : "<nick> :Erroneous nickname"
- **433 ERR_NICKNAMEINUSE** : "<nick> :Nickname is already in use"

### Messages de Bienvenue (lors de l'enregistrement)
- **001 RPL_WELCOME** : ":Welcome to the IRC Network <nick>!<user>@<host>"
- **002 RPL_YOURHOST** : ":Your host is <server>, running version <ver>"
- **003 RPL_CREATED** : ":This server was created <date>"
- **004 RPL_MYINFO** : "<server> <ver> <usermodes> <chanmodes>"

### Exemple d'Utilisation
```
Client: NICK john
Server: (pas de réponse - succès silencieux si valide)

Client: NICK john
Server: :irc.server 433 * john :Nickname is already in use

Client: NICK 123john
Server: :irc.server 432 * 123john :Erroneous nickname

Client: NICK
Server: :irc.server 431 * :No nickname given
```

### Changement de Pseudonyme (déjà enregistré)
```
Client: NICK newname
Server: :oldname!user@host NICK :newname
(Diffusé à tous les canaux dans lesquels le client se trouve)
```

---

## Problèmes Courants

### Problème : ERR_NICKNAMEINUSE pour un pseudo différent
- Assurer une comparaison insensible à la casse
- Vérifier l'implémentation de getClientByNick
- Vérifier la logique de comparaison des pseudonymes

### Problème : Messages de bienvenue non envoyés
- S'assurer que le pseudonyme ET le nom d'utilisateur sont définis
- Vérifier l'implémentation de isRegistered()
- Vérifier le statut d'authentification

### Problème : Le changement NICK n'est pas visible dans les canaux
- Implémenter la diffusion à tous les canaux du client
- Utiliser le format de message NICK approprié
- Inclure l'ancien pseudonyme dans le message

---

## Fichiers Liés
- `Server.hpp/cpp` - getClientByNick()
- `Client.hpp/cpp` - setNickname(), isRegistered()
- `PassCommand.cpp` - Authentification requise en premier
- `UserCommand.cpp` - Complète l'enregistrement
- `Channel.hpp/cpp` - Diffuser les changements NICK

---

## Méthodes Supplémentaires Nécessaires

### Dans Client.hpp/cpp
```cpp
// Obtenir les canaux du client pour la diffusion
const std::set<Channel*>& getChannels() const;
```

### Dans Server.hpp/cpp
```cpp
// Recherche de pseudonyme insensible à la casse
Client* getClientByNick(const std::string& nick);
```

---

## États d'Enregistrement

### Avant NICK
```
Authenticated: true
Has nickname: false
Has username: false
Registered: false
```

### Après NICK (pas encore USER)
```
Authenticated: true
Has nickname: true
Has username: false
Registered: false
```

### Après NICK + USER
```
Authenticated: true
Has nickname: true
Has username: true
Registered: true
→ Envoyer les messages de bienvenue !
```

---

## Cas Particuliers

### Sensibilité à la Casse
- Les pseudonymes sont insensibles à la casse pour la comparaison
- Stocker le pseudonyme tel que donné (préserver la casse)
- Comparer en utilisant une fonction insensible à la casse

### Longueur du Pseudonyme
- IRC RFC 1459 : Maximum 9 caractères
- IRC moderne : Permet souvent plus long (jusqu'à 30)
- Pour ce projet : Appliquer la limite de 9 caractères

### Caractères Spéciaux
- Autorisés : `-[]\\`^{}`
- Non autorisés : Espace, virgule, caractères de contrôle
- Ne pas autoriser : `!@#$%&*()`

### Pseudonymes Réservés
- Considérer la réservation de "Server", "Admin", etc.
- Optionnel : Empêcher les pseudonymes commençant par un chiffre
- Optionnel : Bloquer les mots offensants
