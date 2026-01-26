# Classe PrivmsgCommand - Description de la Tâche

## Vue d'ensemble
La classe `PrivmsgCommand` gère la commande PRIVMSG pour envoyer des messages aux utilisateurs ou aux canaux. C'est la fonctionnalité de messagerie centrale d'IRC.

## Emplacement de la Classe
- **En-tête** : `includes/PrivmsgCommand.hpp`
- **Implémentation** : `src/PrivmsgCommand.cpp`

## Commande IRC
**Syntaxe** : `PRIVMSG <cible> :<message>`

---

## Méthodes

### Constructeur
```cpp
PrivmsgCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif** : Initialiser la commande PRIVMSG

**TODO** :
- [x] Appeler le constructeur de la classe de base
- [x] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif** : Envoyer un message à un utilisateur ou un canal

**TODO** :

#### Étape 1 : Vérifier l'Enregistrement
- [x] Vérifier que le client est complètement enregistré
- [x] Si ce n'est pas le cas : envoyer ERR_NOTREGISTERED (451)
  ```cpp
  if (!client->isRegistered()) {
      sendError(451, ":You have not registered");
      return;
  }
  ```

#### Étape 2 : Valider les Paramètres
- [x] Vérifier si params contient au moins 2 éléments (cible et message)
- [x] Si la cible manque : envoyer ERR_NORECIPIENT (411)
  ```cpp
  sendError(411, ":No recipient given (PRIVMSG)");
  return;
  ```
- [x] Si le message manque : envoyer ERR_NOTEXTTOSEND (412)
  ```cpp
  sendError(412, ":No text to send");
  return;
  ```

#### Étape 3 : Extraire la Cible et le Message
- [x] Cible : params[0]
- [x] Message : params[1] (tout ce qui suit le premier ':')
- [x] Exemple : "PRIVMSG #general :Bonjour tout le monde"
  - Cible : "#general"
  - Message : "Bonjour tout le monde"

#### Étape 4 : Déterminer le Type de Cible
- [x] Vérifier si la cible commence par # ou & → Canal
- [x] Sinon → Pseudonyme d'utilisateur

#### Étape 5a : Message de Canal
Si la cible est un canal :
- [x] Récupérer le canal depuis le serveur
- [x] Si le canal n'existe pas : envoyer ERR_NOSUCHCHANNEL (403)
- [x] Vérifier si le client est membre du canal
- [x] Si pas membre : envoyer ERR_CANNOTSENDTOCHAN (404)
- [x] Formater le message : ":nick!user@host PRIVMSG #channel :message"
- [x] Diffuser au canal, EXCLURE l'expéditeur
  ```cpp
  channel->broadcast(msg, client); // client est exclu
  ```

#### Étape 5b : Message Privé
Si la cible est un pseudonyme :
- [x] Trouver le client cible par pseudonyme
- [x] Si non trouvé : envoyer ERR_NOSUCHNICK (401)
- [x] Formater le message : ":nick!user@host PRIVMSG targetnick :message"
- [x] Envoyer au client cible
  ```cpp
  targetClient->sendMessage(msg);
  ```

---

## Priorité d'Implémentation

### Phase 1 - Messagerie de Base
1. Vérification de l'enregistrement
2. Validation des paramètres (cible et message)
3. Extraction de la cible et du message

### Phase 2 - Messages de Canal
4. Détecter la cible canal (commence par #)
5. Récupérer le canal depuis le serveur
6. Vérifier l'appartenance
7. Diffuser au canal (exclure l'expéditeur)

### Phase 3 - Messages Privés
8. Détecter la cible utilisateur (pas #)
9. Trouver l'utilisateur par pseudonyme
10. Envoyer le message à l'utilisateur

### Phase 4 - Gestion des Erreurs
11. ERR_NOSUCHCHANNEL pour un canal invalide
12. ERR_NOSUCHNICK pour un pseudonyme invalide
13. ERR_CANNOTSENDTOCHAN pour les non-membres

---

## Liste de Vérification des Tests

- [x] PRIVMSG #channel :texte envoie au canal
- [x] PRIVMSG nick :texte envoie à l'utilisateur
- [x] PRIVMSG sans cible envoie l'erreur 411
- [x] PRIVMSG cible (pas de message) envoie l'erreur 412
- [x] PRIVMSG #inexistant envoie l'erreur 403
- [x] PRIVMSG pseudoinconnu envoie l'erreur 401
- [x] PRIVMSG #pasmembre envoie l'erreur 404
- [x] PRIVMSG avant l'enregistrement envoie l'erreur 451
- [x] L'expéditeur ne reçoit pas son propre message de canal
- [x] Le format du message inclut le préfixe de l'expéditeur
- [x] Le message préserve les espaces et la ponctuation

---

## Notes sur le Protocole IRC

### Codes d'Erreur
- **401 ERR_NOSUCHNICK** : "<nickname> :No such nick/channel"
- **403 ERR_NOSUCHCHANNEL** : "<channel> :No such channel"
- **404 ERR_CANNOTSENDTOCHAN** : "<channel> :Cannot send to channel"
- **411 ERR_NORECIPIENT** : ":No recipient given (<command>)"
- **412 ERR_NOTEXTTOSEND** : ":No text to send"
- **451 ERR_NOTREGISTERED** : ":You have not registered"

### Format du Message
```
:sender!user@host PRIVMSG target :message content here
```

### Exemples d'Utilisation

#### Message de Canal
```
Client: PRIVMSG #general :Hello everyone
Server: (à tous les autres dans #general)
        :john!john@host PRIVMSG #general :Hello everyone
```

#### Message Privé
```
Client: PRIVMSG alice :Hi Alice!
Server: (à alice seulement)
        :john!john@host PRIVMSG alice :Hi Alice!
```

#### Erreurs
```
Client: PRIVMSG
Server: :irc.server 411 john :No recipient given (PRIVMSG)

Client: PRIVMSG alice
Server: :irc.server 412 john :No text to send

Client: PRIVMSG #nonexistent :hello
Server: :irc.server 403 john #nonexistent :No such channel

Client: PRIVMSG unknownuser :hello
Server: :irc.server 401 john unknownuser :No such nick/channel
```

---

## Détection de la Cible

### Cible Canal
```cpp
bool isChannel(const std::string& target) {
    return !target.empty() && 
           (target[0] == '#' || target[0] == '&');
}
```

### Cible Utilisateur
- Toute chaîne ne commençant pas par # ou &
- Doit être un pseudonyme valide
- Recherche insensible à la casse

---

## Problèmes Courants

### Problème : L'expéditeur reçoit son propre message
- S'assurer que channel->broadcast() exclut l'expéditeur
- Passer l'expéditeur comme second paramètre à broadcast()
- Ne pas envoyer à l'expéditeur dans les messages privés non plus

### Problème : Format de message incorrect
- Inclure le préfixe complet : ":nick!user@host"
- Utiliser le pseudonyme et le nom d'utilisateur de l'expéditeur
- Ajouter deux-points avant le texte du message

### Problème : Impossible d'envoyer au canal
- Vérifier que le client est membre du canal
- Vérifier channel->isMember(client)
- Seuls les membres peuvent envoyer à la plupart des canaux

---

## Fichiers Associés
- `Server.hpp/cpp` - getChannel(), getClientByNick()
- `Client.hpp/cpp` - sendMessage()
- `Channel.hpp/cpp` - broadcast(), isMember()
- `MessageParser.cpp` - Analyser le paramètre final

---

## Formatage du Message

### Format Complet
```
:nickname!username@hostname PRIVMSG target :message text
```

### Composants
- **Préfixe** : `:nickname!username@hostname`
- **Commande** : `PRIVMSG`
- **Cible** : Canal ou pseudonyme
- **Message** : Après les deux-points, inclut les espaces

### Exemple de Construction
```cpp
std::string formatMessage(Client* sender, const std::string& target, 
                         const std::string& text) {
    return ":" + sender->getNickname() + "!" + 
           sender->getUsername() + "@host PRIVMSG " + 
           target + " :" + text + "\r\n";
}
```

---

## Restrictions des Messages de Canal

### Canaux Réservés aux Membres
- Par défaut : Seuls les membres peuvent envoyer
- Vérifier avec channel->isMember(client)
- Envoyer ERR_CANNOTSENDTOCHAN si pas membre

### Canaux Modérés (+m)
- Seuls les opérateurs et les utilisateurs avec voice peuvent envoyer
- Vérifier le statut d'opérateur : channel->isOperator(client)
- Optionnel : Implémenter le statut voice
- Envoyer ERR_CANNOTSENDTOCHAN si non autorisé

### Utilisateurs Bannis (+b)
- Les utilisateurs correspondant au masque de bannissement ne peuvent pas envoyer
- Fonctionnalité optionnelle
- Vérifier la liste de bannissement avant d'autoriser le message

---

## Fonctionnalités des Messages Privés

### Statut Absent
- Optionnel : Vérifier si la cible est absente
- Envoyer RPL_AWAY (301) à l'expéditeur
- Livrer quand même le message

### Écho du Message
- IRC moderne : Renvoyer le message à l'expéditeur
- IRC traditionnel : Pas d'écho
- Pour ce projet : Pas d'écho nécessaire

---

## Cas Particuliers

### Message Vide
```
PRIVMSG #channel :
```
- Le message est une chaîne vide
- Certains serveurs autorisent, d'autres rejettent
- Recommandation : Envoyer ERR_NOTEXTTOSEND

### Cibles Multiples
```
PRIVMSG #chan1,#chan2,user1 :message
```
- IRC autorise les cibles séparées par des virgules
- Fonctionnalité optionnelle (non requise)
- Si implémenté, envoyer à chaque cible

### Caractères de Contrôle
- IRC autorise les codes de contrôle (couleurs, gras, etc.)
- Format : `\x03` pour la couleur, `\x02` pour le gras
- Le serveur doit les transmettre sans changement
- Ne pas essayer d'interpréter ou de filtrer

---

## Considérations de Performance

### Diffusions de Canal
- Grands canaux → nombreux envois
- Utiliser une itération efficace
- Considérer une file d'attente de messages pour les clients lents
- Ne pas bloquer sur les envois échoués

### Taille du Message
- Limite de message IRC : 512 octets au total
- Incluant préfixe, commande, cible, CRLF
- Texte du message : limite pratique de ~400 octets
- Considérer la troncature ou le rejet des messages longs

---

## Considérations de Sécurité

### Protection contre le Flood
- Limiter les messages par seconde par client
- Implémenter une limitation de débit (optionnel)
- Déconnecter les bots de flood

### Contenu du Message
- Ne pas filtrer ou censurer le contenu
- Transmettre sans changement
- Le serveur est un transporteur, pas un modérateur

### Confidentialité
- Ne pas enregistrer les messages privés
- Ne pas diffuser aux non-membres
- Vérifier l'appartenance avant d'envoyer

---

## Scénarios de Test

### Fonctionnalité de Base
1. Envoyer un message à un canal dans lequel vous êtes
2. Envoyer un message à un autre utilisateur
3. Essayer d'envoyer à un canal dans lequel vous n'êtes pas
4. Essayer d'envoyer à un utilisateur inexistant
5. Essayer d'envoyer à un canal inexistant

### Cas Limites
6. Envoyer un message avec des caractères spéciaux
7. Envoyer un message très long
8. Envoyer un message vide
9. Envoyer à soi-même (message privé)
10. Envoyer avant l'enregistrement complet

### Utilisateurs Multiples
11. Plusieurs utilisateurs dans le canal voient le message
12. L'expéditeur ne voit pas son propre message de canal
13. Message privé uniquement à la cible
14. Message de canal vers un canal vide
