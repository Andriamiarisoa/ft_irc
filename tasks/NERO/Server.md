# Classe Server - Description de la Tâche

## Vue d'ensemble
La classe `Server` est le cœur de l'implémentation du serveur IRC. Elle gère les connexions réseau, les sessions client, les canaux et coordonne toutes les opérations du protocole IRC.

## Emplacement de la Classe
- **En-tête** : `includes/Server.hpp`
- **Implémentation** : `src/Server.cpp`

---

## Attributs Privés

### Configuration Réseau
- `int port` - Port d'écoute du serveur
- `std::string password` - Mot de passe du serveur pour l'authentification
- `int serverSocket` - Descripteur de fichier du socket principal du serveur
- `bool running` - Indicateur d'état du serveur

### Gestion des Clients et Canaux
- `std::map<int, Client*> clients` - Carte de fd → pointeur Client
- `std::map<std::string, Channel*> channels` - Carte de nom de canal → pointeur Channel

---

## Méthodes

### Constructeur
```cpp
Server(int port, const std::string& password)
```

**Objectif** : Initialiser le serveur avec le port et le mot de passe

**TODO** :
- [x] Stocker le port et le mot de passe dans les variables membres
- [x] Initialiser serverSocket à -1
- [x] Définir l'indicateur running à false
- [x] Initialiser la map clients vide
- [x] Initialiser la map channels vide

---

### Destructeur
```cpp
~Server()
```

**Objectif** : Nettoyer toutes les ressources

**TODO** :
- [x] Fermer toutes les connexions client (itérer à travers la map clients)
- [x] Supprimer tous les objets Client (libérer la mémoire)
- [x] Supprimer tous les objets Channel (libérer la mémoire)
- [x] Fermer le socket serveur s'il est ouvert (serverSocket >= 0)
- [x] Vider tous les conteneurs (clients, channels)

---

### start()
```cpp
void start()
```

**Objectif** : Démarrer le serveur et entrer dans la boucle d'événements principale

**TODO** :
- [x] Appeler setupSocket() pour initialiser le socket serveur
- [x] Définir l'indicateur running à true
- [x] Entrer dans la boucle principale while(running)
- [x] Appeler handleSelect() à chaque itération
- [x] Gérer Ctrl+C proprement (considérer les gestionnaires de signaux)
- [x] Appeler stop() à la sortie

---

### stop()
```cpp
void stop()
```

**Objectif** : Arrêter le serveur proprement

**TODO** :
- [x] Définir l'indicateur running à false
- [x] Envoyer un message de déconnexion à tous les clients connectés
- [x] Fermer toutes les connexions client
- [x] Fermer le socket serveur
- [x] Enregistrer un message d'arrêt

---

### setupSocket()
```cpp
void setupSocket()
```

**Objectif** : Créer et configurer le socket serveur

**TODO** :
- [x] Créer le socket avec socket(AF_INET, SOCK_STREAM, 0)
- [x] Vérifier les erreurs de création de socket
- [x] Définir l'option de socket SO_REUSEADDR avec setsockopt()
- [x] Définir le socket en mode non-bloquant avec fcntl()
- [x] Créer la structure sockaddr_in avec le port et INADDR_ANY
- [x] Lier le socket à l'adresse avec bind()
- [x] Vérifier les erreurs de bind (port déjà utilisé, etc.)
- [x] Commencer l'écoute avec listen() (backlog = 10)
- [x] Enregistrer un message de succès avec le numéro de port

---

### handleSelect()
```cpp
void handleSelect()
```

**Objectif** : Gérer les événements I/O en utilisant select()

**TODO** :
- [x] Créer et initialiser fd_set readfds avec FD_ZERO()
- [x] Ajouter le socket serveur avec FD_SET(serverSocket, &readfds)
- [x] Itérer à travers tous les clients et les ajouter avec FD_SET()
- [x] Calculer max_fd (le plus grand descripteur de fichier)
- [x] Définir un timeout (struct timeval) pour éviter un blocage infini
- [x] Appeler select(max_fd + 1, &readfds, NULL, NULL, &timeout)
- [x] Vérifier la valeur de retour de select() pour les erreurs
- [x] Si FD_ISSET(serverSocket, &readfds) : appeler acceptNewClient()
- [x] Pour chaque client, si FD_ISSET(client_fd, &readfds) : appeler handleClientMessage(fd)
- [x] Gérer les cas limites (EINTR, lectures partielles)

---

### acceptNewClient()
```cpp
void acceptNewClient()
```

**Objectif** : Accepter une nouvelle connexion client

