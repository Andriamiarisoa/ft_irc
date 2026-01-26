# Classe ModeCommand - Description de la Tâche

## Vue d'ensemble
La classe `ModeCommand` gère la commande MODE pour définir et visualiser les modes de canal/utilisateur. C'est la commande la plus complexe en raison des multiples modes, paramètres et vérifications de permissions.

## Emplacement de la Classe
- **Header**: `includes/ModeCommand.hpp`
- **Implémentation**: `src/ModeCommand.cpp`

## Commande IRC
**Syntaxe**: 
- Voir les modes: `MODE #channel`
- Définir les modes: `MODE #channel +/-<modes> [paramètres]`

---

## Modes de Canal Supportés (du sujet)

1. **+i / -i** : Canal sur invitation uniquement
2. **+t / -t** : Protection du sujet (changements de sujet réservés aux opérateurs)
3. **+k / -k** : Clé de canal (mot de passe)
   - +k nécessite un paramètre (clé)
   - -k nécessite un paramètre (clé à retirer)
4. **+o / -o** : Privilège d'opérateur
   - Nécessite un paramètre (pseudonyme)
5. **+l / -l** : Limite d'utilisateurs
   - +l nécessite un paramètre (nombre limite)
   - -l n'a pas de paramètre

---

## Méthodes

### Constructeur
```cpp
ModeCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande MODE

**TODO**:
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Visualiser ou définir les modes de canal

**TODO**:

#### Étape 1: Vérifier l'Enregistrement
- [x] Vérifier que le client est complètement enregistré
- [x] Sinon: envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2: Valider les Paramètres
- [x] Vérifier si params contient au moins 1 élément (nom du canal)
- [x] Sinon: envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "MODE :Not enough parameters");
  return;
  ```

#### Étape 3: Extraire le Nom du Canal
- [x] Nom du canal: params[0]
- [x] Valider qu'il commence par '#'

#### Étape 4: Obtenir le Canal
- [x] Appeler server->getChannel(channelName)
- [x] Si le canal n'existe pas: envoyer ERR_NOSUCHCHANNEL (403)
  ```cpp
  sendError(403, channelName + " :No such channel");
  return;
  ```

#### Étape 5: Vérifier que le Client est dans le Canal
- [x] Vérifier si le client est membre du canal
- [x] Sinon: envoyer ERR_NOTONCHANNEL (442)
  ```cpp
  sendError(442, channelName + " :You're not on that channel");
  return;
  ```

#### Étape 6: Déterminer l'Action
- [x] Si params.size() == 1: VISUALISER les modes
- [x] Si params.size() >= 2: DÉFINIR les modes

---

### VISUALISER LES MODES (params.size() == 1)

#### Étape 7a: Obtenir les Modes Actuels
- [x] Appeler channel->getModeString()
- [x] Format: "+itk" ou "+itkl 50" (avec paramètres)

#### Étape 8a: Envoyer RPL_CHANNELMODEIS
- [x] Code de réponse: 324
- [x] Format: "#channel +modes [paramètres]"
  ```cpp
  std::string modeStr = channel->getModeString();
  sendReply(324, channelName + " " + modeStr);
  ```

---

### DÉFINIR LES MODES (params.size() >= 2)

#### Étape 7b: Vérifier le Privilège d'Opérateur
- [x] Seuls les opérateurs peuvent changer les modes
- [x] Si pas opérateur: envoyer ERR_CHANOPRIVSNEEDED (482)
  ```cpp
  if (!channel->isOperator(client)) {
      sendError(482, channelName + " :You're not channel operator");
      return;
  }
  ```

#### Étape 8b: Parser la Chaîne de Mode
- [x] Chaîne de mode: params[1]
- [x] Exemple: "+it", "-k", "+o alice", "+kl password 50"

#### Étape 9b: Initialiser le Parseur de Mode
```cpp
std::string modeString = params[1];
size_t paramIndex = 2;  // Début des paramètres de mode
bool adding = true;     // + ou - ?
std::string appliedModes = "";
std::string appliedParams = "";
```

