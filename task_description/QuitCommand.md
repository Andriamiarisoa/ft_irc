# Classe QuitCommand - Description de la Tâche

## Aperçu
La classe `QuitCommand` gère la commande QUIT pour se déconnecter du serveur IRC. Elle ferme gracieusement la connexion du client et le retire de tous les canaux.

## Emplacement de la Classe
- **En-tête**: `includes/QuitCommand.hpp`
- **Implémentation**: `src/QuitCommand.cpp`

## Commande IRC
**Syntaxe**: `QUIT [:<message de déconnexion>]`

---

## Méthodes

### Constructeur
```cpp
QuitCommand(Server* srv, Client* cli, const std::vector<std::string>& params)
    : Command(srv, cli, params)
```

**Objectif**: Initialiser la commande QUIT

**TODO**:
- [ ] Appeler le constructeur de la classe de base
- [ ] Aucune initialisation supplémentaire nécessaire

---

### execute()
```cpp
void execute()
```

**Objectif**: Déconnecter le client du serveur

**TODO**:

#### Étape 1: Extraire le Message de Déconnexion
- [ ] Si params[0] existe, l'utiliser comme message de déconnexion
- [ ] Sinon, utiliser un message par défaut
- [ ] Exemple: "QUIT :Leaving" ou "QUIT"
- [ ] Par défaut: "Client quit" ou le surnom du client

#### Étape 2: Diffuser à Tous les Canaux
- [ ] Obtenir tous les canaux dont le client est membre
- [ ] Pour chaque canal:
  - Formater le message QUIT
  - Diffuser à tous les membres du canal
  - Ne pas exclure le client (il devrait aussi le voir)
- [ ] Format du message: ":nick!user@host QUIT :message de déconnexion"
  ```cpp
  std::string quitMsg = ":" + client->getNickname() + "!" + 
                        client->getUsername() + "@host QUIT :" + 
                        message + "\r\n";
  ```

#### Étape 3: Retirer de Tous les Canaux
- [ ] Itérer à travers les canaux du client
- [ ] Appeler channel->removeMember(client) pour chacun
- [ ] Vider l'ensemble des canaux du client

#### Étape 4: Supprimer les Canaux Vides
- [ ] Pour chaque canal dans lequel le client était:
  - Vérifier si le canal est maintenant vide
  - Si vide, supprimer le canal

#### Étape 5: Déconnecter le Client
- [ ] Appeler server->disconnectClient(client->getFd())
- [ ] Cela va:
  - Fermer le socket
  - Retirer de la map des clients
  - Supprimer l'objet Client

#### Étape 6: Note Importante
- [ ] Après disconnectClient(), le pointeur client est INVALIDE
- [ ] Ne pas accéder au client après ce point
- [ ] Retourner immédiatement après la déconnexion

---

## Priorité d'Implémentation

### Phase 1 - QUIT de Base
1. Extraire le message de déconnexion (paramètre optionnel)
2. Appeler server->disconnectClient()

### Phase 2 - Nettoyage des Canaux
3. Obtenir tous les canaux dans lesquels le client se trouve
4. Retirer de chaque canal
5. Supprimer les canaux vides

### Phase 3 - Diffusion
6. Formater le message QUIT
7. Diffuser à chaque canal
8. S'assurer que tous les membres voient le QUIT

### Phase 4 - Gestion des Erreurs
9. Gérer le client déjà déconnecté
10. Gérer les canaux qui n'existent pas
11. Prévenir les bugs d'utilisation après libération

---

## Liste de Vérification des Tests

- [ ] QUIT déconnecte le client
- [ ] QUIT :message inclut un message personnalisé
- [ ] QUIT sans message utilise le message par défaut
- [ ] QUIT diffuse à tous les canaux
- [ ] Client retiré de tous les canaux
- [ ] Canaux vides supprimés après QUIT
- [ ] Socket fermé correctement
- [ ] Pas de fuites mémoire (valgrind)
- [ ] Objet Client supprimé
- [ ] Les autres clients voient le message QUIT

---

## Notes du Protocole IRC

### Format du Message QUIT
```
:nickname!username@host QUIT :message de déconnexion
```

### Exemples d'Utilisation

#### Avec Message
```
Client: QUIT :Going to sleep
Serveur: (à tous les canaux dans lesquels le client était)
         :john!john@host QUIT :Going to sleep
```

#### Sans Message
```
Client: QUIT
Serveur: (à tous les canaux)
         :john!john@host QUIT :Client quit
```

### Pas de Réponses
- Le serveur n'envoie pas de réponse au client qui quitte
- Le client est déconnecté immédiatement
- Le message QUIT est diffusé, pas renvoyé à l'expéditeur

---

## Problèmes Courants

### Problème: Crash d'utilisation après libération
- L'objet Client est supprimé pendant la déconnexion
- Ne pas accéder au pointeur client après disconnectClient()
- Retourner immédiatement après la déconnexion

### Problème: QUIT non vu par les autres
- Diffuser AVANT de déconnecter
- S'assurer que le message est envoyé à tous les canaux
- Ne pas fermer le socket avant l'envoi

### Problème: Fuite mémoire
- S'assurer que l'objet Client est supprimé
- Vérifier que les canaux sont nettoyés
- Supprimer les canaux vides

---

