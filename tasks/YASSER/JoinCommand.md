# Classe JoinCommand - Description de Tâche

## Vue d'ensemble
La classe `JoinCommand` gère la commande JOIN pour rejoindre les canaux IRC. C'est ainsi que les clients entrent dans les canaux pour participer aux conversations de groupe.

## Emplacement de la Classe
- **En-tête**: `includes/JoinCommand.hpp`
- **Implémentation**: `src/JoinCommand.cpp`

## Commande IRC
**Syntaxe**: `JOIN <canal>{,<canal>} [<clé>{,<clé>}]`

---

## Méthodes

### Constructeur
```cpp
JoinCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande JOIN

**TODO**:
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Rejoindre un ou plusieurs canaux

**TODO**:

#### Étape 1: Vérifier l'Enregistrement
- [x] Vérifier que le client est entièrement enregistré
- [x] Si non: envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2: Valider les Paramètres
- [x] Vérifier si params a au moins 1 élément (nom du canal)
- [x] Si non: envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "JOIN :Not enough parameters");
  return;
  ```

#### Étape 3: Analyser les Canaux et les Clés
- [x] Diviser params[0] par des virgules pour obtenir la liste des canaux
- [x] Si params[1] existe, diviser par des virgules pour obtenir la liste des clés
- [x] Exemple: "JOIN #foo,#bar key1,key2"
  - Canaux: ["#foo", "#bar"]
  - Clés: ["key1", "key2"]

#### Étape 4: Pour Chaque Canal
- [x] Valider le format du nom du canal:
  - Doit commencer par # ou &
  - Pas d'espaces, virgules ou caractères de contrôle
  - Maximum 50 caractères
- [x] Si invalide: envoyer ERR_NOSUCHCHANNEL (403)

#### Étape 5: Obtenir ou Créer le Canal
- [x] Appeler server->getOrCreateChannel(channelName)
- [x] Si le canal est nouveau: le client devient le premier opérateur

#### Étape 6: Vérifier l'Accès au Canal
- [x] Si le canal a une clé (mode +k):
  - Vérifier si le client a fourni la bonne clé
  - Si mauvaise/manquante: envoyer ERR_BADCHANNELKEY (475)
- [x] Si le canal est sur invitation uniquement (mode +i):
  - Vérifier si le client est invité
  - Si non: envoyer ERR_INVITEONLYCHAN (473)
- [x] Si le canal a une limite d'utilisateurs (mode +l):
  - Vérifier si le canal est plein
  - Si plein: envoyer ERR_CHANNELISFULL (471)

