# Classe PartCommand - Description de la tâche

## Vue d'ensemble
La classe `PartCommand` gère la commande PART pour quitter les canaux IRC. Cela permet aux clients de sortir des canaux qu'ils ont rejoints.

## Emplacement de la classe
- **Header**: `includes/PartCommand.hpp`
- **Implémentation**: `src/PartCommand.cpp`

## Commande IRC
**Syntaxe**: `PART <channel>{,<channel>} [:<reason>]`

---

## Méthodes

### Constructeur
```cpp
PartCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande PART

**TODO**:
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Quitter un ou plusieurs canaux

**TODO**:

#### Étape 1: Vérifier l'enregistrement
- [x] Vérifier que le client est complètement enregistré
- [x] Sinon: envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2: Valider les paramètres
- [x] Vérifier que params contient au moins 1 élément (nom du canal)
- [x] Sinon: envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "PART :Not enough parameters");
  return;
  ```

#### Étape 3: Analyser les canaux
- [x] Diviser params[0] par virgules pour obtenir la liste des canaux
- [x] Exemple: "PART #foo,#bar"
  - Canaux: ["#foo", "#bar"]

#### Étape 4: Extraire la raison du départ (Optionnel)
- [x] Si params[1] existe, l'utiliser comme raison du départ
- [x] Exemple: "PART #general :Going to sleep"
- [x] La raison peut être vide (juste quitter)

#### Étape 5: Pour chaque canal
- [x] Obtenir le canal du serveur
- [x] Si le canal n'existe pas: envoyer ERR_NOSUCHCHANNEL (403)
  ```cpp
  Channel* channel = server->getChannel(channelName);
  if (!channel) {
      sendError(403, channelName + " :No such channel");
      continue;
  }
  ```

#### Étape 6: Vérifier l'appartenance
- [x] Vérifier si le client est membre du canal
- [x] Sinon: envoyer ERR_NOTONCHANNEL (442)
  ```cpp
  if (!channel->isMember(client)) {
      sendError(442, channelName + " :You're not on that channel");
      continue;
  }
  ```

#### Étape 7: Diffuser le message PART
- [x] Format: ":nick!user@host PART #channel :reason"
- [x] Diffuser à tous les membres (y compris le client qui part)
- [x] Inclure la raison si fournie
  ```cpp
  std::string partMsg = ":" + client->getNickname() + "!" + 
                        client->getUsername() + "@host PART " + 
                        channelName;
  if (!reason.empty())
      partMsg += " :" + reason;
  channel->broadcast(partMsg, NULL); // NULL = envoyer à tous
  ```

#### Étape 8: Retirer du canal
- [x] Appeler channel->removeMember(client)
- [x] Cela va:
  - Retirer de l'ensemble des membres
  - Retirer de l'ensemble des opérateurs si opérateur
  - Mettre à jour la liste des canaux du client

#### Étape 9: Supprimer le canal vide
- [x] Vérifier si le canal est maintenant vide
- [x] Si oui: le serveur doit supprimer le canal
- [x] Appeler server->deleteChannel(channelName)

---

## Priorité d'implémentation

### Phase 1 - Part basique
1. Vérification de l'enregistrement
2. Validation des paramètres
3. Vérification de l'existence du canal
4. Vérification de l'appartenance
5. Retirer du canal

### Phase 2 - Diffusion
6. Formater le message PART
7. Diffuser à tous les membres
8. Inclure le client qui part dans la diffusion

### Phase 3 - Raison du départ
9. Extraire la raison des params
10. Inclure la raison dans la diffusion

### Phase 4 - Canaux multiples
11. Analyser les canaux séparés par virgules
12. Quitter chaque canal indépendamment

### Phase 5 - Nettoyage
13. Vérifier si le canal est vide
14. Supprimer les canaux vides

---

## Liste de tests

- [x] PART #channel quitte le canal
- [x] PART sans paramètre envoie l'erreur 461
- [x] PART #nonexistent envoie l'erreur 403
- [x] PART #notmember envoie l'erreur 442
- [x] PART avant l'enregistrement envoie l'erreur 451
- [x] PART #foo,#bar quitte plusieurs canaux
- [x] PART #channel :Goodbye inclut la raison
- [x] PART diffuse à tous les membres
- [x] Les canaux vides sont supprimés
- [x] Le client est retiré des membres du canal
- [x] Le canal est retiré des canaux du client

---

## Notes sur le protocole IRC

### Codes d'erreur
- **403 ERR_NOSUCHCHANNEL**: "<channel> :No such channel"
- **442 ERR_NOTONCHANNEL**: "<channel> :You're not on that channel"
- **451 ERR_NOTREGISTERED**: ":You have not registered"
- **461 ERR_NEEDMOREPARAMS**: "<command> :Not enough parameters"

