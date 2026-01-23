# Classe InviteCommand - Description de la Tâche

## Vue d'ensemble
La classe `InviteCommand` gère la commande INVITE pour inviter des utilisateurs dans des canaux. Ceci est requis pour les canaux sur invitation uniquement (+i) et disponible pour tous les canaux.

## Emplacement de la Classe
- **En-tête** : `includes/InviteCommand.hpp`
- **Implémentation** : `src/InviteCommand.cpp`

## Commande IRC
**Syntaxe** : `INVITE <pseudo> <canal>`

---

## Méthodes

### Constructeur
```cpp
InviteCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande INVITE

**TODO** :
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Inviter un utilisateur dans un canal

**TODO** :

#### Étape 1 : Vérifier l'Enregistrement
- [x] Vérifier que le client est complètement enregistré
- [x] Sinon : envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2 : Valider les Paramètres
- [x] Vérifier si params a au moins 2 éléments (pseudo et canal)
- [x] Sinon : envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "INVITE :Not enough parameters");
  return;
  ```

#### Étape 3 : Extraire les Paramètres
- [x] Pseudo cible : params[0]
- [x] Nom du canal : params[1]

#### Étape 4 : Obtenir le Canal
- [x] Appeler server->getChannel(channelName)
- [x] Si le canal n'existe pas : envoyer ERR_NOSUCHCHANNEL (403)
  ```cpp
  sendError(403, channelName + " :No such channel");
  return;
  ```

#### Étape 5 : Vérifier que l'Inviteur est dans le Canal
- [x] Vérifier si le client (inviteur) est membre du canal
- [x] Sinon : envoyer ERR_NOTONCHANNEL (442)
  ```cpp
  sendError(442, channelName + " :You're not on that channel");
  return;
  ```

#### Étape 6 : Vérifier le Statut d'Opérateur (pour les canaux +i)
- [x] Si le canal est sur invitation uniquement (+i) :
  - Vérifier si le client est opérateur du canal
  - Sinon : envoyer ERR_CHANOPRIVSNEEDED (482)
  ```cpp
  if (channel->isInviteOnly() && !channel->isOperator(client)) {
      sendError(482, channelName + " :You're not channel operator");
      return;
  }
  ```

#### Étape 7 : Obtenir le Client Cible
- [x] Appeler server->getClientByNick(targetNick)
- [x] Si non trouvé : envoyer ERR_NOSUCHNICK (401)
  ```cpp
  sendError(401, targetNick + " :No such nick/channel");
  return;
  ```

#### Étape 8 : Vérifier que la Cible n'est pas Déjà dans le Canal
- [x] Vérifier si la cible est déjà membre du canal
- [x] Si oui : envoyer ERR_USERONCHANNEL (443)
  ```cpp
  sendError(443, targetNick + " " + channelName + 
            " :is already on channel");
  return;
  ```

#### Étape 9 : Ajouter à la Liste des Invités
- [x] Appeler channel->inviteUser(targetClient)
- [x] Cela ajoute la cible à l'ensemble des invités du canal

#### Étape 10 : Envoyer une Confirmation à l'Inviteur
- [x] Envoyer RPL_INVITING (341) à l'inviteur
  ```cpp
  sendReply(341, channelName + " " + targetNick);
  ```

#### Étape 11 : Envoyer INVITE à la Cible
- [x] Format : ":inviter!user@host INVITE target #channel"
- [x] Envoyer au client cible
  ```cpp
  std::string inviteMsg = ":" + client->getNickname() + "!" +
                          client->getUsername() + "@host INVITE " +
                          targetNick + " " + channelName + "\r\n";
  targetClient->sendMessage(inviteMsg);
  ```

---

## Priorité d'Implémentation

### Phase 1 - Validation de Base
1. Vérification de l'enregistrement
2. Validation des paramètres (pseudo et canal)
3. Vérification de l'existence du canal
4. Vérification de l'existence du client cible

### Phase 2 - Vérifications des Permissions
5. Vérifier que l'inviteur est dans le canal
6. Vérifier que l'inviteur est opérateur (si canal +i)
7. Vérifier que la cible n'est pas déjà dans le canal

### Phase 3 - Exécution de l'Invitation
8. Ajouter la cible à la liste des invités
9. Envoyer RPL_INVITING à l'inviteur
10. Envoyer le message INVITE à la cible

---

