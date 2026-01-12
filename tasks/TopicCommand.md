# Classe TopicCommand - Description de la Tâche

## Vue d'ensemble
La classe `TopicCommand` gère la commande TOPIC pour afficher et définir les sujets des canaux. Les sujets peuvent être protégés avec le mode +t (opérateurs uniquement).

## Emplacement de la Classe
- **En-tête** : `includes/TopicCommand.hpp`
- **Implémentation** : `src/TopicCommand.cpp`

## Commande IRC
**Syntaxe** : 
- Voir le sujet : `TOPIC #canal`
- Définir le sujet : `TOPIC #canal :Nouveau message de sujet`

---

## Méthodes

### Constructeur
```cpp
TopicCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande TOPIC

**TODO** :
- [ ] Appeler le constructeur de la classe de base
- [ ] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Voir ou définir le sujet du canal

**TODO** :

#### Étape 1 : Vérifier l'Enregistrement
- [ ] Vérifier que le client est complètement enregistré
- [ ] Sinon : envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2 : Valider les Paramètres
- [ ] Vérifier que params a au moins 1 élément (nom du canal)
- [ ] Sinon : envoyer ERR_NEEDMOREPARAMS (461)
  ```cpp
  sendError(461, "TOPIC :Not enough parameters");
  return;
  ```

#### Étape 3 : Extraire le Nom du Canal
- [ ] Nom du canal : params[0]

#### Étape 4 : Obtenir le Canal
- [ ] Appeler server->getChannel(channelName)
- [ ] Si le canal n'existe pas : envoyer ERR_NOSUCHCHANNEL (403)
  ```cpp
  sendError(403, channelName + " :No such channel");
  return;
  ```

#### Étape 5 : Vérifier que le Client est dans le Canal
- [ ] Vérifier si le client est membre du canal
- [ ] Sinon : envoyer ERR_NOTONCHANNEL (442)
  ```cpp
  sendError(442, channelName + " :You're not on that channel");
  return;
  ```

#### Étape 6 : Déterminer l'Action (Voir ou Définir)
- [ ] Si params.size() == 1 : VOIR le sujet
- [ ] Si params.size() >= 2 : DÉFINIR le sujet

---

### VOIR LE SUJET (params.size() == 1)

#### Étape 7a : Vérifier si le Sujet est Défini
- [ ] Appeler channel->getTopic()
- [ ] Si le sujet est vide : envoyer RPL_NOTOPIC (331)
  ```cpp
  sendReply(331, channelName + " :No topic is set");
  return;
  ```

#### Étape 8a : Envoyer le Sujet au Client
- [ ] Envoyer RPL_TOPIC (332) avec le texte du sujet
  ```cpp
  sendReply(332, channelName + " :" + channel->getTopic());
  ```

#### Étape 9a : Optionnel - Envoyer les Infos de Définition du Sujet
- [ ] Envoyer RPL_TOPICWHOTIME (333) avec l'auteur et l'horodatage
- [ ] Format : "#canal auteur horodatage"
  ```cpp
  // Optionnel - si vous suivez qui/quand le sujet a été défini
  sendReply(333, channelName + " " + channel->getTopicSetter() + 
            " " + channel->getTopicTime());
  ```

---

### DÉFINIR LE SUJET (params.size() >= 2)

#### Étape 7b : Extraire le Nouveau Sujet
- [ ] Joindre tous les params à partir de l'index 1
- [ ] Supprimer le ':' initial s'il est présent
  ```cpp
  std::string newTopic = params[1];
  for (size_t i = 2; i < params.size(); ++i) {
      newTopic += " " + params[i];
  }
  // Supprimer le ':' initial s'il est présent
  if (!newTopic.empty() && newTopic[0] == ':') {
      newTopic = newTopic.substr(1);
  }
  ```

#### Étape 8b : Vérifier le Mode de Protection du Sujet (+t)
- [ ] Si le canal a le mode +t (protection du sujet) :
  - Vérifier si le client est opérateur du canal
  - Sinon : envoyer ERR_CHANOPRIVSNEEDED (482)
  ```cpp
  if (channel->isTopicProtected() && !channel->isOperator(client)) {
      sendError(482, channelName + " :You're not channel operator");
      return;
  }
  ```

#### Étape 9b : Définir le Sujet
- [ ] Appeler channel->setTopic(newTopic)
- [ ] Optionnel : Sauvegarder le pseudo de l'auteur et l'horodatage
  ```cpp
  channel->setTopic(newTopic);
  // Optionnel
  channel->setTopicSetter(client->getNickname());
  channel->setTopicTime(time(NULL));
  ```

#### Étape 10b : Diffuser le Changement de TOPIC
- [ ] Envoyer le message TOPIC à TOUS les membres du canal (y compris l'émetteur)
- [ ] Format : ":nick!user@host TOPIC #canal :nouveau sujet"
  ```cpp
  std::string topicMsg = ":" + client->getNickname() + "!" +
                         client->getUsername() + "@host TOPIC " +
                         channelName + " :" + newTopic + "\r\n";
  channel->broadcast(topicMsg);
  ```

---

### EFFACER LE SUJET (sujet vide)

#### Cas Spécial : Supprimer le Sujet
```
TOPIC #canal :
```

- [ ] Une chaîne vide après ':' efface le sujet
- [ ] Nécessite toujours la permission d'opérateur si +t
- [ ] Diffuser le sujet vide à tous les membres
  ```cpp
  std::string topicMsg = ":" + client->getNickname() + "!" +
                         client->getUsername() + "@host TOPIC " +
                         channelName + " :\r\n";
  channel->broadcast(topicMsg);
  ```

---

## Priorité d'Implémentation

### Phase 1 - Validation de Base
1. Vérification de l'enregistrement
2. Validation des paramètres (nom du canal)
3. Vérification de l'existence du canal
4. Vérification de l'appartenance

### Phase 2 - Voir le Sujet
5. Déterminer l'action voir vs définir
6. Vérifier si le sujet existe
7. Envoyer RPL_TOPIC ou RPL_NOTOPIC

### Phase 3 - Définir le Sujet
8. Extraire le nouveau texte du sujet
9. Vérifier le mode +t et le statut d'opérateur
10. Définir le sujet dans le canal
11. Diffuser à tous les membres

---

## Liste de Vérification des Tests

- [ ] TOPIC #canal affiche le sujet actuel
- [ ] TOPIC #canal :Nouveau affiche RPL_NOTOPIC s'il n'y a pas de sujet
- [ ] TOPIC #canal :Nouveau définit le sujet
- [ ] Le sujet est diffusé à tous les membres
- [ ] TOPIC sans paramètres envoie l'erreur 461
- [ ] TOPIC #inexistant envoie l'erreur 403
- [ ] TOPIC par un non-membre envoie l'erreur 442
- [ ] TOPIC sur un canal +t par un non-op envoie l'erreur 482
- [ ] TOPIC sur un canal +t par un opérateur réussit
- [ ] TOPIC sur un canal normal par un membre réussit
- [ ] Un sujet vide efface le sujet
- [ ] Un sujet avec plusieurs mots est préservé
- [ ] Un sujet avec des caractères spéciaux est préservé

---

## Notes du Protocole IRC

### Codes d'Erreur
- **403 ERR_NOSUCHCHANNEL** : "<canal> :No such channel"
- **442 ERR_NOTONCHANNEL** : "<canal> :You're not on that channel"
- **451 ERR_NOTREGISTERED** : ":You have not registered"
- **461 ERR_NEEDMOREPARAMS** : "<commande> :Not enough parameters"
- **482 ERR_CHANOPRIVSNEEDED** : "<canal> :You're not channel operator"

### Codes de Succès
- **331 RPL_NOTOPIC** : "<canal> :No topic is set"
- **332 RPL_TOPIC** : "<canal> :<sujet>"
- **333 RPL_TOPICWHOTIME** : "<canal> <qui> <heure>" (optionnel)

### Formats de Message

#### Voir le Sujet
```
Client: TOPIC #general
Serveur: :irc.server 332 alice #general :Welcome to General Chat
Serveur: :irc.server 333 alice #general bob 1234567890
```

#### Pas de Sujet
```
Client: TOPIC #general
Serveur: :irc.server 331 alice #general :No topic is set
```

#### Définir le Sujet (Diffusion)
```
Client: TOPIC #general :New topic for today
Serveur: (à tous les membres y compris l'émetteur)
        :alice!alice@host TOPIC #general :New topic for today
```

#### Effacer le Sujet
```
Client: TOPIC #general :
Serveur: (à tous les membres)
        :alice!alice@host TOPIC #general :
```

---

## Stockage du Sujet dans Channel

### Méthodes de la Classe Channel

#### Obtenir le Sujet
```cpp
const std::string& Channel::getTopic() const {
    return topic;
}
```

#### Définir le Sujet
```cpp
void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}
```

#### Vérifier si Vide
```cpp
bool Channel::hasTopic() const {
    return !topic.empty();
}
```

#### Optionnel : Métadonnées du Sujet
```cpp
const std::string& Channel::getTopicSetter() const {
    return topicSetter;
}

