# Classe Channel - Description des Tâches

## Vue d'ensemble
La classe `Channel` représente un canal IRC. Elle gère les membres du canal, les opérateurs, le sujet, les modes, et gère la diffusion des messages au sein du canal.

## Emplacement de la classe
- **En-tête** : `includes/Channel.hpp`
- **Implémentation** : `src/Channel.cpp`

---

## Attributs privés

### Informations de base
- `std::string name` - Nom du canal (ex : #general)
- `std::string topic` - Sujet du canal
- `std::string key` - Mot de passe du canal (optionnel)

### Membres et opérateurs
- `std::set<Client*> members` - Ensemble de tous les membres du canal
- `std::set<Client*> operators` - Ensemble des opérateurs du canal
- `std::set<Client*> invited` - Ensemble des utilisateurs invités (pour le mode +i)

### Modes du canal
- `bool inviteOnly` - Mode +i : canal sur invitation uniquement
- `bool topicRestricted` - Mode +t : seuls les opérateurs peuvent modifier le sujet
- `int userLimit` - Mode +l : nombre maximum d'utilisateurs (0 = aucune limite)

---

## Méthodes

### Constructeur
```cpp
Channel(const std::string& name)
```

**Objectif** : Créer un canal avec le nom donné

**TODO** :
- [ ] Stocker le nom dans la variable membre
- [ ] Initialiser le sujet à une chaîne vide
- [ ] Initialiser la clé à une chaîne vide
- [ ] Initialiser l'ensemble des membres vide
- [ ] Initialiser l'ensemble des opérateurs vide
- [ ] Initialiser l'ensemble des invités vide
- [ ] Définir inviteOnly à false
- [ ] Définir topicRestricted à true (comportement IRC par défaut)
- [ ] Définir userLimit à 0 (aucune limite)

---

### Destructeur
```cpp
~Channel()
```

**Objectif** : Nettoyer les ressources du canal

**TODO** :
- [ ] Vider tous les ensembles (members, operators, invited)
- [ ] Note : Ne pas supprimer les objets Client (appartiennent au Server)
- [ ] Vider les chaînes topic et key

---

### getName()
```cpp
std::string getName() const
```

**Objectif** : Obtenir le nom du canal

**TODO** :
- [ ] Retourner la chaîne name
- [ ] Inclure le préfixe # dans le nom

---

### getTopic()
```cpp
std::string getTopic() const
```

**Objectif** : Obtenir le sujet du canal

**TODO** :
- [ ] Retourner la chaîne topic
- [ ] Retourner une chaîne vide si aucun sujet n'est défini

---

### setTopic(const std::string& topic, Client* client)
```cpp
void setTopic(const std::string& topic, Client* client)
```

**Objectif** : Définir le sujet du canal avec vérification des permissions

**TODO** :
- [ ] Vérifier si le mode topicRestricted est activé
- [ ] Si restreint : vérifier que le client est opérateur
- [ ] Si pas opérateur et restreint : envoyer l'erreur ERR_CHANOPRIVSNEEDED (482)
- [ ] Stocker le nouveau sujet
- [ ] Diffuser le changement de sujet à tous les membres
- [ ] Envoyer RPL_TOPIC (332) au canal

---

### setKey(const std::string& key)
```cpp
void setKey(const std::string& key)
```

**Objectif** : Définir le mot de passe du canal (mode +k)

**TODO** :
- [ ] Stocker la clé dans la variable membre
- [ ] Une clé vide signifie aucun mot de passe
- [ ] Appelé par la commande MODE +k

---

### hasKey()
```cpp
bool hasKey() const
```

**Objectif** : Vérifier si le canal a un mot de passe

**TODO** :
- [ ] Retourner true si la clé n'est pas vide
- [ ] Retourner false si la clé est vide

---

### checkKey(const std::string& key)
```cpp
bool checkKey(const std::string& key) const
```

**Objectif** : Vérifier que la clé fournie correspond à la clé du canal

**TODO** :
- [ ] Comparer le paramètre avec la clé stockée
- [ ] Retourner true si correspondance
- [ ] Retourner false si pas de correspondance
- [ ] Retourner true si aucune clé définie (pas de mot de passe)

---

### addMember(Client* client)
```cpp
void addMember(Client* client)
```

**Objectif** : Ajouter un client aux membres du canal

**TODO** :
- [ ] Vérifier que le client n'est pas NULL
- [ ] Insérer le client dans l'ensemble members
- [ ] Appeler client->addToChannel(this)
- [ ] Si c'est le premier membre : le faire opérateur
- [ ] Diffuser le message JOIN à tous les membres
- [ ] Envoyer les informations du canal au nouveau membre :
  - Sujet (RPL_TOPIC 332, RPL_NOTOPIC 331)
  - Liste des membres (RPL_NAMREPLY 353, RPL_ENDOFNAMES 366)

---

### removeMember(Client* client)
```cpp
void removeMember(Client* client)
```

**Objectif** : Retirer un client du canal

**TODO** :
- [ ] Vérifier si le client est membre
- [ ] Retirer de l'ensemble members
- [ ] Retirer de l'ensemble operators si opérateur
- [ ] Retirer de l'ensemble invited si invité
- [ ] Appeler client->removeFromChannel(this)
- [ ] Diffuser le message PART aux membres restants
- [ ] Si le canal devient vide : le Server doit supprimer le canal

---

### addOperator(Client* client)
```cpp
void addOperator(Client* client)
```

**Objectif** : Donner le statut d'opérateur à un client

**TODO** :
- [ ] Vérifier si le client est membre du canal
- [ ] Si pas membre : envoyer une erreur
- [ ] Insérer le client dans l'ensemble operators
- [ ] Diffuser le message MODE +o au canal

---

### removeOperator(Client* client)
```cpp
void removeOperator(Client* client)
```

**Objectif** : Retirer le statut d'opérateur à un client

**TODO** :
- [ ] Vérifier si le client est opérateur
- [ ] Retirer de l'ensemble operators
- [ ] Diffuser le message MODE -o au canal
- [ ] S'assurer qu'au moins un opérateur reste

---

### isOperator(Client* client)
```cpp
bool isOperator(Client* client) const
```

**Objectif** : Vérifier si le client est opérateur du canal

**TODO** :
- [ ] Vérifier si le client existe dans l'ensemble operators
- [ ] Retourner true si trouvé
- [ ] Retourner false sinon

---

### isMember(Client* client)
```cpp
bool isMember(Client* client) const
```

**Objectif** : Vérifier si le client est membre du canal

**TODO** :
- [ ] Vérifier si le client existe dans l'ensemble members
- [ ] Retourner true si trouvé
- [ ] Retourner false sinon

---

### broadcast(const std::string& msg, Client* exclude)
```cpp
void broadcast(const std::string& msg, Client* exclude)
```

**Objectif** : Envoyer un message à tous les membres du canal sauf un

**TODO** :
- [ ] Itérer à travers l'ensemble members
- [ ] Pour chaque membre :
  - Ignorer si membre == exclude
  - Appeler member->sendMessage(msg)
- [ ] Gérer les erreurs d'envoi avec élégance
- [ ] Ne pas lever d'exceptions sur les échecs individuels

---

### setInviteOnly(bool mode)
```cpp
void setInviteOnly(bool mode)
```

**Objectif** : Définir le mode invitation uniquement (+i/-i)

**TODO** :
- [ ] Définir le flag inviteOnly au paramètre mode
- [ ] Si désactivation : vider l'ensemble invited
- [ ] Diffuser le message MODE au canal

---

### setTopicRestricted(bool mode)
```cpp
void setTopicRestricted(bool mode)
```

**Objectif** : Définir le mode de restriction du sujet (+t/-t)

**TODO** :
- [ ] Définir le flag topicRestricted au paramètre mode
- [ ] Diffuser le message MODE au canal

---

### setUserLimit(int limit)
```cpp
void setUserLimit(int limit)
```

**Objectif** : Définir la limite maximum d'utilisateurs (+l/-l)

**TODO** :
- [ ] Valider la limite (>= 0)
- [ ] Définir userLimit au paramètre limit
- [ ] 0 signifie aucune limite
- [ ] Diffuser le message MODE au canal

---

### inviteUser(Client* client)
```cpp
void inviteUser(Client* client)
```

**Objectif** : Inviter un utilisateur à un canal sur invitation uniquement

**TODO** :
- [ ] Vérifier si le canal est en mode invitation uniquement
- [ ] Ajouter le client à l'ensemble invited
- [ ] Envoyer RPL_INVITING (341) à celui qui invite
- [ ] Envoyer le message INVITE au client invité
- [ ] L'invitation est à usage unique (effacée après le join)

---

### isInvited(Client* client)
```cpp
bool isInvited(Client* client) const
```

**Objectif** : Vérifier si le client est invité

**TODO** :
- [ ] Vérifier si le client existe dans l'ensemble invited
- [ ] Retourner true si trouvé
- [ ] Retourner false sinon
- [ ] Utilisé quand un client essaie de JOIN un canal +i

---

### kickMember(Client* client, const std::string& reason)
```cpp
void kickMember(Client* client, const std::string& reason)
```

**Objectif** : Expulser un membre du canal

**TODO** :
- [ ] Vérifier si le client est membre
- [ ] Si pas membre : envoyer une erreur
- [ ] Retirer le membre du canal (appeler removeMember)
- [ ] Diffuser le message KICK avec la raison à tous les membres
- [ ] Inclure le client expulsé dans la diffusion

---

## Modes de canal (Standard IRC)

### Modes de base
- **+i** : Invitation uniquement (setInviteOnly)
- **+t** : Sujet restreint aux opérateurs (setTopicRestricted)
- **+k** : Clé/mot de passe du canal (setKey)
- **+l** : Limite d'utilisateurs (setUserLimit)
- **+o** : Opérateur du canal (addOperator/removeOperator)

---

## Priorité d'implémentation

### Phase 1 - Configuration de base
1. Constructeur/Destructeur
2. getName()
3. getTopic()

### Phase 2 - Gestion des membres
4. addMember() - version basique
5. removeMember() - version basique
6. isMember()
7. broadcast() - version basique

### Phase 3 - Gestion des opérateurs
8. addOperator()
9. removeOperator()
10. isOperator()

### Phase 4 - Gestion du sujet
11. setTopic() - avec vérification des permissions

### Phase 5 - Clé/Mot de passe
12. setKey()
13. hasKey()
14. checkKey()

### Phase 6 - Modes avancés
15. setInviteOnly()
16. inviteUser()
17. isInvited()
18. setTopicRestricted()
19. setUserLimit()

### Phase 7 - Modération
20. kickMember()

### Phase 8 - Améliorations
21. Ajouter les diffusions appropriées JOIN/PART/TOPIC/KICK
22. Ajouter l'application de la limite d'utilisateurs dans addMember()
23. Ajouter la vérification d'invitation dans addMember()

---

## Liste de vérification des tests

- [ ] Canal créé avec un nom valide
- [ ] Le premier membre devient opérateur automatiquement
- [ ] Les membres peuvent rejoindre et partir
- [ ] Les opérateurs peuvent modifier le sujet quand +t est activé
- [ ] Les non-opérateurs ne peuvent pas modifier le sujet quand +t est activé
- [ ] La clé du canal bloque les joins non autorisés
- [ ] Le mode invitation uniquement fonctionne correctement
- [ ] La limite d'utilisateurs empêche les joins excessifs
- [ ] Kick retire le membre correctement
- [ ] Broadcast envoie à tous les membres sauf celui exclu
- [ ] Gestion des canaux vides

---

## Notes du protocole IRC

### Nommage des canaux
- Doit commencer par # (ou & pour les canaux locaux)
- Pas d'espaces, virgules ou caractères de contrôle
- Comparaison insensible à la casse
- Maximum 50 caractères (RFC 1459)

### Réponses numériques
- RPL_NOTOPIC (331) : Aucun sujet défini
- RPL_TOPIC (332) : Contenu du sujet
- RPL_INVITING (341) : Invitation envoyée
- RPL_NAMREPLY (353) : Liste des noms
- RPL_ENDOFNAMES (366) : Fin des noms
- ERR_CHANOPRIVSNEEDED (482) : Pas opérateur du canal
- ERR_USERNOTINCHANNEL (441) : Utilisateur pas dans le canal
- ERR_NOTONCHANNEL (442) : Vous n'êtes pas sur le canal

---

## Fichiers associés
- `Server.hpp/cpp` - Crée et gère les canaux
- `Client.hpp/cpp` - Membres du canal
- `JoinCommand.cpp` - Implémentation de JOIN
- `PartCommand.cpp` - Implémentation de PART
- `ModeCommand.cpp` - Implémentation de MODE
- `KickCommand.cpp` - Implémentation de KICK
- `TopicCommand.cpp` - Implémentation de TOPIC
- `InviteCommand.cpp` - Implémentation de INVITE
