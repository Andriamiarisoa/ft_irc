# Tutoriel Complet sur select()

## Table des Matières
1. [Introduction](#introduction)
2. [Concepts Fondamentaux](#concepts-fondamentaux)
3. [Prototype et Paramètres](#prototype-et-paramètres)
4. [Les fd_set : Ensembles de Descripteurs](#les-fd_set--ensembles-de-descripteurs)
5. [Macros de Manipulation](#macros-de-manipulation)
6. [Utilisation Basique](#utilisation-basique)
7. [Exemple Simple](#exemple-simple)
8. [Serveur IRC avec select()](#serveur-irc-avec-select)
9. [Gestion des Erreurs](#gestion-des-erreurs)
10. [Avantages et Limitations](#avantages-et-limitations)
11. [Bonnes Pratiques](#bonnes-pratiques)
12. [Comparaison select vs poll](#comparaison-select-vs-poll)

---

## Introduction

`select()` est un appel système qui permet de surveiller plusieurs descripteurs de fichiers (sockets, fichiers, pipes, etc.) pour savoir quand ils sont prêts pour des opérations d'I/O sans bloquer.

### Pourquoi select() ?

Sans `select()`, un serveur devrait :
- Soit bloquer sur un seul client (inacceptable pour un serveur multi-clients)
- Soit utiliser plusieurs threads/processus (complexe)
- Soit faire du polling actif (consomme du CPU inutilement)

**select() résout ce problème** en permettant de surveiller plusieurs sockets simultanément de manière efficace.

---

## Concepts Fondamentaux

### Multiplexage I/O

Le multiplexage I/O permet à un seul thread de gérer plusieurs connexions en surveillant :
- **Lecture** : données disponibles à lire
- **Écriture** : prêt à écrire sans bloquer
- **Exceptions** : conditions exceptionnelles

### Fonctionnement

1. Vous créez des ensembles de descripteurs à surveiller
2. Vous appelez `select()`
3. `select()` bloque jusqu'à ce qu'au moins un descripteur soit prêt
4. `select()` modifie les ensembles pour indiquer lesquels sont prêts
5. Vous traitez les descripteurs prêts

---

## Prototype et Paramètres

### Fichier d'en-tête
```cpp
#include <sys/select.h>
```

### Prototype
```cpp
int select(int nfds,
           fd_set *readfds,
           fd_set *writefds,
           fd_set *exceptfds,
           struct timeval *timeout);
```

### Paramètres Détaillés

#### 1. `nfds` (int)
- **Valeur** : `max_fd + 1` où `max_fd` est le plus grand descripteur à surveiller
- **Pourquoi +1** : `select()` vérifie les descripteurs de 0 à `nfds-1`
- **Exemple** : Si vous surveillez les fd 3, 5, 7 → `nfds = 8`

```cpp
int max_fd = 7;  // Plus grand fd
int nfds = max_fd + 1;  // = 8
```

#### 2. `readfds` (fd_set*)
- **Ensemble de descripteurs** à surveiller pour la **lecture**
- **Input** : descripteurs que vous voulez surveiller
- **Output** : descripteurs prêts à lire (MODIFIÉ par select)
- **Peut être NULL** si vous ne surveillez pas la lecture

**Prêt à lire signifie** :
- Données disponibles sur une socket
- Nouvelle connexion sur une socket d'écoute
- EOF atteint (connexion fermée)

#### 3. `writefds` (fd_set*)
- **Ensemble de descripteurs** à surveiller pour l'**écriture**
- **Output** : descripteurs prêts à écrire (MODIFIÉ par select)
- **Peut être NULL**

**Prêt à écrire signifie** :
- Buffer d'envoi a de l'espace disponible
- Généralement toujours prêt sauf si buffer plein

#### 4. `exceptfds` (fd_set*)
- **Ensemble de descripteurs** à surveiller pour les **exceptions**
- **Output** : descripteurs avec conditions exceptionnelles
- **Rarement utilisé** (peut être NULL)

**Conditions exceptionnelles** :
- Données urgentes (OOB data)
- Erreurs sur socket

#### 5. `timeout` (struct timeval*)
- **Contrôle le temps d'attente** de select()
- **NULL** : bloque indéfiniment jusqu'à événement
- **{0, 0}** : retourne immédiatement (polling)
- **{sec, usec}** : attend au maximum sec secondes et usec microsecondes

```cpp
struct timeval {
    time_t      tv_sec;   // secondes
    suseconds_t tv_usec;  // microsecondes
};
```

### Valeur de Retour

```cpp
int result = select(nfds, &readfds, &writefds, NULL, &timeout);
```

- **> 0** : Nombre total de descripteurs prêts dans tous les ensembles
- **0** : Timeout expiré, aucun descripteur prêt
- **-1** : Erreur (vérifier `errno`)

---

## Les fd_set : Ensembles de Descripteurs

### Qu'est-ce qu'un fd_set ?

Un `fd_set` est une structure opaque qui représente un **ensemble de descripteurs de fichiers**.

**Important** : Ne manipulez JAMAIS directement un `fd_set`. Utilisez toujours les macros fournies.

### Limite : FD_SETSIZE

```cpp
#define FD_SETSIZE 1024  // Sur la plupart des systèmes
```

- `select()` peut surveiller jusqu'à `FD_SETSIZE` descripteurs
- Les descripteurs doivent être **< FD_SETSIZE**
- Si vous avez fd = 1025, `select()` ne peut pas le surveiller

---

## Macros de Manipulation

### FD_ZERO : Vider l'ensemble
```cpp
void FD_ZERO(fd_set *set);
```

**Utilisation** : Initialise un ensemble vide

```cpp
fd_set readfds;
FD_ZERO(&readfds);  // TOUJOURS faire avant d'utiliser un fd_set
```

**⚠️ CRITIQUE** : Appelez TOUJOURS `FD_ZERO()` avant la première utilisation d'un `fd_set`.

---

### FD_SET : Ajouter un descripteur
```cpp
void FD_SET(int fd, fd_set *set);
```

**Utilisation** : Ajoute `fd` à l'ensemble

```cpp
int server_socket = 3;
int client_socket = 5;

FD_SET(server_socket, &readfds);  // Surveiller socket serveur
FD_SET(client_socket, &readfds);  // Surveiller socket client
```

---

### FD_CLR : Retirer un descripteur
```cpp
void FD_CLR(int fd, fd_set *set);
```

**Utilisation** : Retire `fd` de l'ensemble

```cpp
// Client déconnecté, ne plus le surveiller
FD_CLR(client_socket, &readfds);
close(client_socket);
```

---

### FD_ISSET : Tester un descripteur
```cpp
int FD_ISSET(int fd, fd_set *set);
```

**Utilisation** : Vérifie si `fd` est dans l'ensemble (prêt après select)

```cpp
if (FD_ISSET(client_socket, &readfds)) {
    // Ce socket a des données à lire
    recv(client_socket, buffer, sizeof(buffer), 0);
}
```

**Retourne** :
- **Non-zéro** si `fd` est dans l'ensemble
- **0** si `fd` n'est pas dans l'ensemble

---

## Utilisation Basique

### Schéma d'Utilisation Standard

```cpp
// 1. Créer et initialiser les ensembles
fd_set readfds;
FD_ZERO(&readfds);

// 2. Ajouter les descripteurs à surveiller
FD_SET(server_socket, &readfds);
FD_SET(client_socket, &readfds);

// 3. Calculer nfds
int max_fd = (server_socket > client_socket) ? server_socket : client_socket;
int nfds = max_fd + 1;

// 4. Définir le timeout (optionnel)
struct timeval timeout;
timeout.tv_sec = 5;
timeout.tv_usec = 0;

// 5. Appeler select()
int ret = select(nfds, &readfds, NULL, NULL, &timeout);

// 6. Vérifier le résultat
if (ret == -1) {
    perror("select");
    // Gérer l'erreur
} else if (ret == 0) {
    // Timeout
    printf("Aucune activité pendant 5 secondes\n");
} else {
    // 7. Vérifier quels descripteurs sont prêts
    if (FD_ISSET(server_socket, &readfds)) {
        // Nouvelle connexion
        accept_new_connection();
    }
    if (FD_ISSET(client_socket, &readfds)) {
        // Données du client
        receive_client_data();
    }
}
```

### ⚠️ Point Crucial : select() MODIFIE les fd_set

**IMPORTANT** : Après `select()`, les ensembles contiennent SEULEMENT les descripteurs prêts.

```cpp
// AVANT select()
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(3, &readfds);  // readfds contient {3, 5, 7}
FD_SET(5, &readfds);
FD_SET(7, &readfds);

select(8, &readfds, NULL, NULL, NULL);

// APRÈS select() : readfds modifié !
// Si seul fd 5 est prêt : readfds contient {5}
// Les fd 3 et 7 ont été retirés !
```

**Solution** : Toujours reconstruire les ensembles avant chaque `select()`

```cpp
while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    
    // Reconstruire l'ensemble à chaque itération
    for (int fd : all_sockets) {
        FD_SET(fd, &readfds);
    }
    
    select(nfds, &readfds, NULL, NULL, NULL);
    
    // Traiter...
}
```

---

## Exemple Simple

### Serveur Echo Simple avec select()

```cpp
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>

int main() {
    // 1. Créer socket serveur
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Réutiliser l'adresse
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind sur port 8080
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
    
    // Écouter
    listen(server_socket, 10);
    printf("Serveur en écoute sur port 8080...\n");
    
    // Liste des clients connectés
    std::vector<int> clients;
    
    // 2. Boucle principale
    while (1) {
        // 3. Préparer l'ensemble des descripteurs
        fd_set readfds;
        FD_ZERO(&readfds);
        
        // Ajouter socket serveur
        FD_SET(server_socket, &readfds);
        int max_fd = server_socket;
        
        // Ajouter tous les clients
        for (int client_fd : clients) {
            FD_SET(client_fd, &readfds);
            if (client_fd > max_fd)
                max_fd = client_fd;
        }
        
        // 4. Appeler select()
        printf("En attente d'activité...\n");
        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (ret == -1) {
            perror("select");
            break;
        }
        
        // 5. Vérifier socket serveur (nouvelle connexion)
        if (FD_ISSET(server_socket, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_socket, 
                                   (struct sockaddr*)&client_addr,
                                   &client_len);
            
            if (client_fd != -1) {
                printf("Nouveau client connecté: fd=%d\n", client_fd);
                clients.push_back(client_fd);
            }
        }
        
        // 6. Vérifier tous les clients
        for (auto it = clients.begin(); it != clients.end(); ) {
            int client_fd = *it;
            
            if (FD_ISSET(client_fd, &readfds)) {
                // Client a envoyé des données
                char buffer[1024];
                int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes <= 0) {
                    // Client déconnecté
                    printf("Client déconnecté: fd=%d\n", client_fd);
                    close(client_fd);
                    it = clients.erase(it);
                } else {
                    // Echo : renvoyer les données
                    buffer[bytes] = '\0';
                    printf("Reçu de fd=%d: %s", client_fd, buffer);
                    send(client_fd, buffer, bytes, 0);
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
    
    // Nettoyage
    close(server_socket);
    for (int client_fd : clients) {
        close(client_fd);
    }
    
    return 0;
}
```

### Compilation et Test

```bash
# Compiler
g++ -std=c++98 server.cpp -o server

# Lancer le serveur
./server

# Dans un autre terminal, tester avec netcat
nc localhost 8080
Hello
Hello        # Echo du serveur
```

---

## Serveur IRC avec select()

### Architecture Recommandée

```cpp
class Server {
private:
    int serverSocket;
    std::map<int, Client*> clients;
    std::map<std::string, Channel*> channels;
    
public:
    void run() {
        while (true) {
            fd_set readfds, writefds;
            int max_fd = prepareSelectSets(readfds, writefds);
            
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            
            int ret = select(max_fd + 1, &readfds, &writefds, NULL, &timeout);
            
            if (ret > 0) {
                handleSelect(readfds, writefds);
            }
        }
    }
    
private:
    int prepareSelectSets(fd_set& readfds, fd_set& writefds) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        
        // Socket serveur : toujours surveiller pour nouvelles connexions
        FD_SET(serverSocket, &readfds);
        int max_fd = serverSocket;
        
        // Tous les clients : surveiller lecture
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it) {
            int fd = it->first;
            Client* client = it->second;
            
            // Surveiller lecture
            FD_SET(fd, &readfds);
            
            // Surveiller écriture si données en attente
            if (client->hasPendingData()) {
                FD_SET(fd, &writefds);
            }
            
            if (fd > max_fd)
                max_fd = fd;
        }
        
        return max_fd;
    }
    
    void handleSelect(fd_set& readfds, fd_set& writefds) {
        // 1. Nouvelle connexion ?
        if (FD_ISSET(serverSocket, &readfds)) {
            acceptNewConnection();
        }
        
        // 2. Clients existants
        std::vector<int> toRemove;
        
        for (std::map<int, Client*>::iterator it = clients.begin();
             it != clients.end(); ++it) {
            int fd = it->first;
            Client* client = it->second;
            
            // Données à lire ?
            if (FD_ISSET(fd, &readfds)) {
                if (!handleClientRead(fd, client)) {
                    toRemove.push_back(fd);
                    continue;
                }
            }
            
            // Prêt à écrire ?
            if (FD_ISSET(fd, &writefds)) {
                handleClientWrite(fd, client);
            }
        }
        
        // 3. Supprimer clients déconnectés
        for (size_t i = 0; i < toRemove.size(); ++i) {
            removeClient(toRemove[i]);
        }
    }
    
    void acceptNewConnection() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket,
                                  (struct sockaddr*)&clientAddr,
                                  &clientLen);
        
        if (clientSocket == -1) {
            perror("accept");
            return;
        }
        
        // Non-bloquant
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        
        // Créer objet Client
        Client* client = new Client(clientSocket);
        clients[clientSocket] = client;
        
        std::cout << "Nouvelle connexion: fd=" << clientSocket << std::endl;
    }
    
    bool handleClientRead(int fd, Client* client) {
        char buffer[512];
        int bytes = recv(fd, buffer, sizeof(buffer), 0);
        
        if (bytes <= 0) {
            // Client déconnecté ou erreur
            return false;
        }
        
        // Ajouter au buffer du client
        client->appendToBuffer(buffer, bytes);
        
        // Traiter les commandes complètes
        while (client->hasCompleteCommand()) {
            std::string command = client->extractCommand();
            processCommand(client, command);
        }
        
        return true;
    }
    
    void handleClientWrite(int fd, Client* client) {
        client->flushPendingData(fd);
    }
};
```

### Gestion du Buffer par Client

```cpp
class Client {
private:
    std::string recvBuffer;
    std::string sendBuffer;
    
public:
    void appendToBuffer(const char* data, size_t len) {
        recvBuffer.append(data, len);
    }
    
    bool hasCompleteCommand() {
        return recvBuffer.find("\r\n") != std::string::npos;
    }
    
    std::string extractCommand() {
        size_t pos = recvBuffer.find("\r\n");
        std::string command = recvBuffer.substr(0, pos);
        recvBuffer.erase(0, pos + 2);
        return command;
    }
    
    void sendMessage(const std::string& msg) {
        sendBuffer += msg;
    }
    
    bool hasPendingData() const {
        return !sendBuffer.empty();
    }
    
    void flushPendingData(int fd) {
        if (sendBuffer.empty())
            return;
        
        int sent = send(fd, sendBuffer.c_str(), sendBuffer.size(), 0);
        if (sent > 0) {
            sendBuffer.erase(0, sent);
        }
    }
};
```

---

## Gestion des Erreurs

### Erreurs Courantes

#### EBADF (Bad file descriptor)
```cpp
if (ret == -1 && errno == EBADF) {
    // Un des descripteurs est invalide
    // Vérifier que tous les fd sont ouverts
}
```

#### EINTR (Interrupted by signal)
```cpp
if (ret == -1 && errno == EINTR) {
    // Signal reçu pendant select()
    // Généralement, réessayer
    continue;
}
```

#### EINVAL (Invalid argument)
```cpp
if (ret == -1 && errno == EINVAL) {
    // nfds négatif ou timeout invalide
}
```

### Gestion Robuste

```cpp
int result;
do {
    result = select(max_fd + 1, &readfds, &writefds, NULL, &timeout);
} while (result == -1 && errno == EINTR);

if (result == -1) {
    perror("select");
    // Gérer l'erreur
}
```

---

## Avantages et Limitations

### ✅ Avantages

1. **Portable** : Disponible sur tous les systèmes UNIX/Linux
2. **Simple** : API facile à comprendre
3. **Standard** : POSIX.1-2001
4. **Efficace** pour petit nombre de descripteurs

### ❌ Limitations

1. **FD_SETSIZE** : Limité à 1024 descripteurs (généralement)
   - Problème pour serveurs à grande échelle
   
2. **Performance O(n)** : `select()` doit scanner tous les fd
   - Inefficace avec beaucoup de descripteurs
   
3. **Modification des ensembles** : Doit reconstruire à chaque appel
   - Overhead supplémentaire

4. **nfds** : Doit calculer le max_fd à chaque fois

### Quand Utiliser select() ?

**Utilisez select() si** :
- Petit nombre de connexions (< 100)
- Portabilité importante
- Simplicité recherchée

**Préférez poll() ou epoll() si** :
- Beaucoup de connexions (> 1000)
- Performance critique
- Linux uniquement (epoll)

---

## Bonnes Pratiques

### 1. Toujours Utiliser FD_ZERO

```cpp
// ✅ BON
fd_set readfds;
FD_ZERO(&readfds);  // Initialisation
FD_SET(fd, &readfds);

// ❌ MAUVAIS
fd_set readfds;  // Non initialisé !
FD_SET(fd, &readfds);
```

### 2. Reconstruire les Ensembles

```cpp
// ✅ BON
while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    // Reconstruire à chaque itération
    for (int fd : all_fds) {
        FD_SET(fd, &readfds);
    }
    select(...);
}

// ❌ MAUVAIS
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd, &readfds);
while (1) {
    select(...);  // readfds modifié ! Bogué !
}
```

### 3. Vérifier max_fd

```cpp
// ✅ BON
int max_fd = -1;
for (int fd : all_fds) {
    if (fd > max_fd)
        max_fd = fd;
}
select(max_fd + 1, ...);

// ❌ MAUVAIS
select(FD_SETSIZE, ...);  // Inefficace
```

### 4. Gérer les Timeouts

```cpp
// ✅ BON - Timeout défini
struct timeval timeout;
timeout.tv_sec = 1;
timeout.tv_usec = 0;
select(nfds, &readfds, NULL, NULL, &timeout);

// ⚠️ ATTENTION - Bloque indéfiniment
select(nfds, &readfds, NULL, NULL, NULL);
```

### 5. Sockets Non-Bloquants

```cpp
// Rendre socket non-bloquant
#include <fcntl.h>
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

**Pourquoi ?** :
- `recv()` ne bloquera jamais
- `send()` retourne immédiatement si buffer plein
- Meilleur contrôle

### 6. Vérifier Valeur de Retour

```cpp
// ✅ BON
int ret = select(nfds, &readfds, NULL, NULL, &timeout);
if (ret == -1) {
    perror("select");
} else if (ret == 0) {
    // Timeout
} else {
    // Traiter
}

// ❌ MAUVAIS
select(nfds, &readfds, NULL, NULL, &timeout);
// Ne vérifie pas ret !
```

---

## Comparaison select vs poll

### select()

```cpp
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd1, &readfds);
FD_SET(fd2, &readfds);
select(max_fd + 1, &readfds, NULL, NULL, NULL);
if (FD_ISSET(fd1, &readfds)) { /* ... */ }
```

**Avantages** :
- Standard POSIX ancien (très portable)
- Simple pour petits nombres

**Inconvénients** :
- Limité à FD_SETSIZE (1024)
- Doit calculer max_fd
- Ensembles modifiés (reconstruction nécessaire)

---

### poll()

```cpp
struct pollfd fds[2];
fds[0].fd = fd1;
fds[0].events = POLLIN;
fds[1].fd = fd2;
fds[1].events = POLLIN;
poll(fds, 2, -1);
if (fds[0].revents & POLLIN) { /* ... */ }
```

**Avantages** :
- Pas de limite FD_SETSIZE
- Pas besoin de calculer max_fd
- Tableau non modifié (events vs revents)

**Inconvénients** :
- Moins portable que select() (mais standard POSIX.1-2001)
- Performance O(n) aussi

---

### Tableau Comparatif

| Critère              | select()        | poll()          |
|----------------------|-----------------|-----------------|
| Limite descripteurs  | FD_SETSIZE      | Illimitée       |
| Calcul max_fd        | Oui             | Non             |
| Reconstruction       | Oui             | Non             |
| Portabilité          | +++             | ++              |
| Performance          | O(n)            | O(n)            |
| Facilité            | ++              | +++             |

---

## Exemple Complet : Serveur IRC Minimal

```cpp
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

class IRCServer {
private:
    int serverSocket;
    std::map<int, std::string> clientBuffers;
    
public:
    IRCServer(int port) {
        // Créer socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("socket");
            exit(1);
        }
        
        // Réutiliser adresse
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // Non-bloquant
        fcntl(serverSocket, F_SETFL, O_NONBLOCK);
        
        // Bind
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("bind");
            exit(1);
        }
        
        // Listen
        if (listen(serverSocket, 10) == -1) {
            perror("listen");
            exit(1);
        }
        
        std::cout << "Serveur IRC démarré sur port " << port << std::endl;
    }
    
    void run() {
        while (true) {
            fd_set readfds;
            FD_ZERO(&readfds);
            
            // Ajouter socket serveur
            FD_SET(serverSocket, &readfds);
            int max_fd = serverSocket;
            
            // Ajouter tous les clients
            for (std::map<int, std::string>::iterator it = clientBuffers.begin();
                 it != clientBuffers.end(); ++it) {
                int fd = it->first;
                FD_SET(fd, &readfds);
                if (fd > max_fd)
                    max_fd = fd;
            }
            
            // Timeout 1 seconde
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            
            // Select
            int ret = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
            
            if (ret == -1) {
                if (errno == EINTR)
                    continue;
                perror("select");
                break;
            }
            
            if (ret == 0) {
                // Timeout - on peut faire du housekeeping ici
                continue;
            }
            
            // Nouvelle connexion ?
            if (FD_ISSET(serverSocket, &readfds)) {
                acceptNewClient();
            }
            
            // Clients existants
            std::vector<int> toRemove;
            for (std::map<int, std::string>::iterator it = clientBuffers.begin();
                 it != clientBuffers.end(); ++it) {
                int fd = it->first;
                
                if (FD_ISSET(fd, &readfds)) {
                    if (!handleClientData(fd)) {
                        toRemove.push_back(fd);
                    }
                }
            }
            
            // Supprimer clients déconnectés
            for (size_t i = 0; i < toRemove.size(); ++i) {
                removeClient(toRemove[i]);
            }
        }
    }
    
private:
    void acceptNewClient() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientFd = accept(serverSocket,
                             (struct sockaddr*)&clientAddr,
                             &clientLen);
        
        if (clientFd == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("accept");
            }
            return;
        }
        
        // Non-bloquant
        fcntl(clientFd, F_SETFL, O_NONBLOCK);
        
        // Ajouter à la liste
        clientBuffers[clientFd] = "";
        
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
        std::cout << "Nouvelle connexion de " << ip 
                  << " (fd=" << clientFd << ")" << std::endl;
        
        // Message de bienvenue
        std::string welcome = ":server NOTICE AUTH :*** Bienvenue sur le serveur IRC\r\n";
        send(clientFd, welcome.c_str(), welcome.length(), 0);
    }
    
    bool handleClientData(int fd) {
        char buffer[512];
        int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            // Déconnexion ou erreur
            if (bytes == 0) {
                std::cout << "Client déconnecté (fd=" << fd << ")" << std::endl;
            } else {
                perror("recv");
            }
            return false;
        }
        
        buffer[bytes] = '\0';
        clientBuffers[fd] += buffer;
        
        // Traiter les commandes complètes
        size_t pos;
        while ((pos = clientBuffers[fd].find("\r\n")) != std::string::npos) {
            std::string command = clientBuffers[fd].substr(0, pos);
            clientBuffers[fd].erase(0, pos + 2);
            
            std::cout << "Client " << fd << ": " << command << std::endl;
            
            // Traiter la commande
            processCommand(fd, command);
        }
        
        return true;
    }
    
    void processCommand(int fd, const std::string& command) {
        // Parsing simple
        if (command.substr(0, 4) == "PING") {
            std::string pong = "PONG " + command.substr(5) + "\r\n";
            send(fd, pong.c_str(), pong.length(), 0);
        } else if (command.substr(0, 4) == "QUIT") {
            removeClient(fd);
        } else {
            // Echo pour test
            std::string response = ":server NOTICE client :Commande reçue: " 
                                 + command + "\r\n";
            send(fd, response.c_str(), response.length(), 0);
        }
    }
    
    void removeClient(int fd) {
        close(fd);
        clientBuffers.erase(fd);
        std::cout << "Client supprimé (fd=" << fd << ")" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    int port = 6667;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    IRCServer server(port);
    server.run();
    
    return 0;
}
```

### Compilation et Test

```bash
# Compiler
g++ -std=c++98 -Wall -Wextra -Werror irc_server.cpp -o ircserv

# Lancer
./ircserv 6667

# Tester avec netcat
nc localhost 6667
```

---

## Résumé des Points Clés

### 📌 À Retenir Absolument

1. **FD_ZERO** : Toujours initialiser avec `FD_ZERO()` avant utilisation
2. **Reconstruction** : Reconstruire les fd_set à chaque itération
3. **nfds** : Toujours `max_fd + 1`
4. **FD_ISSET** : Tester avec `FD_ISSET()` après select
5. **Modification** : Les fd_set sont MODIFIÉS par select()
6. **Non-bloquant** : Utiliser O_NONBLOCK sur les sockets
7. **Timeout** : NULL = bloque, {0,0} = polling, {n,m} = attente
8. **Limite** : FD_SETSIZE (généralement 1024)

### 🔄 Workflow Typique

```
1. FD_ZERO(&readfds)
2. FD_SET(fd1, &readfds)
3. FD_SET(fd2, &readfds)
4. max_fd = max(fd1, fd2)
5. select(max_fd + 1, &readfds, ...)
6. if (FD_ISSET(fd1, &readfds)) { traiter fd1 }
7. if (FD_ISSET(fd2, &readfds)) { traiter fd2 }
8. Retour à l'étape 1
```

---

## Ressources Supplémentaires

### Pages Man
```bash
man 2 select
man 2 FD_ZERO
man 2 socket
man 2 fcntl
```

### Standards
- POSIX.1-2001
- POSIX.1-2008
- SUSv2

### Alternatives
- **poll()** : Similaire mais sans limite FD_SETSIZE
- **epoll()** : Linux-only, très performant (O(1))
- **kqueue()** : BSD-only, équivalent d'epoll

---

## Conclusion

`select()` est un outil puissant pour le multiplexage I/O. Bien qu'il ait des limitations (FD_SETSIZE, performance), il reste très utilisé pour :
- Serveurs avec peu de connexions
- Applications portables
- Apprentissage du multiplexage I/O

Pour votre projet IRC ft_irc, `select()` est **parfaitement adapté** car :
- Nombre de connexions raisonnable
- Portabilité importante
- Simplicité d'implémentation

**Points critiques à ne pas oublier** :
1. Toujours `FD_ZERO()` avant utilisation
2. Reconstruire les fd_set à chaque boucle
3. Utiliser `FD_ISSET()` pour tester
4. Gérer correctement les déconnexions
5. Sockets en mode non-bloquant

Bon courage pour votre implémentation ! 🚀