## Liste de Vérification des Tests

- [ ] INVITE user #channel invite avec succès
- [ ] INVITE sans paramètres envoie l'erreur 461
- [ ] INVITE vers un canal inexistant envoie l'erreur 403
- [ ] INVITE d'un utilisateur inexistant envoie l'erreur 401
- [ ] INVITE par un non-membre envoie l'erreur 442
- [ ] INVITE vers un canal +i par un non-op envoie l'erreur 482
- [ ] INVITE d'un utilisateur déjà dans le canal envoie l'erreur 443
- [ ] RPL_INVITING envoyé à l'inviteur
- [ ] Message INVITE envoyé à l'utilisateur invité
- [ ] L'utilisateur invité peut rejoindre un canal +i
- [ ] Invitation effacée après la jonction

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **401 ERR_NOSUCHNICK** : "<pseudo> :No such nick/channel"
- **403 ERR_NOSUCHCHANNEL** : "<canal> :No such channel"
- **442 ERR_NOTONCHANNEL** : "<canal> :You're not on that channel"
- **443 ERR_USERONCHANNEL** : "<utilisateur> <canal> :is already on channel"
- **451 ERR_NOTREGISTERED** : ":You have not registered"
- **461 ERR_NEEDMOREPARAMS** : "<commande> :Not enough parameters"
- **482 ERR_CHANOPRIVSNEEDED** : "<canal> :You're not channel operator"

### Codes de Succès
- **341 RPL_INVITING** : "<canal> <pseudo>"

### Formats de Message

#### RPL_INVITING (à l'inviteur)
```
:irc.server 341 inviter #channel invitednick
```

#### INVITE (à l'utilisateur invité)
```
:inviter!user@host INVITE invitednick #channel
```

### Exemples d'Utilisation

#### Invitation Réussie
```
Client: INVITE bob #general
Server: (au client)
        :irc.server 341 alice #general bob
Server: (à bob)
        :alice!alice@host INVITE bob #general
```

#### Erreurs
```
Client: INVITE bob #general
Server: :irc.server 443 alice bob #general :is already on channel

Client: INVITE bob #private
Server: :irc.server 482 alice #private :You're not channel operator

Client: INVITE nonexistent #general
Server: :irc.server 401 alice nonexistent :No such nick/channel
```

---

## Comportement des Invitations

### Canaux sur Invitation Uniquement (+i)
- Seuls les opérateurs peuvent inviter
- Les membres réguliers ne peuvent pas inviter
- Les utilisateurs invités contournent la restriction +i

### Canaux Réguliers (pas +i)
- N'importe quel membre peut inviter
- Statut d'opérateur non requis
- L'invitation est une notification de courtoisie

---

## Problèmes Courants

### Problème : L'utilisateur invité ne peut pas rejoindre
- Vérifier que l'invitation est ajoutée à l'ensemble des invités du canal
- Vérifier que la commande JOIN vérifie le statut d'invité
- Effacer l'invitation après une jonction réussie

### Problème : Les membres réguliers ne peuvent pas inviter
- Vérifier si le canal est sur invitation uniquement
- Exiger l'opérateur seulement pour les canaux +i
- Permettre à n'importe quel membre pour les canaux réguliers

### Problème : RPL_INVITING non envoyé
- S'assurer que sendReply est appelé avant INVITE
- Vérifier le code de réponse (341)
- Format : "canal pseudo" (séparés par un espace)

---

## Fichiers Associés
- `Server.hpp/cpp` - getChannel(), getClientByNick()
- `Client.hpp/cpp` - sendMessage()
- `Channel.hpp/cpp` - inviteUser(), isInvited(), isInviteOnly()
- `JoinCommand.cpp` - Vérifier le statut d'invité
- `ModeCommand.cpp` - Définir le mode +i

---

## Gestion de la Liste des Invitations

### Ajout à la Liste des Invitations
```cpp
void Channel::inviteUser(Client* client) {
    invited.insert(client);
}
```

### Vérification du Statut d'Invitation
```cpp
bool Channel::isInvited(Client* client) const {
    return invited.find(client) != invited.end();
}
```

### Effacement de l'Invitation (après jonction)
```cpp
void Channel::clearInvite(Client* client) {
    invited.erase(client);
}
```

---

## Invitation à Usage Unique