### Format du message PART
```
:nick!user@host PART #channel
:nick!user@host PART #channel :Part message here
```

### Exemples d'utilisation
```
Client: PART #general
Server: :john!john@host PART #general
(Diffusé à tous les membres y compris john)

Client: PART #general :Going to sleep
Server: :john!john@host PART #general :Going to sleep

Client: PART #foo,#bar
Server: :john!john@host PART #foo
Server: :john!john@host PART #bar

Client: PART #nonexistent
Server: :irc.server 403 john #nonexistent :No such channel

Client: PART #notmember
Server: :irc.server 442 john #notmember :You're not on that channel
```

---

## Canaux multiples

### Syntaxe
```
PART <channel1>,<channel2>,<channel3> [:<reason>]
```

### Exemples
```
PART #foo,#bar
PART #general,#test :Leaving both
```

### Comportement
- La même raison s'applique à tous les canaux
- Traiter chaque canal indépendamment
- Continuer même si un échoue
- Envoyer une erreur pour chaque canal échoué

---

## Problèmes courants

### Problème: PART n'est pas diffusé aux autres
- S'assurer que broadcast est appelé AVANT de retirer le membre
- Le client doit toujours être dans le canal pour recevoir la diffusion
- Ne pas exclure le client qui part de la diffusion PART

### Problème: Le client est toujours dans le canal après PART
- Vérifier que channel->removeMember(client) est appelé
- Vérifier que client->removeFromChannel(channel) est appelé
- S'assurer que les listes du client et du canal sont mises à jour

### Problème: Les canaux vides ne sont pas supprimés
- Implémenter server->deleteChannel()
- Vérifier le nombre de membres du canal après suppression
- Supprimer seulement si le compte atteint 0

---

## Fichiers associés
- `Server.hpp/cpp` - getChannel(), deleteChannel()
- `Client.hpp/cpp` - removeFromChannel()
- `Channel.hpp/cpp` - removeMember(), isMember()
- `JoinCommand.cpp` - Rejoindre des canaux
- `QuitCommand.cpp` - Nettoyage similaire lors de la déconnexion

---

## Méthodes serveur supplémentaires nécessaires

### Dans Server.hpp/cpp
```cpp
// Obtenir un canal existant (ne pas créer)
Channel* getChannel(const std::string& name);

// Supprimer le canal s'il est vide
void deleteChannel(const std::string& name);
```

### Dans Server.cpp
```cpp
Channel* Server::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end())
        return it->second;
    return NULL;
}

void Server::deleteChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end()) {
        if (it->second->getMemberCount() == 0) {
            delete it->second;
            channels.erase(it);
        }
    }
}
```

---

## PART vs QUIT

### PART
- Quitter des canaux spécifiques
- Le client reste connecté
- Reste dans les autres canaux
- Peut rejoindre plus tard

### QUIT
- Se déconnecter du serveur
- Quitte automatiquement tous les canaux
- Connexion fermée
- Doit se reconnecter pour rejoindre

---

## Cas particuliers

### Le dernier opérateur quitte
- Si le dernier opérateur part, le canal peut avoir besoin d'un nouvel opérateur
- Optionnel: Promouvoir un membre aléatoire en opérateur
- Ou: Permettre au canal d'exister sans opérateurs

### Longueur de la raison du départ
- Limiter la raison à une longueur raisonnable (ex: 200 caractères)
- Tronquer si trop long
- Une raison vide est valide

### Sensibilité à la casse
- Les noms de canaux sont insensibles à la casse
- #General, #general sont identiques
- Utiliser une casse cohérente pour les recherches

### Quitter tous les canaux
- Le client peut PART plusieurs canaux en même temps
- Traiter chacun indépendamment
- Ne pas s'arrêter à la première erreur

---

## Timing de la diffusion

### Important: Diffuser AVANT de retirer
```cpp
// ORDRE CORRECT:
1. Diffuser le message PART (le client est encore membre)
2. Retirer le client du canal
3. Supprimer le canal s'il est vide

// MAUVAIS ORDRE:
1. Retirer le client du canal  ← Le client ne peut pas recevoir la diffusion!
2. Diffuser le message PART
```

### Pourquoi?
- Le client doit recevoir son propre message PART
- Certains clients IRC se basent sur cela pour les mises à jour de l'interface
- Channel->broadcast() envoie seulement aux membres

---

## Gestion des canaux vides

### Quand supprimer
- Après le départ du dernier membre (PART)
- Après qu'un membre soit expulsé (KICK)
- Après la déconnexion d'un membre (QUIT)
- Après l'expiration d'un membre

### Comment vérifier
```cpp
if (channel->getMemberCount() == 0) {
    server->deleteChannel(channelName);
}
```

### Gestion de la mémoire
- Supprimer l'objet Channel
- Retirer de la map des canaux du serveur
- S'assurer qu'il n'y a pas de fuites mémoire