## Fichiers Associés
- `Server.hpp/cpp` - disconnectClient()
- `Client.hpp/cpp` - getChannels()
- `Channel.hpp/cpp` - removeMember(), broadcast()
- `PartCommand.cpp` - Nettoyage similaire des canaux

---

## Ordre de Diffusion

### Critique: Diffuser AVANT de Déconnecter
```cpp
// ORDRE CORRECT:
1. Formater le message QUIT
2. Diffuser à tous les canaux
3. Retirer des canaux
4. Déconnecter le client (supprime l'objet)

// ORDRE INCORRECT:
1. Déconnecter le client (supprime l'objet)  ← Le pointeur Client est maintenant INVALIDE!
2. Essayer d'accéder au client  ← CRASH!
```

---

## Nettoyage des Canaux

### Pour Chaque Canal
```cpp
// Obtenir les canaux avant toute modification
std::set<Channel*> clientChannels = client->getChannels();

// Itérer à travers la copie
for (std::set<Channel*>::iterator it = clientChannels.begin();
     it != clientChannels.end(); ++it) {
    Channel* channel = *it;
    
    // Diffuser QUIT au canal
    std::string quitMsg = ":" + client->getNickname() + "!" +
                          client->getUsername() + "@host QUIT :" +
                          message + "\r\n";
    channel->broadcast(quitMsg, NULL); // Envoyer à tous (y compris celui qui quitte)
    
    // Retirer du canal
    channel->removeMember(client);
    
    // Supprimer si vide
    if (channel->getMemberCount() == 0) {
        server->deleteChannel(channel->getName());
    }
}
```

---

## Implémentation de la Déconnexion

### Dans Server::disconnectClient()
```cpp
void Server::disconnectClient(int fd) {
    // Trouver le client
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;
    
    Client* client = it->second;
    
    // Retirer des descripteurs de fichiers poll
    for (size_t i = 0; i < fds.size(); i++) {
        if (fds[i].fd == fd) {
            fds.erase(fds.begin() + i);
            break;
        }
    }
    
    // Fermer le socket
    close(fd);
    
    // Retirer de la map
    clients.erase(it);
    
    // Supprimer l'objet (CELA DÉTRUIT LE POINTEUR CLIENT!)
    delete client;
}
```

---

## Messages de Déconnexion par Défaut

### Options
- "Client quit"
- "Quit: <nickname>"
- "Leaving"
- Chaîne vide

### Bonne Pratique
```cpp
std::string quitMessage = params.empty() ? "Client quit" : params[0];
```

---

## Cas Particuliers

### QUIT Avant l'Enregistrement
- Le client peut quitter à tout moment
- Même avant PASS/NICK/USER
- Simplement déconnecter, pas besoin de diffusion
- Pas de canaux à nettoyer

### Canaux Multiples
- Le client peut être dans plusieurs canaux
- Diffuser à chacun indépendamment
- Ne pas optimiser en supprimant les messages "dupliqués"
- Chaque canal a besoin de sa propre notification

### Le Dernier Membre Quitte
- Le canal devient vide
- Supprimer l'objet canal
- Retirer de la map des canaux du serveur
- Prévenir les fuites mémoire

---

## Scénarios d'Erreur

### Client Déjà Parti
- Socket déjà fermé
- Objet Client déjà supprimé
- Vérifier si le client existe avant de traiter

### Canal Supprimé Pendant la Boucle
- Le QUIT d'un autre client peut supprimer le canal
- Copier la liste des canaux avant d'itérer
- Vérifier que le canal existe toujours avant d'y accéder

---

## Arrêt du Serveur

### QUIT vs Arrêt du Serveur
- QUIT: Déconnexion initiée par le client
- Arrêt: Fermeture de tous initiée par le serveur

### Arrêt Gracieux
```cpp
void Server::stop() {
    // Copier la liste des clients
    std::map<int, Client*> clientsCopy = clients;
    
    // Déconnecter chaque client
    for (std::map<int, Client*>::iterator it = clientsCopy.begin();
         it != clientsCopy.end(); ++it) {
        // Envoyer le message QUIT
        // Déconnecter le client
    }
}
```

---

## États de Connexion

### Opération Normale
```
Connecté → Authentifié → Enregistré → Dans des Canaux → [QUIT] → Déconnecté
```

### Déconnexion Précoce
```
Connecté → [QUIT] → Déconnecté
Connecté → Authentifié → [QUIT] → Déconnecté
```

### Expulsion par le Serveur
```
Connecté → Enregistré → [QUIT Serveur] → Déconnecté
```

---

## Journalisation

### Quoi Journaliser
- Déconnexion du client (surnom, message de déconnexion)
- Nombre de canaux dans lesquels le client était
- Heure de déconnexion
- Adresse IP (si disponible)

### Quoi Ne Pas Journaliser
- Ne pas journaliser chaque canal individuellement
- Ne pas journaliser les erreurs de socket (trop verbeux)
- Garder les journaux de déconnexion concis

---

## Liste de Vérification de la Gestion Mémoire

- [ ] Objet Client supprimé
- [ ] Client retiré de la map server->clients
- [ ] Client retiré de tous les canaux
- [ ] Canaux vides supprimés
- [ ] Descripteur de fichier socket fermé
- [ ] Poll fd retiré du vecteur fds
- [ ] Aucun pointeur pendant ne reste
- [ ] Pas de fuites mémoire (vérifier avec valgrind)