**TODO** :
- [x] Appeler accept() sur le socket serveur
- [x] Vérifier les erreurs d'accept
- [x] Définir le nouveau socket en mode non-bloquant avec fcntl()
- [x] Créer un nouvel objet Client avec fd
- [x] Ajouter le client à la map clients (clé = fd)
- [x] Enregistrer la nouvelle connexion (adresse IP si possible)
- [x] Envoyer un message de bienvenue au client

---

### handleClientMessage(int fd)
```cpp
void handleClientMessage(int fd)
```

**Objectif** : Lire et traiter les messages du client

**TODO** :
- [x] Trouver le client dans la map clients par fd
- [x] Lire les données du socket avec recv()
- [x] Gérer les erreurs de recv (EAGAIN, EWOULDBLOCK, erreurs)
- [x] Si recv retourne 0 : client déconnecté, appeler disconnectClient()
- [x] Ajouter les données reçues au buffer du client
- [x] Extraire les commandes complètes (chercher \r\n)
- [x] Pour chaque commande complète : appeler executeCommand()
- [x] Gérer les messages partiels (garder dans le buffer)

---

### disconnectClient(int fd)
```cpp
void disconnectClient(int fd)
```

**Objectif** : Déconnecter le client et nettoyer

**TODO** :
- [x] Trouver le client dans la map clients
- [x] Retirer le client de tous les canaux (itérer les canaux du client)
- [x] Diffuser le message QUIT aux canaux
- [x] Fermer le socket avec close(fd)
- [x] Supprimer l'objet Client
- [x] Retirer de la map clients
- [x] Enregistrer la déconnexion

---

### getClientByNick(const std::string& nick)
```cpp
Client* getClientByNick(const std::string& nick)
```

**Objectif** : Trouver un client par son pseudonyme

**TODO** :
- [x] Itérer à travers la map clients
- [x] Comparer le pseudonyme de chaque client avec le paramètre
- [x] Retourner le pointeur si trouvé
- [x] Retourner NULL si non trouvé
- [x] Considérer la comparaison insensible à la casse (standard IRC)

---

### getOrCreateChannel(const std::string& name)
```cpp
Channel* getOrCreateChannel(const std::string& name)
```

**Objectif** : Obtenir un canal existant ou en créer un nouveau

**TODO** :
- [x] Rechercher le canal dans la map channels
- [x] Si trouvé : retourner le pointeur du canal existant
- [x] Si non trouvé : créer un nouvel objet Channel avec le nom
- [x] Ajouter le nouveau canal à la map channels
- [x] Retourner le pointeur du nouveau canal
- [x] Valider le format du nom du canal (#channel)

---

### executeCommand(Client* client, const std::string& cmd)
```cpp
void executeCommand(Client* client, const std::string& cmd)
```

**Objectif** : Analyser et exécuter une commande IRC

**TODO** :
- [ ] Appeler MessageParser::parse(cmd, this, client)
- [ ] Vérifier si l'objet Command a été créé (NULL = invalide)
- [ ] Appeler command->execute()
- [ ] Supprimer l'objet command (nettoyage)
- [ ] Gérer les exceptions de l'exécution de commande
- [ ] Enregistrer l'exécution de commande (mode debug)

---

## Priorité d'Implémentation

### Phase 1 - Configuration de Base
1. Constructeur/Destructeur
2. setupSocket()
3. start() - boucle basique
4. stop()

### Phase 2 - Gestion des Connexions
5. acceptNewClient()
6. handleSelect() - select basique
7. disconnectClient()

### Phase 3 - Traitement des Messages
8. handleClientMessage()
9. executeCommand()

### Phase 4 - Méthodes Auxiliaires
10. getClientByNick()
11. getOrCreateChannel()

---

## Liste de Vérification des Tests

- [ ] Le serveur démarre sur le port spécifié
- [ ] Le serveur rejette les connexions sur le mauvais port
- [ ] Plusieurs clients peuvent se connecter simultanément
- [ ] Le serveur gère les déconnexions client proprement
- [ ] Le serveur ne plante pas sur une entrée mal formée
- [ ] Fuites mémoire vérifiées avec valgrind
- [ ] Le serveur répond correctement à Ctrl+C
- [ ] Le timeout de select ne cause pas une utilisation CPU à 100%

---

## Fichiers Associés
- `Client.hpp/cpp` - Gestion de l'état du client
- `Channel.hpp/cpp` - Opérations de canal
- `MessageParser.hpp/cpp` - Analyse des commandes
- `Command.hpp/cpp` - Exécution des commandes