#### Étape 7: Vérifier si Déjà Membre
- [x] Si le client est déjà dans le canal: ignorer (pas d'erreur)
- [x] IRC permet le re-JOIN silencieusement

#### Étape 8: Ajouter le Client au Canal
- [x] Appeler channel->addMember(client)
- [x] Cela va:
  - Ajouter le client à l'ensemble des membres
  - Faire du premier membre un opérateur
  - Diffuser JOIN à tous les membres
  - Envoyer le sujet et la liste des noms au client

#### Étape 9: Effacer l'Invitation (si invité)
- [x] Si le client était invité, retirer de la liste des invités
- [x] L'invitation est à usage unique

---

## Validation du Nom de Canal

### Noms de Canaux Valides
- **Commencer par**: `#` (réseau entier) ou `&` (local)
- **Caractères**: Lettres, chiffres, tiret, underscore
- **Non autorisés**: Espaces, virgules, caractères de contrôle, cloche (0x07)
- **Longueur**: 1-50 caractères (incluant #)

### Fonction de Validation
```cpp
bool isValidChannelName(const std::string& name) {
    if (name.empty() || name.length() > 50)
        return false;
    
    // Doit commencer par # ou &
    if (name[0] != '#' && name[0] != '&')
        return false;
    
    // Vérifier les caractères invalides
    for (size_t i = 1; i < name.length(); i++) {
        char c = name[i];
        if (c == ' ' || c == ',' || c == 7 || iscntrl(c))
            return false;
    }
    
    return true;
}
```

---

## Priorité d'Implémentation

### Phase 1 - Join Basique
1. Vérification de l'enregistrement
2. Validation des paramètres
3. Validation du nom du canal
4. Obtenir ou créer le canal
5. Ajouter le client au canal

### Phase 2 - Clés de Canal
6. Vérifier le mode +k
7. Vérifier la clé si nécessaire
8. Envoyer ERR_BADCHANNELKEY si incorrecte

### Phase 3 - Invitation Uniquement
9. Vérifier le mode +i
10. Vérifier le statut d'invitation
11. Effacer l'invitation après le join

### Phase 4 - Limite d'Utilisateurs
12. Vérifier le mode +l
13. Compter les membres actuels
14. Envoyer ERR_CHANNELISFULL si à la limite

### Phase 5 - Canaux Multiples
15. Analyser les canaux séparés par des virgules
16. Analyser les clés séparées par des virgules
17. Associer les clés aux canaux dans l'ordre

---

## Liste de Vérification des Tests

- [ ] JOIN #channel crée un nouveau canal
- [ ] JOIN #channel rejoint un canal existant
- [ ] JOIN sans paramètre envoie l'erreur 461
- [ ] JOIN nom invalide envoie l'erreur 403
- [ ] JOIN avant l'enregistrement envoie l'erreur 451
- [ ] JOIN #channel key utilise la clé fournie
- [ ] JOIN #foo,#bar rejoint plusieurs canaux
- [ ] JOIN canal +k sans clé envoie l'erreur 475
- [ ] JOIN canal +i sans invitation envoie l'erreur 473
- [ ] JOIN canal plein envoie l'erreur 471
- [ ] Le premier membre devient opérateur
- [ ] JOIN diffuse à tous les membres du canal
- [ ] Le client reçoit le sujet et la liste des noms

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **403 ERR_NOSUCHCHANNEL**: "<canal> :No such channel"
- **405 ERR_TOOMANYCHANNELS**: "<canal> :You have joined too many channels"
- **451 ERR_NOTREGISTERED**: ":You have not registered"
- **461 ERR_NEEDMOREPARAMS**: "<commande> :Not enough parameters"
- **471 ERR_CHANNELISFULL**: "<canal> :Cannot join channel (+l)"
- **473 ERR_INVITEONLYCHAN**: "<canal> :Cannot join channel (+i)"
- **475 ERR_BADCHANNELKEY**: "<canal> :Cannot join channel (+k)"

### Réponses de Succès
Après un join réussi, le client reçoit:
1. **Diffusion JOIN**: ":nick!user@host JOIN #channel"
2. **RPL_TOPIC (332)**: Sujet du canal (si défini)
3. **RPL_NAMREPLY (353)**: Liste des membres du canal
4. **RPL_ENDOFNAMES (366)**: Fin de la liste des noms

### Exemple d'Utilisation
```
Client: JOIN #general
Server: :john!john@host JOIN #general
Server: :irc.server 332 john #general :Welcome to general chat
Server: :irc.server 353 john = #general :@john alice bob
Server: :irc.server 366 john #general :End of NAMES list

Client: JOIN #private wrongkey
Server: :irc.server 475 john #private :Cannot join channel (+k)

Client: JOIN #invite
Server: :irc.server 473 john #invite :Cannot join channel (+i)
```

---

## Canaux Multiples

### Syntaxe
```
JOIN <canal1>,<canal2>,<canal3> [<clé1>,<clé2>,<clé3>]
```

### Exemples
```
JOIN #foo,#bar
JOIN #foo,#bar key1,key2
JOIN #public,#private,#secret key2,key3
```

### Correspondance des Clés
- Les clés correspondent aux canaux par position
- `#foo` obtient `key1`, `#bar` obtient `key2`
- Si plus de canaux que de clés, les canaux supplémentaires n'utilisent pas de clé
- Si plus de clés que de canaux, les clés supplémentaires sont ignorées

---

## Messages de Canal Après le Join

### 1. Diffusion JOIN
```
:nick!user@host JOIN #channel
```
- Envoyé à tous les membres (y compris celui qui rejoint)
- Montre qui a rejoint

### 2. Sujet (si défini)
```
:irc.server 332 nick #channel :This is the channel topic
```
- Envoyé uniquement si le sujet est défini
- Si pas de sujet, envoyer RPL_NOTOPIC (331)

### 3. Liste des Noms
```
:irc.server 353 nick = #channel :@operator +voice regular
:irc.server 366 nick #channel :End of NAMES list
```
- 353 peut être envoyé plusieurs fois si beaucoup de membres
- Préfixe @ = opérateur
- Préfixe + = voice (optionnel)
- Pas de préfixe = membre régulier
- 366 marque la fin de la liste

---

## Problèmes Courants

### Problème: Le premier à rejoindre n'est pas opérateur
- Vérifier si le canal est nouvellement créé
- Appeler channel->addOperator(client) pour le premier membre
- Vérifier dans Channel::addMember()

### Problème: JOIN non diffusé aux autres
- S'assurer que Channel::broadcast() est appelé
- Vérifier que le client est ajouté aux membres avant la diffusion
- Ne pas exclure celui qui rejoint de la diffusion JOIN

### Problème: Liste des noms vide
- Vérifier que les membres sont ajoutés au canal
- Vérifier l'implémentation de Channel::getMembers()
- Format: "@nick" pour les opérateurs, "nick" pour régulier

---

## Fichiers Associés
- `Server.hpp/cpp` - getOrCreateChannel()
- `Client.hpp/cpp` - addToChannel()
- `Channel.hpp/cpp` - addMember(), broadcast()
- `InviteCommand.cpp` - Crée les invitations
- `ModeCommand.cpp` - Définit les modes de canal (+i, +k, +l)
- `PartCommand.cpp` - Quitter les canaux

---

## Modes de Canal à Vérifier

### +k (Clé/Mot de passe)
```cpp
if (channel->hasKey()) {
    // Obtenir la clé pour ce canal
    std::string providedKey = getKeyForChannel(channelIndex);
    if (!channel->checkKey(providedKey)) {
        sendError(475, channelName + " :Cannot join channel (+k)");
        continue; // Passer ce canal
    }
}
```

### +i (Invitation Uniquement)
```cpp
if (channel->isInviteOnly()) {
    if (!channel->isInvited(client)) {
        sendError(473, channelName + " :Cannot join channel (+i)");
        continue;
    }
    // Effacer l'invitation après un join réussi
    channel->clearInvite(client);
}
```

### +l (Limite d'Utilisateurs)
```cpp
if (channel->getUserLimit() > 0) {
    if (channel->getMemberCount() >= channel->getUserLimit()) {
        sendError(471, channelName + " :Cannot join channel (+l)");
        continue;
    }
}
```

---

## Cas Spéciaux

### JOIN 0
```
JOIN 0
```
- Commande spéciale: quitter tous les canaux
- Équivalent à PART #chan1,#chan2,#chan3 pour tous les canaux
- Fonctionnalité optionnelle (peut rejeter avec une erreur)

### Sensibilité à la Casse
- Les noms de canaux sont insensibles à la casse
- #General, #general, #GENERAL sont le même canal
- Stocker dans une casse, comparer dans une autre

### Création de Canal Vide
- Si le canal n'existe pas, le créer
- Le premier membre devient opérateur automatiquement
- Pas de sujet, pas de modes définis initialement

### Rejoindre à Nouveau
- Un client peut JOIN un canal dans lequel il est déjà
- Standard IRC: pas d'erreur, succès silencieux
- Ne pas envoyer de messages JOIN en double
