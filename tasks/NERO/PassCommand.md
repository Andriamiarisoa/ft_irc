# Classe PassCommand - Description de la tâche

## Vue d'ensemble
La classe `PassCommand` gère la commande PASS pour l'authentification par mot de passe. C'est la première étape dans l'enregistrement du client et doit être complétée avant les commandes NICK et USER.

## Emplacement de la classe
- **En-tête**: `includes/PassCommand.hpp`
- **Implémentation**: `src/PassCommand.cpp`

## Commande IRC
**Syntaxe**: `PASS <password>`

---

## Méthodes

### Constructeur
```cpp
PassCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande PASS

**TODO**:
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Authentifier le client avec le mot de passe

**TODO**:

#### Étape 1: Valider les paramètres
- [x] Vérifier si le vecteur params a au moins 1 élément
- [x] Sinon: envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  #include "Replies.hpp"
  
  client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS") + "\r\n");
  return;
  ```

#### Étape 2: Vérifier l'état d'enregistrement
- [x] Vérifier si le client est déjà enregistré
- [x] Si oui: envoyer ERR_ALREADYREGISTRED (462)
  ```cpp
  if (client->isRegistered()) {
      client->sendMessage(ERR_ALREADYREGISTERED(client->getNickname()) + "\r\n");
      return;
  }
  ```

#### Étape 3: Vérifier le mot de passe
- [x] Obtenir le mot de passe du serveur (besoin d'ajouter un getter à la classe Server)
- [x] Comparer params[0] avec le mot de passe du serveur
- [x] Si correspondance:
  - Appeler client->authenticate()
  - Aucune réponse nécessaire (succès silencieux selon le standard IRC)
- [x] Si non-correspondance:
  - Envoyer ERR_PASSWDMISMATCH (464)
  ```cpp
  client->sendMessage(ERR_PASSWDMISMATCH(client->getNickname()) + "\r\n");
  ```

---

## Priorité d'implémentation

### Phase 1 - Implémentation de base
1. Validation des paramètres
2. Vérification de l'état d'enregistrement
3. Comparaison du mot de passe
4. Authentification du client

### Phase 2 - Amélioration
5. Ajouter le getter Server::getPassword()
6. Considérer la prévention des attaques par timing (comparaison en temps constant)
7. Ajouter une limitation du taux de tentatives de mot de passe

---

## Liste de vérification des tests

- [x] PASS avec le bon mot de passe authentifie le client
- [x] PASS avec le mauvais mot de passe envoie l'erreur 464
- [x] PASS sans paramètre envoie l'erreur 461
- [x] PASS après l'enregistrement envoie l'erreur 462
- [x] Plusieurs tentatives PASS avant l'enregistrement autorisées
- [x] Le client peut passer à NICK après un PASS réussi

---

## Notes sur le protocole IRC

### Spécification RFC 1459
- PASS doit être envoyé avant NICK/USER
- Certains serveurs autorisent PASS après NICK mais avant USER
- IRC moderne: ordre PASS → NICK → USER imposé
- Un PASS échoué ne déconnecte pas le client (peut réessayer)

### Codes d'erreur
- **461 ERR_NEEDMOREPARAMS**: "<command> :Not enough parameters"
- **462 ERR_ALREADYREGISTRED**: ":You may not reregister"
- **464 ERR_PASSWDMISMATCH**: ":Password incorrect"

### Exemple d'utilisation
```
Client: PASS secretpass
Server: (pas de réponse - succès silencieux)

Client: PASS wrongpass
Server: :irc.server 464 * :Password incorrect

Client: PASS
Server: :irc.server 461 * PASS :Not enough parameters
```

---

## Problèmes courants

### Problème: Client non authentifié
- Vérifier si le mot de passe correspond exactement (sensible à la casse)
- Vérifier que client->authenticate() est appelé
- S'assurer que isAuthenticated() retourne true après PASS

### Problème: Impossible d'envoyer PASS après NICK
- C'est le comportement correct
- PASS doit venir en premier dans la séquence d'enregistrement
- Le client doit se reconnecter pour réessayer

---

## Fichiers associés
- `Server.hpp/cpp` - Stocke le mot de passe du serveur
- `Client.hpp/cpp` - État d'authentification
- `NickCommand.cpp` - Étape suivante dans l'enregistrement
- `UserCommand.cpp` - Étape finale dans l'enregistrement

---

## Méthode serveur supplémentaire nécessaire

Ajouter à la classe Server:
```cpp
// Dans Server.hpp
public:
    const std::string& getPassword() const;

// Dans Server.cpp
const std::string& Server::getPassword() const {
    return password;
}
```

---

## Flux d'enregistrement

### Séquence d'enregistrement complète
1. **PASS**: S'authentifier avec le mot de passe
2. **NICK**: Définir le pseudonyme
3. **USER**: Définir le nom d'utilisateur
4. **Résultat**: Le client est entièrement enregistré, reçoit les messages de bienvenue

### Transitions d'état
```
Initial → [PASS succès] → Authentifié
Authentifié → [NICK] → A un pseudonyme
A un pseudonyme → [USER] → Entièrement enregistré
```

---

## Considérations de sécurité

### Gestion du mot de passe
- Comparer les mots de passe en temps constant (prévenir les attaques par timing)
- Ne pas enregistrer les mots de passe dans les logs
- Envisager de hacher le mot de passe du serveur (optionnel)
- Limiter le taux de tentatives échouées (optionnel)

### Meilleures pratiques
- Effacer le mot de passe de la mémoire après utilisation
- Ne pas envoyer le mot de passe dans les messages d'erreur
- Enregistrer les tentatives d'authentification échouées dans les logs
- Envisager une limitation basée sur l'adresse IP
