# Classe KickCommand - Description de la Tâche

## Aperçu
La classe `KickCommand` gère la commande KICK pour retirer des utilisateurs des canaux. Il s'agit d'une fonctionnalité de modération disponible uniquement pour les opérateurs de canal.

## Emplacement de la Classe
- **En-tête** : `includes/KickCommand.hpp`
- **Implémentation** : `src/KickCommand.cpp`

## Commande IRC
**Syntaxe** : `KICK <canal> <utilisateur> [:<raison>]`

---

## Méthodes

### Constructeur
```cpp
KickCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande KICK

**TODO** :
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Expulser un utilisateur d'un canal

**TODO** :

#### Étape 1 : Vérifier l'Enregistrement
- [x] Vérifier que le client est complètement enregistré
- [x] Sinon : envoyer ERR_NOTREGISTERED (451)
  ```cpp
  #include "Replies.hpp"
  
  if (!client->isRegistered()) {
      client->sendMessage(ERR_NOTREGISTERED(client->getNickname()) + "\r\n");
      return;
  }
  ```
  > **Note**: ERR_NOTREGISTERED n'existe pas dans Replies.hpp standard.
  > Ajouter si besoin : `#define ERR_NOTREGISTERED(nick) (":" SERVER_NAME " 451 " + (nick) + " :You have not registered")`

#### Étape 2 : Valider les Paramètres
- [x] Vérifier si params a au moins 2 éléments (canal et utilisateur)
- [x] Sinon : envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK") + "\r\n");
  return;
  ```

#### Étape 3 : Extraire les Paramètres
- [x] Nom du canal : params[0]
- [x] Pseudo cible : params[1]
- [x] Raison de l'expulsion : params[2] (optionnel, par défaut : pseudo expulsé)

#### Étape 4 : Obtenir le Canal
- [x] Appeler server->getChannel(channelName)
- [x] Si le canal n'existe pas : envoyer ERR_NOSUCHCHANNEL (403)
  ```cpp
  client->sendMessage(ERR_NOSUCHCHANNEL(client->getNickname(), channelName) + "\r\n");
  return;
  ```

#### Étape 5 : Vérifier que l'Expulseur est dans le Canal
- [x] Vérifier si le client (expulseur) est membre du canal
- [x] Sinon : envoyer ERR_NOTONCHANNEL (442)
  ```cpp
  client->sendMessage(ERR_NOTONCHANNEL(client->getNickname(), channelName) + "\r\n");
  return;
  ```

#### Étape 6 : Vérifier que l'Expulseur est Opérateur
- [x] Vérifier si le client est opérateur du canal
- [x] Sinon : envoyer ERR_CHANOPRIVSNEEDED (482)
  ```cpp
  client->sendMessage(ERR_CHANOPRIVSNEEDED(client->getNickname(), channelName) + "\r\n");
  return;
  ```

#### Étape 7 : Obtenir le Client Cible
- [x] Appeler server->getClientByNick(targetNick)
- [x] Si non trouvé : envoyer ERR_NOSUCHNICK (401)
  ```cpp
  client->sendMessage(ERR_NOSUCHNICK(client->getNickname(), targetNick) + "\r\n");
  return;
  ```

#### Étape 8 : Vérifier que la Cible est dans le Canal
- [x] Vérifier si la cible est membre du canal
- [x] Sinon : envoyer ERR_USERNOTINCHANNEL (441)
  ```cpp
  client->sendMessage(ERR_USERNOTINCHANNEL(client->getNickname(), targetNick, channelName) + "\r\n");
  return;
  ```

#### Étape 9 : Diffuser le Message KICK
- [x] Format : ":expulseur!user@host KICK #canal cible :raison"
- [x] Diffuser à TOUS les membres (y compris l'expulseur et l'expulsé)
  ```cpp
  std::string kickMsg = USER_PREFIX(client->getNickname(), 
                                    client->getUsername(), "host") +
                        " KICK " + channelName + " " + targetNick + 
                        " :" + reason + "\r\n";
  channel->broadcast(kickMsg, NULL); // NULL = envoyer à tous
  ```

#### Étape 10 : Retirer la Cible du Canal
- [x] Appeler channel->removeMember(targetClient)
- [x] Mettre à jour la liste des canaux de la cible

#### Étape 11 : Supprimer le Canal Vide
- [x] Vérifier si le canal est maintenant vide
- [x] Si vide : supprimer le canal

---

## Priorité d'Implémentation

### Phase 1 - Validation de Base
1. Vérification de l'enregistrement
2. Validation des paramètres (canal et utilisateur)
3. Vérification de l'existence du canal

### Phase 2 - Vérifications des Permissions
4. Vérifier que l'expulseur est dans le canal
5. Vérifier que l'expulseur est opérateur
6. Vérifier que la cible existe
7. Vérifier que la cible est dans le canal

### Phase 3 - Exécution du Kick
8. Formater le message KICK
9. Diffuser à tous les membres
10. Retirer la cible du canal

### Phase 4 - Raison Optionnelle
11. Extraire la raison de l'expulsion des params
12. Utiliser une raison par défaut si non fournie

### Phase 5 - Nettoyage
13. Supprimer les canaux vides

---

## Liste de Vérification des Tests

- [x] KICK #canal utilisateur expulse avec succès
- [x] KICK #canal utilisateur :raison inclut la raison
- [x] KICK sans paramètres envoie l'erreur 461
- [x] KICK #inexistant utilisateur envoie l'erreur 403
- [x] KICK par un non-membre envoie l'erreur 442
- [x] KICK par un non-opérateur envoie l'erreur 482
- [x] KICK utilisateur inexistant envoie l'erreur 401
- [x] KICK utilisateur pas dans le canal envoie l'erreur 441
- [x] KICK diffuse à tous les membres
- [x] L'utilisateur expulsé est retiré du canal
- [x] Le canal vide est supprimé après le kick
- [x] L'utilisateur expulsé peut rejoindre (pas banni)

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **401 ERR_NOSUCHNICK** : "<pseudo> :No such nick/channel"
- **403 ERR_NOSUCHCHANNEL** : "<canal> :No such channel"
- **441 ERR_USERNOTINCHANNEL** : "<pseudo> <canal> :They aren't on that channel"
- **442 ERR_NOTONCHANNEL** : "<canal> :You're not on that channel"
- **451 ERR_NOTREGISTERED** : ":You have not registered"
- **461 ERR_NEEDMOREPARAMS** : "<commande> :Not enough parameters"
- **482 ERR_CHANOPRIVSNEEDED** : "<canal> :You're not channel operator"

### Format du Message KICK
```
:expulseur!user@host KICK #canal cible :raison
```

### Exemples d'Utilisation

#### Kick Réussi
```
Client: KICK #general baduser :Spamming
Serveur: (à tous les membres)
        :alice!alice@host KICK #general baduser :Spamming