void Channel::setTopicSetter(const std::string& setter) {
    topicSetter = setter;
}

time_t Channel::getTopicTime() const {
    return topicTime;
}

void Channel::setTopicTime(time_t time) {
    topicTime = time;
}
```

---

## Mode de Protection du Sujet (+t)

### Quand +t est Actif
- Seuls les opérateurs du canal peuvent changer le sujet
- Les membres normaux peuvent seulement voir
- Les changements de sujet nécessitent des privilèges d'opérateur

### Quand +t est Inactif
- N'importe quel membre du canal peut changer le sujet
- Aucune exigence d'opérateur
- Mode par défaut pour les nouveaux canaux (spécifique au projet)

### Vérification des Permissions
```cpp
if (channel->isTopicProtected()) {
    if (!channel->isOperator(client)) {
        sendError(482, channelName + " :You're not channel operator");
        return;
    }
}
```

---

## Considérations sur le Format du Sujet

### Sujets Multi-Mots
```
TOPIC #canal :Ceci est un sujet multi-mots
```
- Tout le texte après le premier ':' est le sujet
- Les espaces sont préservés
- La casse est préservée

### Caractères Spéciaux
```
TOPIC #canal :Bienvenue! @tout-le-monde #règles
```
- Les caractères spéciaux sont autorisés
- Aucun filtrage nécessaire (IRC est permissif)
- Les deux-points, dièses, etc. sont acceptables

### Chaîne Vide vs Pas de Paramètres
```
TOPIC #canal         -> Voir le sujet actuel
TOPIC #canal :       -> Effacer le sujet (chaîne vide)
```

### Limites de Longueur
- Standard IRC : Pas de limite fixe
- Pratique courante : 80-390 caractères
- Pour ce projet : Limite optionnelle (par ex., 390)
  ```cpp
  if (newTopic.length() > 390) {
      newTopic = newTopic.substr(0, 390);
  }
  ```

---

## Problèmes Courants

### Problème : Le sujet n'est pas diffusé
- Vérifier que la diffusion inclut TOUS les membres
- Vérifier que le format du message inclut '\r\n'
- S'assurer que la diffusion se produit après setTopic()

### Problème : Le sujet multi-mots est tronqué
- Joindre TOUS les params à partir de l'index 1
- Ne pas utiliser seulement params[1]
- Gérer correctement le préfixe ':'

### Problème : L'opérateur ne peut pas définir le sujet
- Vérifier isTopicProtected() vs. isOperator()
- Vérifier que le mode +t est réellement défini
- Tester avec les modes +t et -t

---

## Fichiers Associés
- `Server.hpp/cpp` - getChannel()
- `Channel.hpp/cpp` - getTopic(), setTopic(), isTopicProtected()
- `ModeCommand.cpp` - Définir le mode +t/-t
- `JoinCommand.cpp` - Envoyer le sujet lors de la jonction

---

## Intégration avec JOIN

### Envoyer le Sujet lors de la Jonction
```cpp
// Dans JoinCommand::execute() après une jonction réussie
if (channel->hasTopic()) {
    // Envoyer RPL_TOPIC
    std::string topicReply = ":irc.server 332 " + 
                             client->getNickname() + " " +
                             channelName + " :" +
                             channel->getTopic() + "\r\n";
    client->sendMessage(topicReply);
    
    // Optionnel : Envoyer RPL_TOPICWHOTIME
    if (!channel->getTopicSetter().empty()) {
        std::string whoTimeReply = ":irc.server 333 " +
                                   client->getNickname() + " " +
                                   channelName + " " +
                                   channel->getTopicSetter() + " " +
                                   intToString(channel->getTopicTime()) + "\r\n";
        client->sendMessage(whoTimeReply);
    }
}
```

---

## Scénarios de Test

### Fonctionnalité de Base
1. Voir le sujet avec TOPIC #canal
2. Définir le sujet avec TOPIC #canal :texte
3. Effacer le sujet avec TOPIC #canal :
4. Voir le sujet effacé (RPL_NOTOPIC)

### Tests de Permissions
5. Un membre définit le sujet sur un canal normal
6. Un membre essaie de définir le sujet sur un canal +t (refusé)
7. Un opérateur définit le sujet sur un canal +t (autorisé)
8. Un opérateur définit le sujet sur un canal normal (autorisé)

### Tests de Diffusion
9. Tous les membres voient la diffusion TOPIC
10. L'émetteur reçoit aussi la diffusion
11. Les nouveaux arrivants voient le sujet avec RPL_TOPIC

### Tests de Format
12. Sujet d'un seul mot
13. Sujet multi-mots avec espaces
14. Sujet avec caractères spéciaux
15. Sujet très long (si limite appliquée)
16. Sujet vide (effacer)

### Cas d'Erreur
17. TOPIC sans nom de canal (erreur 461)
18. TOPIC sur un canal inexistant (erreur 403)
19. TOPIC par un non-membre (erreur 442)

---

## Matrice des Permissions

### Qui Peut Définir le Sujet ?

| Mode du Canal | Type de Membre | Peut Définir le Sujet ? |
|---------------|----------------|-------------------------|
| Normal        | Opérateur      | Oui                     |
| Normal        | Membre         | Oui                     |
| +t            | Opérateur      | Oui                     |
| +t            | Membre         | Non (erreur 482)        |

### Qui Peut Voir le Sujet ?

| Statut du Membre | Peut Voir ? |
|------------------|-------------|
| Membre           | Oui         |
| Non-membre       | Non         |

---

## Persistance du Sujet

### Durant la Durée de Vie du Canal
- Le sujet persiste jusqu'à ce qu'il soit explicitement changé
- Survit au départ de tous les membres (si le canal persiste)
- Survit aux changements d'opérateurs

### Suppression du Canal
- Le sujet est perdu quand le canal est supprimé
- Un nouveau canal commence sans sujet
- Le sujet n'est pas sauvegardé entre les redémarrages du serveur

---

## Exemples d'Utilisation

### Changement de Sujet Réussi
```
alice: TOPIC #general :Bienvenue au Chat Général
Serveur: (à alice)
        :alice!alice@host TOPIC #general :Bienvenue au Chat Général