### Utilisation de l'Invitation
- L'invitation est à usage unique
- Effacée après un JOIN réussi
- L'utilisateur doit être réinvité s'il quitte

### Implémentation
```cpp
// Dans JoinCommand::execute()
if (channel->isInviteOnly()) {
    if (!channel->isInvited(client)) {
        sendError(473, channelName + " :Cannot join channel (+i)");
        return;
    }
    // Effacer l'invitation après une jonction réussie
    channel->clearInvite(client);
}
```

---

## Matrice de Permissions

### Qui Peut Inviter ?

| Mode du Canal | Type de Membre | Peut Inviter ? |
|---------------|----------------|----------------|
| Régulier      | Opérateur      | Oui            |
| Régulier      | Membre         | Oui            |
| +i            | Opérateur      | Oui            |
| +i            | Membre         | Non            |

---

## Cas Spéciaux

### Invitation vers un Canal Plein (+l)
- L'invitation ne contourne pas la limite d'utilisateurs
- L'utilisateur invité ne peut toujours pas rejoindre si c'est plein
- Comportement standard IRC
- Optionnel : Permettre aux utilisateurs invités de contourner la limite

### Invitation vers un Canal avec Clé (+k)
- L'invitation ne contourne pas l'exigence de clé
- L'utilisateur invité a toujours besoin de la clé
- Comportement standard IRC
- Optionnel : Permettre aux utilisateurs invités de contourner la clé

### Auto-Invitation
```
INVITE own_nickname #channel
```
- Sans intérêt (déjà dans le canal pour inviter)
- Retourner ERR_USERONCHANNEL
- Ou ignorer silencieusement

---

## Invitations Multiples

### Standard IRC
- Un utilisateur par commande INVITE
- Pas de pseudos séparés par des virgules
- Pour inviter plusieurs : envoyer plusieurs commandes INVITE

### Exemple
```
INVITE bob #channel
INVITE alice #channel
INVITE charlie #channel
```

---

## Expiration des Invitations

### Invitations Persistantes
- Les invitations n'expirent pas
- Restent jusqu'à utilisation (JOIN)
- Restent jusqu'à suppression du canal
- Restent même si l'inviteur part

### Optionnel : Expiration Temporelle
- Pas dans le standard IRC
- Peut implémenter un timeout (optionnel)
- Effacer après N minutes/heures
- Pour ce projet : Pas d'expiration nécessaire

---

## Notification Client-à-Client

### IRC Moderne
- Le client voit le message INVITE
- L'interface utilisateur du client affiche une notification
- L'utilisateur peut cliquer pour rejoindre

### Exemple d'Affichage Client
```
*** Alice (alice@host) vous a invité dans #general
```

---

## Scénarios de Test

### Fonctionnalité de Base
1. Un opérateur invite dans un canal +i
2. Un membre invite dans un canal régulier
3. Un utilisateur invité rejoint le canal
4. L'invitation est effacée après la jonction

### Tests de Permissions
5. Un non-membre essaie d'inviter
6. Un non-opérateur essaie d'inviter dans un canal +i
7. Un opérateur invite dans un canal régulier

### Cas d'Erreur
8. Inviter un utilisateur inexistant
9. Inviter vers un canal inexistant
10. Inviter un utilisateur déjà dans le canal
11. Inviter sans paramètres

### Cas Limites
12. Inviter vers un canal juste avant sa suppression
13. Invitations multiples pour le même utilisateur
14. Inviter puis kicker immédiatement (avant la jonction)
15. L'utilisateur invité se déconnecte avant de rejoindre

---

## Considérations de Sécurité

### Flooding d'Invitations
- Les invitations rapides peuvent spammer les utilisateurs
- Considérer la limitation de débit (optionnel)
- Enregistrer les comportements suspects

### Invitations Non Désirées
- Les utilisateurs ne peuvent pas bloquer les invitations (dans l'IRC de base)
- Optionnel : Implémenter une liste d'ignorance d'invitations
- Non requis pour ce projet

---

## Intégration avec JOIN

### Dans JoinCommand
```cpp
// Vérifier le mode sur invitation uniquement
if (channel->isInviteOnly()) {
    if (!channel->isInvited(client)) {
        sendError(473, channelName + " :Cannot join channel (+i)");
        return;
    }
}

// Après une jonction réussie, effacer l'invitation
if (channel->isInvited(client)) {
    channel->clearInvite(client);
}
```