#### Étape 10b: Itérer sur la Chaîne de Mode
```cpp
for (size_t i = 0; i < modeString.length(); ++i) {
    char mode = modeString[i];
    
    if (mode == '+') {
        adding = true;
        continue;
    }
    if (mode == '-') {
        adding = false;
        continue;
    }
    
    // Traiter le mode individuel
    // ...
}
```

---

### MODE: +i / -i (Sur Invitation Uniquement)

#### Implémentation
```cpp
if (mode == 'i') {
    if (adding) {
        channel->setInviteOnly(true);
        appliedModes += "+i";
    } else {
        channel->setInviteOnly(false);
        appliedModes += "-i";
    }
}
```

**TODO**:
- [x] Définir/retirer le flag sur invitation uniquement
- [x] Aucun paramètre requis
- [x] Ajouter à la chaîne des modes appliqués

---

### MODE: +t / -t (Protection du Sujet)

#### Implémentation
```cpp
if (mode == 't') {
    if (adding) {
        channel->setTopicProtected(true);
        appliedModes += "+t";
    } else {
        channel->setTopicProtected(false);
        appliedModes += "-t";
    }
}
```

**TODO**:
- [x] Définir/retirer le flag de protection du sujet
- [x] Aucun paramètre requis
- [x] Ajouter à la chaîne des modes appliqués

---

### MODE: +k / -k (Clé de Canal)

#### Implémentation
```cpp
if (mode == 'k') {
    // Nécessite un paramètre
    if (paramIndex >= params.size()) {
        sendError(461, "MODE +k :Not enough parameters");
        continue;
    }
    
    std::string key = params[paramIndex++];
    
    if (adding) {
        channel->setKey(key);
        appliedModes += "+k";
        appliedParams += " " + key;
    } else {
        // Vérifier que la clé correspond avant de la retirer
        if (channel->getKey() == key) {
            channel->setKey("");
            appliedModes += "-k";
            appliedParams += " " + key;
        } else {
            sendError(467, channelName + " :Channel key mismatch");
        }
    }
}
```