Serveur: (à bob)
        :alice!alice@host TOPIC #general :Bienvenue au Chat Général
Serveur: (à charlie)
        :alice!alice@host TOPIC #general :Bienvenue au Chat Général
```

### Sujet Réservé aux Opérateurs
```
alice: TOPIC #private :Nouveau sujet
Serveur: (alice n'est pas opérateur)
        :irc.server 482 alice #private :You're not channel operator

bob: MODE #private +o alice
alice: TOPIC #private :Nouveau sujet
Serveur: (diffusion à tous)
        :alice!alice@host TOPIC #private :Nouveau sujet
```

### Voir le Sujet
```
alice: TOPIC #general
Serveur: :irc.server 332 alice #general :Bienvenue au Chat Général
Serveur: :irc.server 333 alice #general bob 1234567890
```

---

## Considérations de Sécurité

### Inondation de Sujets
- Les changements rapides de sujet peuvent spammer le canal
- Considérer une limitation de débit par utilisateur (optionnel)
- Enregistrer les changements excessifs

### Contenu Malveillant
- Pas de filtrage de contenu dans IRC de base
- Les utilisateurs sont responsables du contenu
- Optionnel : Limite de longueur pour prévenir les abus

### Historique du Sujet
- Non suivi dans IRC de base
- Optionnel : Garder les N derniers sujets
- Pour ce projet : Non requis

---

## Améliorations Optionnelles

### Historique du Sujet
```cpp
class Channel {
private:
    std::vector<std::string> topicHistory;
    
public:
    void setTopic(const std::string& newTopic) {
        topicHistory.push_back(newTopic);
        if (topicHistory.size() > 10) {
            topicHistory.erase(topicHistory.begin());
        }
        topic = newTopic;
    }
};
```

### Limite de Longueur du Sujet
```cpp
const size_t MAX_TOPIC_LENGTH = 390;

if (newTopic.length() > MAX_TOPIC_LENGTH) {
    newTopic = newTopic.substr(0, MAX_TOPIC_LENGTH);
    // Optionnel : Notifier l'utilisateur de la troncature
}
```

### RPL_TOPICWHOTIME
- Suivre qui a défini le sujet
- Suivre quand le sujet a été défini
- Envoyer avec RPL_TOPIC (réponse 333)
- Fonctionnalité IRC moderne
- Optionnel pour ce projet

---

## Cas Limites de l'Analyse

### Sujet avec Deux-Points Initial
```
TOPIC #canal ::Ceci a un deux-points initial
```
- Le premier ':' est le délimiteur IRC
- Le second ':' fait partie du sujet
- Résultat : ":Ceci a un deux-points initial"

### Sujet sans Deux-Points
```
TOPIC #canal mot_unique
```
- params[1] = "mot_unique"
- Pas besoin de ':' pour un seul mot
- Toujours valide

### Paramètres Multiples
```
TOPIC #canal :Mot1 Mot2 Mot3
```
- MessageParser devrait mettre tout après ':' dans params[1]
- Ou joindre manuellement params[1..n]
- S'assurer que le sujet complet est préservé

---

## Affichage Client

### Clients IRC Modernes
- Afficher le sujet en haut de la fenêtre du canal
- Mettre à jour en temps réel quand il change
- Cliquer pour éditer (si opérateur)
- Afficher qui a défini le sujet et quand

### Affichage Console
```
*** Sujet pour #general: "Bienvenue au Chat Général"
*** Sujet défini par bob le Jeu 01 Jan 12:00:00 2024
```

---

## Liste de Vérification Finale

- [ ] Voir le sujet fonctionne pour les membres
- [ ] Définir le sujet fonctionne avec les permissions correctes
- [ ] Effacer le sujet fonctionne avec TOPIC #canal :
- [ ] La diffusion inclut tous les membres
- [ ] Le mode +t restreint aux opérateurs
- [ ] Le non-+t autorise tous les membres
- [ ] RPL_NOTOPIC envoyé quand il n'y a pas de sujet
- [ ] RPL_TOPIC envoyé avec le texte du sujet
- [ ] Les sujets multi-mots sont préservés
- [ ] Les messages d'erreur sont corrects
- [ ] La commande join affiche le sujet aux nouveaux membres