```

#### Sans Raison
```
Client: KICK #general baduser
Serveur: :alice!alice@host KICK #general baduser :baduser
```

#### Erreurs
```
Client: KICK #general baduser
Serveur: :irc.server 482 alice #general :You're not channel operator

Client: KICK #nonexistent baduser
Serveur: :irc.server 403 alice #nonexistent :No such channel

Client: KICK #general nonexistent
Serveur: :irc.server 401 alice nonexistent :No such nick/channel

Client: KICK #general gooduser
Serveur: :irc.server 441 alice gooduser #general :They aren't on that channel
```

---

## Hiérarchie des Permissions

### Qui Peut Expulser ?
1. **Opérateurs de Canal** : Peuvent expulser n'importe qui (sauf les autres opérateurs dans certaines implémentations)
2. **Membres Réguliers** : Ne peuvent pas expulser
3. **Non-Membres** : Ne peuvent pas expulser

### Les Opérateurs Peuvent-ils Expulser d'Autres Opérateurs ?
- IRC traditionnel : Oui
- Certains réseaux : Non (rang égal)
- Pour ce projet : Autoriser les opérateurs à expulser les opérateurs

---

## Problèmes Courants

### Problème : KICK pas diffusé à tous
- S'assurer que la diffusion envoie à TOUS les membres
- Ne pas exclure l'expulseur ou l'utilisateur expulsé
- Les deux doivent voir le message KICK

### Problème : L'utilisateur expulsé reste dans le canal
- Vérifier que channel->removeMember() est appelé
- Vérifier que le membre est effectivement retiré
- Mettre à jour les listes du canal et du client

### Problème : L'opérateur ne peut pas expulser
- Vérifier la vérification channel->isOperator(client)
- S'assurer que l'ensemble des opérateurs est maintenu correctement
- Vérifier que le statut d'opérateur est défini lors du JOIN

---

## Fichiers Associés
- `Server.hpp/cpp` - getChannel(), getClientByNick()
- `Client.hpp/cpp` - Appartenance aux canaux
- `Channel.hpp/cpp` - isOperator(), isMember(), removeMember()
- `ModeCommand.cpp` - Définir le statut d'opérateur (+o)
- `JoinCommand.cpp` - Le premier membre devient opérateur

---

## Raison d'Expulsion par Défaut

### Options
- Utiliser le pseudo de la cible
- "Aucune raison donnée"
- Pseudo de l'expulseur
- Chaîne vide

### Bonne Pratique
```cpp
std::string reason = (params.size() >= 3) ? params[2] : targetNick;
```

---

## Ordre de Diffusion

### Important : Diffuser AVANT de Retirer
```cpp
// CORRECT :
1. Diffuser KICK à tous les membres (y compris l'utilisateur expulsé)
2. Retirer l'utilisateur expulsé du canal
3. Supprimer le canal s'il est vide

// FAUX :
1. Retirer l'utilisateur expulsé du canal  ← Ne peut pas recevoir le message KICK !
2. Diffuser KICK
```

---

## KICK vs BAN

### KICK
- Retire l'utilisateur immédiatement
- L'utilisateur peut rejoindre
- Retrait temporaire
- Aucune restriction permanente

### BAN (mode +b)
- Définit un masque de bannissement
- Empêche de rejoindre
- Permanent jusqu'au débannissement
- Fonctionnalité optionnelle (non requise)

---

## Cas Spéciaux

### S'Expulser Soi-Même
```
KICK #canal votre_propre_pseudo
```
- Certains serveurs autorisent (auto-expulsion)
- D'autres interdisent (utiliser PART à la place)
- Recommandation : Autoriser (sans danger)

### Expulser le Dernier Membre
- Si l'utilisateur expulsé est le dernier membre
- Le canal devient vide
- Supprimer le canal

### Expulsions Multiples
- IRC ne supporte pas les cibles séparées par des virgules
- Une commande KICK = un utilisateur
- Pour expulser plusieurs : envoyer plusieurs commandes KICK

---

## Vérifications des Opérateurs

### Vérifier le Statut d'Opérateur
```cpp
if (!channel->isOperator(client)) {
    sendError(482, channelName + " :You're not channel operator");
    return;
}
```

### Quand les Utilisateurs Sont-ils Opérateurs ?
- Premier membre à rejoindre le canal
- Promu par un opérateur existant (MODE +o)
- Jamais automatiquement par le serveur

---

## Après KICK

### Qu'Arrive-t-il à l'Utilisateur Expulsé ?
1. Reçoit le message KICK
2. Retiré du canal
3. Peut toujours voir le serveur
4. Peut rejoindre le canal (sauf si banni)
5. Peut rejoindre d'autres canaux

### Comportement du Client
- La plupart des clients IRC rejoignent automatiquement après expulsion
- Certains affichent le message d'expulsion
- Certains alertent l'utilisateur avec un son/notification

---

## Gestion des Canaux Vides

### Après Kick
```cpp
// Retirer du canal
channel->removeMember(targetClient);

// Vérifier si vide
if (channel->getMemberCount() == 0) {
    server->deleteChannel(channelName);
}
```

### Gestion de la Mémoire
- Supprimer l'objet Channel
- Retirer de la map du serveur
- Éviter les fuites de mémoire

---

## Détails des Messages d'Erreur

### ERR_CHANOPRIVSNEEDED (482)
```
:irc.server 482 pseudo #canal :You're not channel operator
```
- Envoyé quand un non-opérateur essaie d'expulser
- Envoyé quand un non-opérateur essaie MODE +o/+k/+l
- Erreur de permission courante

### ERR_USERNOTINCHANNEL (441)
```
:irc.server 441 pseudo pseudocible #canal :They aren't on that channel
```
- La cible n'est pas dans le canal
- Impossible d'expulser quelqu'un qui n'est pas là
- Vérifier l'appartenance avant d'expulser

---

## Considérations de Sécurité

### Abus d'Opérateur
- Les opérateurs peuvent expulser n'importe qui
- Considérer la journalisation des expulsions
- Considérer des limites (optionnel)

### Inondation de Kicks
- Les expulsions rapides peuvent être perturbatrices
- Considérer une limitation de débit (optionnel)
- Journaliser les comportements suspects

### Boucles d'Expulsion
- Utilisateur expulsé, rejoint, expulsé à nouveau
- Pas un bug, problème d'opérateur
- L'opérateur devrait utiliser BAN (+b)

---

## Scénarios de Test

### Fonctionnalité de Base
1. L'opérateur expulse un utilisateur régulier
2. L'opérateur expulse avec une raison
3. L'opérateur expulse sans raison
4. Un non-opérateur essaie d'expulser

### Cas d'Erreur
5. Expulsion depuis un canal inexistant
6. Expulsion d'un utilisateur inexistant
7. Expulsion d'un utilisateur pas dans le canal
8. Expulsion quand on n'est pas soi-même dans le canal

### Cas Limites
9. S'expulser soi-même
10. Expulser le dernier membre (suppression du canal)
11. Plusieurs opérateurs expulsent le même utilisateur
12. Expulser un opérateur (si autorisé)