**TODO**:
- [x] Vérifier que le paramètre existe
- [x] +k: Définir la clé du canal (mot de passe)
- [x] -k: Retirer la clé (vérifier qu'elle correspond)
- [x] Ajouter la clé aux paramètres appliqués
- [x] Envoyer l'erreur 467 si la clé ne correspond pas

---

### MODE: +o / -o (Privilège d'Opérateur)

#### Implémentation
```cpp
if (mode == 'o') {
    // Nécessite un paramètre (pseudonyme)
    if (paramIndex >= params.size()) {
        sendError(461, "MODE +o :Not enough parameters");
        continue;
    }
    
    std::string targetNick = params[paramIndex++];
    
    // Obtenir le client cible
    Client* target = server->getClientByNick(targetNick);
    if (!target) {
        sendError(401, targetNick + " :No such nick/channel");
        continue;
    }
    
    // Vérifier que la cible est dans le canal
    if (!channel->isMember(target)) {
        sendError(441, targetNick + " " + channelName + 
                  " :They aren't on that channel");
        continue;
    }
    
    if (adding) {
        channel->addOperator(target);
        appliedModes += "+o";
        appliedParams += " " + targetNick;
    } else {
        channel->removeOperator(target);
        appliedModes += "-o";
        appliedParams += " " + targetNick;
    }
}
```

**TODO**:
- [x] Vérifier que le paramètre existe (pseudonyme)
- [x] Obtenir le client cible
- [x] Vérifier que la cible existe (erreur 401)
- [x] Vérifier que la cible est dans le canal (erreur 441)
- [x] +o: Ajouter à l'ensemble des opérateurs
- [x] -o: Retirer de l'ensemble des opérateurs
- [x] Ajouter le pseudonyme aux paramètres appliqués

---

### MODE: +l / -l (Limite d'Utilisateurs)

#### Implémentation
```cpp
if (mode == 'l') {
    if (adding) {
        // Nécessite un paramètre (nombre limite)
        if (paramIndex >= params.size()) {
            sendError(461, "MODE +l :Not enough parameters");
            continue;
        }
        
        std::string limitStr = params[paramIndex++];
        
        // Convertir en entier
        int limit = atoi(limitStr.c_str());
        if (limit <= 0) {
            sendError(696, channelName + " l * :Invalid user limit");
            continue;
        }
        
        channel->setUserLimit(limit);
        appliedModes += "+l";
        appliedParams += " " + limitStr;
    } else {
        // -l: Pas de paramètre, juste retirer la limite
        channel->setUserLimit(0);  // 0 = pas de limite
        appliedModes += "-l";
    }
}
```

**TODO**:
- [x] +l: Vérifier que le paramètre existe (nombre limite)
- [x] +l: Valider que la limite est un entier positif
- [x] +l: Définir la limite d'utilisateurs
- [x] -l: Retirer la limite (pas de paramètre)
- [x] Ajouter la limite aux paramètres appliqués (seulement pour +l)

---

### Étape 11: Gérer les Modes Inconnus
```cpp
// Si le mode ne correspond pas à i, t, k, o, l
sendError(472, std::string(1, mode) + " :is unknown mode char to me");
continue;
```

**TODO**:
- [x] Envoyer ERR_UNKNOWNMODE (472) pour les modes invalides
- [x] Continuer le traitement des modes restants

---

### Étape 12: Diffuser les Changements de Mode
- [x] Diffuser seulement si des modes ont été effectivement changés
- [x] Format: ":nick!user@host MODE #channel +modes paramètres"
  ```cpp
  if (!appliedModes.empty()) {
      std::string modeMsg = ":" + client->getNickname() + "!" +
                            client->getUsername() + "@host MODE " +
                            channelName + " " + appliedModes;
      if (!appliedParams.empty()) {
          modeMsg += appliedParams;
      }
      modeMsg += "\r\n";
      channel->broadcast(modeMsg);
  }
  ```

**TODO**:
- [ ] Vérifier si des modes ont été appliqués
- [ ] Construire le message MODE avec les modes et paramètres appliqués
- [ ] Diffuser à tous les membres du canal

---

## Priorité d'Implémentation

### Phase 1 - Structure de Base
1. Vérification de l'enregistrement
2. Validation des paramètres
3. Vérifications d'existence du canal et d'appartenance
4. Vérification des privilèges d'opérateur

### Phase 2 - Visualiser les Modes
5. Implémenter RPL_CHANNELMODEIS
6. Formater la chaîne de mode avec les paramètres

### Phase 3 - Modes Simples (sans paramètres)
7. Implémenter +i / -i
8. Implémenter +t / -t
9. Tester chacun individuellement

### Phase 4 - Modes avec Paramètres
10. Implémenter +k / -k avec le paramètre clé
11. Implémenter +l / -l avec le paramètre limite
12. Implémenter +o / -o avec le paramètre pseudonyme

### Phase 5 - Parsing des Modes
13. Gérer les bascules '+' et '-'
14. Parser plusieurs modes en une commande
15. Gérer l'extraction des paramètres

### Phase 6 - Diffusion
16. Diffuser les changements réussis
17. Formater avec seulement les modes appliqués
18. Inclure les paramètres dans la diffusion

---

## Liste de Vérification des Tests

### Visualiser les Modes
- [x] MODE #channel affiche les modes actuels
- [x] Les modes vides affichent "+"
- [x] Les modes avec paramètres sont affichés correctement

### Modes Simples
- [x] MODE #channel +i définit sur invitation uniquement
- [x] MODE #channel -i retire sur invitation uniquement
- [x] MODE #channel +t définit la protection du sujet
- [x] MODE #channel -t retire la protection du sujet
- [x] MODE #channel +it définit les deux

### Mode Clé
- [x] MODE #channel +k password définit la clé
- [x] JOIN nécessite la clé après +k
- [x] MODE #channel -k password retire la clé
- [x] MODE #channel -k wrongpass échoue (erreur 467)

### Mode Opérateur
- [x] MODE #channel +o alice donne l'opérateur
- [x] MODE #channel -o alice retire l'opérateur
- [x] MODE #channel +o nonexistent envoie l'erreur 401
- [x] MODE #channel +o notinchannel envoie l'erreur 441
- [x] Le nouvel opérateur peut définir les modes

### Mode Limite
- [x] MODE #channel +l 50 définit la limite
- [x] MODE #channel -l retire la limite
- [x] JOIN échoue quand la limite est atteinte
- [x] MODE #channel +l 0 échoue (invalide)
- [x] MODE #channel +l abc échoue (invalide)

### Tests de Permission
- [x] Le changement MODE par un non-opérateur échoue (erreur 482)
- [x] Le changement MODE par un opérateur réussit
- [x] MODE par un non-membre échoue (erreur 442)

### Modes Multiples
- [x] MODE #channel +it définit les deux
- [x] MODE #channel +kl pass 50 définit les deux avec params
- [x] MODE #channel +o-o alice bob donne/retire les ops
- [x] MODE #channel -itk pass retire tous

### Tests de Diffusion
- [x] Tous les membres voient la diffusion MODE
- [x] La diffusion inclut les paramètres
- [x] L'expéditeur reçoit la diffusion

---

## Notes du Protocole IRC

### Codes d'Erreur
- **401 ERR_NOSUCHNICK**: "<nickname> :No such nick/channel"
- **403 ERR_NOSUCHCHANNEL**: "<channel> :No such channel"
- **441 ERR_USERNOTINCHANNEL**: "<nick> <channel> :They aren't on that channel"
- **442 ERR_NOTONCHANNEL**: "<channel> :You're not on that channel"
- **451 ERR_NOTREGISTERED**: ":You have not registered"
- **461 ERR_NEEDMOREPARAMS**: "<command> :Not enough parameters"
- **467 ERR_KEYSET**: "<channel> :Channel key already set" (ou clé non correspondante)
- **472 ERR_UNKNOWNMODE**: "<char> :is unknown mode char to me"
- **482 ERR_CHANOPRIVSNEEDED**: "<channel> :You're not channel operator"
- **696 ERR_INVALIDMODEPARAM**: "<channel> <mode> <param> :Invalid mode parameter"

### Codes de Succès
- **324 RPL_CHANNELMODEIS**: "<channel> <mode> <mode params>"

### Formats de Message

#### Visualiser les Modes
```
Client: MODE #general
Server: :irc.server 324 alice #general +itk secretkey
```

#### Définir les Modes (Diffusion)
```
Client: MODE #general +it
Server: (à tous les membres)
        :alice!alice@host MODE #general +it
```

#### Définir les Modes avec Paramètres
```
Client: MODE #general +kl secretkey 50
Server: (à tous les membres)
        :alice!alice@host MODE #general +kl secretkey 50
```

#### Donner/Retirer l'Opérateur
```
Client: MODE #general +o bob
Server: (à tous les membres)
        :alice!alice@host MODE #general +o bob

Client: MODE #general -o bob
Server: (à tous les membres)
        :alice!alice@host MODE #general -o bob
```

---

## Format de Chaîne de Mode

### Visualisation des Modes
- Vide: `+`
- Sur invitation uniquement: `+i`
- Sujet protégé: `+t`
- Avec clé: `+k <key>`
- Avec limite: `+l <limit>`
- Multiples: `+itkl <key> <limit>`

### Définition des Modes
- Simple: `+i`
- Multiples: `+it`
- Avec paramètre: `+k password`
- Multiples avec params: `+kl password 50`
- Mixte: `+o-o alice bob` (donner alice, retirer bob)
- Retirer: `-itk oldpassword`

---

## Gestion des Paramètres de Mode

### Modes Nécessitant des Paramètres

| Mode | Ajout (+)  | Retrait (-) | Paramètre         |
|------|------------|-------------|-------------------|
| i    | Non        | Non         | -                 |
| t    | Non        | Non         | -                 |
| k    | Oui        | Oui         | Chaîne clé        |
| o    | Oui        | Oui         | Pseudonyme        |
| l    | Oui        | Non         | Nombre limite     |

### Ordre des Paramètres
```
MODE #channel +kol password alice 50
                 ^   ^        ^     ^
                 k   o        l     params[2,3,4]
```

Paramètres extraits séquentiellement:
1. +k → params[2] = "password"
2. +o → params[3] = "alice"
3. +l → params[4] = "50"

---

## Méthodes de la Classe Channel

### Mode Sur Invitation Uniquement
```cpp
void Channel::setInviteOnly(bool value);
bool Channel::isInviteOnly() const;
```

### Mode Protection du Sujet
```cpp
void Channel::setTopicProtected(bool value);
bool Channel::isTopicProtected() const;
```

### Mode Clé
```cpp
void Channel::setKey(const std::string& key);
const std::string& Channel::getKey() const;
bool Channel::hasKey() const;
```

### Mode Limite d'Utilisateurs
```cpp
void Channel::setUserLimit(int limit);
int Channel::getUserLimit() const;
bool Channel::hasUserLimit() const;
```

### Gestion des Opérateurs
```cpp
void Channel::addOperator(Client* client);
void Channel::removeOperator(Client* client);
bool Channel::isOperator(Client* client) const;
```

### Obtenir la Chaîne de Mode
```cpp
std::string Channel::getModeString() const {
    std::string modes = "+";
    std::string params = "";
    
    if (inviteOnly) modes += "i";
    if (topicProtected) modes += "t";
    if (hasKey()) {
        modes += "k";
        params += " " + key;
    }
    if (hasUserLimit()) {
        modes += "l";
        params += " " + intToString(userLimit);
    }
    
    if (modes == "+") return "+";
    return modes + params;
}
```

---

## Exemple de Parsing Complexe de Mode

### Entrée
```
MODE #channel +itko-l password alice
```

### Étapes de Parsing
1. Début: adding = true
2. 'i': Appliquer +i
3. 't': Appliquer +t
4. 'k': Appliquer +k avec param "password"
5. 'o': Appliquer +o avec param "alice"
6. '-': Basculer vers adding = false
7. 'l': Appliquer -l (pas de param)

### Résultat
- Le canal est sur invitation uniquement
- Le canal a le sujet protégé
- Le canal a la clé "password"
- Alice est opératrice
- Limite d'utilisateurs retirée

### Diffusion
```
:operator!user@host MODE #channel +itko-l password alice
```

---

## Problèmes Courants

### Problème: Paramètres non extraits
- Suivre paramIndex attentivement
- Incrémenter après chaque utilisation de paramètre
- Vérifier paramIndex < params.size() avant l'accès

### Problème: L'ordre des modes compte
- Traiter les modes de gauche à droite
- Les bascules '+' et '-' affectent les modes suivants
- Exemple: "+i-i" résulte en -i (le dernier gagne)

### Problème: La diffusion montre de mauvais modes
- Ajouter seulement les modes appliqués avec succès
- Ignorer les modes en erreur
- Construire la chaîne appliedModes de manière incrémentale

### Problème: Le retrait de clé échoue
- Vérifier que la clé correspond à la clé actuelle
- Utiliser la comparaison ==
- Envoyer l'erreur 467 si non correspondance

---

## Fichiers Liés
- `Server.hpp/cpp` - getChannel(), getClientByNick()
- `Channel.hpp/cpp` - Setters/getters de mode, getModeString()
- `JoinCommand.cpp` - Vérifier les modes (+i, +k, +l)
- `TopicCommand.cpp` - Vérifier le mode +t
- `InviteCommand.cpp` - Vérifier le mode +i

---

## Scénarios de Test

### Opérations de Mode de Base
1. Voir les modes vides: `MODE #channel` → `+`
2. Définir +i: `MODE #channel +i`
3. Définir +t: `MODE #channel +t`
4. Définir les deux: `MODE #channel +it`
5. Retirer +i: `MODE #channel -i`
6. Voir les modes: `MODE #channel` → `+t`

### Test du Mode Clé
7. Définir la clé: `MODE #channel +k secret`
8. JOIN nécessite la clé
9. Mauvaise clé échoue
10. Retirer la clé: `MODE #channel -k secret`
11. JOIN ne nécessite plus la clé
12. Retrait de mauvaise clé échoue: `MODE #channel -k wrong` → erreur 467

### Test du Mode Limite
13. Définir la limite: `MODE #channel +l 2`
14. Troisième JOIN échoue (plein)
15. Retirer la limite: `MODE #channel -l`
16. Troisième JOIN réussit

### Test du Mode Opérateur
17. Donner op: `MODE #channel +o bob`
18. Bob peut définir les modes
19. Retirer op: `MODE #channel -o bob`
20. Bob ne peut pas définir les modes
21. Donner op à non-existant: erreur 401
22. Donner op à non-membre: erreur 441

### Chaînes de Mode Complexes
23. `MODE #channel +itk secret`
24. `MODE #channel +kl secret 50`
25. `MODE #channel +o-o alice bob`
26. `MODE #channel -itk secret`

### Test de Permission
27. Tentative MODE par non-opérateur: erreur 482
28. Tentative MODE par non-membre: erreur 442
29. MODE par opérateur réussit

### Cas Limites
30. Mode inconnu: `MODE #channel +x` → erreur 472
31. Paramètre manquant: `MODE #channel +k` → erreur 461
32. Limite invalide: `MODE #channel +l 0` → erreur 696
33. Limite invalide: `MODE #channel +l abc` → erreur 696

---

## Persistance des Modes

### Durée de Vie du Canal
- Les modes persistent jusqu'à ce qu'ils soient explicitement changés
- Survivent au départ de tous les membres (si le canal persiste)
- Le statut +o persiste tant qu'on est dans le canal

### L'Utilisateur Quitte le Canal
- Le statut +o est perdu quand l'utilisateur quitte
- Doit être ré-accordé lors du retour
- Liste des opérateurs mise à jour

### Recréation du Canal
- Un canal supprimé perd tous les modes
- Un nouveau canal commence avec les modes par défaut
- Le premier utilisateur devient opérateur

---

## Considérations de Sécurité

### Abus d'Opérateur
- Les opérateurs ont un pouvoir significatif
- Peuvent kicker, changer les modes, contrôler l'accès
- Envisager de logger les actions des opérateurs

### Sécurité de la Clé
- Les clés sont visibles par tous les opérateurs
- MODE #channel affiche la clé
- Envisager de ne pas montrer la clé dans la vue MODE (optionnel)

### Contournement de Limite
- Optionnel: Les opérateurs contournent la limite
- Standard: Tout le monde respecte la limite
- Pour ce projet: Comportement standard

---

## Modes de Canal par Défaut

### Nouveau Canal
- Quand le premier utilisateur crée le canal:
  ```cpp
  // L'utilisateur qui crée le canal devient opérateur
  channel->addOperator(firstUser);
  
  // Modes par défaut (spécifiques au projet)
  channel->setInviteOnly(false);     // -i
  channel->setTopicProtected(false);  // -t (ou true pour +t)
  channel->setKey("");                // pas de clé
  channel->setUserLimit(0);           // pas de limite
  ```

### Valeurs par Défaut Recommandées
- **-i**: Pas sur invitation uniquement (ouvert)
- **+t**: Sujet protégé (empêche le spam)
- **-k**: Pas de clé
- **-l**: Pas de limite

---

## Flux de la Commande Mode

```
1. MODE #channel +itko-l password alice

2. Parser: adding=true, modes="itko-l", params=["password","alice"]

3. Traiter:
   - 'i': +i → inviteOnly=true
   - 't': +t → topicProtected=true
   - 'k': +k password → key="password"
   - 'o': +o alice → addOperator(alice)
   - '-': adding=false
   - 'l': -l → userLimit=0

4. Diffuser: ":nick!user@host MODE #channel +itko-l password alice"

5. Tous les membres reçoivent la mise à jour MODE
```

---

## Liste de Vérification Finale

- [ ] Visualiser les modes (RPL_CHANNELMODEIS)
- [ ] Définir +i / -i (sur invitation uniquement)
- [ ] Définir +t / -t (protection du sujet)
- [ ] Définir +k / -k avec paramètre clé
- [ ] Définir +o / -o avec paramètre pseudonyme
- [ ] Définir +l / -l avec paramètre limite
- [ ] Parser plusieurs modes en une commande
- [ ] Gérer les bascules '+' et '-'
- [ ] Extraire les paramètres correctement
- [ ] Valider le nombre de paramètres
- [ ] Vérifier le privilège d'opérateur
- [ ] Gérer les modes inconnus (erreur 472)
- [ ] Diffuser les changements réussis
- [ ] Intégration avec JOIN (vérifier les modes)
- [ ] Intégration avec TOPIC (vérifier +t)
- [ ] Intégration avec INVITE (vérifier +i)
