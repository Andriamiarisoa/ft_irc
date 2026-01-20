# Classe Client - Description de la Tâche

## Vue d'ensemble
La classe `Client` représente une connexion client IRC. Elle gère l'état du client, l'authentification, l'enregistrement, la mise en mémoire tampon des messages et l'appartenance aux canaux.

## Emplacement de la Classe
- **En-tête**: `includes/Client.hpp`
- **Implémentation**: `src/Client.cpp`

---

## Attributs Privés

### Réseau
- `int fd` - Descripteur de fichier socket pour ce client

### Identité
- `std::string nickname` - Pseudonyme IRC du client
- `std::string username` - Nom d'utilisateur du client
- `std::string buffer` - Tampon de messages incomplets

### État
- `bool authenticated` - Indicateur d'authentification par mot de passe
- `bool registered` - Indicateur d'enregistrement complet (NICK + USER)

### Relations
- `std::set<Channel*> channels` - Ensemble des canaux rejoints par le client

---

## Méthodes

### Constructeur
```cpp
Client(int fd)
```

**Objectif**: Initialiser le client avec le descripteur de socket

**TODO**:
- [x] Stocker fd dans la variable membre
- [x] Initialiser nickname à une chaîne vide
- [x] Initialiser username à une chaîne vide
- [x] Définir authenticated à false
- [x] Définir registered à false
- [x] Initialiser le buffer vide
- [x] Initialiser l'ensemble channels vide

---

### Destructeur
```cpp
~Client()
```

**Objectif**: Nettoyer les ressources du client

**TODO**:
- [x] Vider l'ensemble channels (ne pas supprimer les objets Channel)
- [x] Vider le buffer
- [x] Note: Le socket doit être fermé par la classe Server
- [x] Note: Ne pas fermer fd ici (Server en est propriétaire)

---

### getFd()
```cpp
int getFd() const
```

**Objectif**: Obtenir le descripteur de socket du client

**TODO**:
- [x] Retourner la valeur de fd
- [x] Getter simple, aucune validation nécessaire

---

### getNickname()
```cpp
std::string getNickname() const
```

**Objectif**: Obtenir le pseudonyme du client

**TODO**:
- [x] Retourner la chaîne nickname
- [x] Retourner une chaîne vide si non défini

---

### getUsername()
```cpp
std::string getUsername() const
```

**Objectif**: Obtenir le nom d'utilisateur du client

**TODO**:
- [x] Retourner la chaîne username
- [x] Retourner une chaîne vide si non défini

---

### isAuthenticated()
```cpp
bool isAuthenticated() const
```

**Objectif**: Vérifier si le client a réussi l'authentification par mot de passe

**TODO**:
- [x] Retourner l'indicateur authenticated
- [x] Utilisé pour vérifier que la commande PASS a réussi

---

### isRegistered()
```cpp
bool isRegistered() const
```

**Objectif**: Vérifier si le client est complètement enregistré (NICK + USER)

**TODO**:
- [x] Retourner l'indicateur registered
- [x] Le client doit être authenticated ET avoir un nickname ET un username
- [x] Utilisé pour vérifier si le client peut utiliser les commandes IRC

---

### setNickname(const std::string& nick)
```cpp
void setNickname(const std::string& nick)
```

**Objectif**: Définir le pseudonyme du client

**TODO**:
- [x] Valider le format du pseudonyme (RFC 1459)
  - Doit commencer par une lettre
  - Peut contenir lettres, chiffres, caractères spéciaux (-, [, ], \, `, ^, {, })
  - Maximum 9 caractères
- [x] Stocker nickname dans la variable membre
- [x] Vérifier si le client devient enregistré après avoir défini le pseudonyme
- [ ] Si déjà enregistré, envoyer la réponse NICK aux canaux

---

### setUsername(const std::string& user)
```cpp
void setUsername(const std::string& user)
```

**Objectif**: Définir le nom d'utilisateur du client

**TODO**:
- [x] Stocker username dans la variable membre
- [x] Vérifier si le client devient enregistré après avoir défini le nom d'utilisateur
- [ ] Envoyer les messages de bienvenue (001-004) si nouvellement enregistré
- [x] Marquer l'indicateur registered comme true

---

### authenticate()
```cpp
void authenticate()
```

**Objectif**: Marquer le client comme authentifié par mot de passe

**TODO**:
- [x] Définir l'indicateur authenticated à true
- [x] Appelé après une commande PASS réussie
- [x] Le client a toujours besoin de NICK et USER pour être complètement enregistré

---

### addToChannel(Channel* channel)
```cpp
void addToChannel(Channel* channel)
```

**Objectif**: Ajouter le client à un canal

**TODO**:
- [x] Insérer le pointeur channel dans l'ensemble channels
- [x] Vérifier que channel n'est pas NULL
- [x] Aucune vérification de doublons nécessaire (set le gère)
- [x] Ne pas appeler channel->addMember ici (fait par le canal)

---

### removeFromChannel(Channel* channel)
```cpp
void removeFromChannel(Channel* channel)
```

**Objectif**: Retirer le client d'un canal

**TODO**:
- [x] Effacer le canal de l'ensemble channels
- [x] Vérifier d'abord si le canal existe dans l'ensemble
- [x] Ne pas appeler channel->removeMember ici (fait par le canal)

---

### appendToBuffer(const std::string& data)
```cpp
void appendToBuffer(const std::string& data)
```

**Objectif**: Ajouter les données reçues au tampon de messages

**TODO**:
- [x] Ajouter data à la chaîne buffer
- [x] Vérifier le débordement du tampon (max 512 octets par message)
- [x] Si buffer > 1024 octets, tronquer ou déconnecter
- [x] Pas d'analyse ici, juste accumulation

---

### extractCommand()
```cpp
std::string extractCommand()
```

**Objectif**: Extraire une commande complète du tampon

**TODO**:
- [x] Rechercher "\r\n" dans le buffer
- [x] Si trouvé:
  - Extraire la sous-chaîne du début jusqu'à "\r\n"
  - Retirer la partie extraite du buffer (y compris "\r\n")
  - Retourner la commande extraite
- [x] Si non trouvé:
  - Retourner une chaîne vide (message incomplet)
- [x] Gérer plusieurs "\r\n" (extraire seulement le premier)

---

### sendMessage(const std::string& msg)
```cpp
void sendMessage(const std::string& msg)
```

**Objectif**: Envoyer un message au client

**TODO**:
- [x] S'assurer que le message se termine par "\r\n"
- [x] Appeler send() avec fd et le message
- [x] Gérer les envois partiels (EAGAIN, EWOULDBLOCK)
- [x] Considérer une file d'attente de messages pour les gros messages
- [x] Gérer les erreurs d'envoi avec précaution
- [x] Journaliser les erreurs mais ne pas lancer d'exceptions

---

## Transitions d'État

### Flux d'Authentification
1. **Initial**: `authenticated = false, registered = false`
2. **Après PASS**: `authenticated = true, registered = false`
3. **Après NICK**: `authenticated = true, registered = false` (nécessite USER)
4. **Après USER**: `authenticated = true, registered = true` (si NICK est aussi défini)

### Exigences d'Enregistrement
- Le client doit compléter: PASS → NICK → USER
- L'ordre peut varier mais PASS devrait être en premier
- Seulement après les trois, le client est complètement enregistré

---

## Priorité d'Implémentation

### Phase 1 - Getters/Setters de Base
1. Constructeur/Destructeur
2. getFd()
3. getNickname()
4. getUsername()
5. isAuthenticated()
6. isRegistered()

### Phase 2 - Gestion d'Identité
7. setNickname() - sans validation
8. setUsername()
9. authenticate()

### Phase 3 - Gestion des Canaux
10. addToChannel()
11. removeFromChannel()

### Phase 4 - Gestion des Messages
12. appendToBuffer()
13. extractCommand()
14. sendMessage()

### Phase 5 - Validation & Cas Limites
15. Ajouter la validation du pseudonyme à setNickname()
16. Ajouter la protection contre le débordement du tampon
17. Gérer correctement les erreurs d'envoi

---

## Liste de Vérification des Tests

- [ ] Création d'objet Client avec un fd valide
- [ ] L'état initial est non authentifié et non enregistré
- [ ] authenticate() définit l'indicateur correctement
- [ ] setNickname() stocke le pseudonyme
- [ ] setUsername() stocke le nom d'utilisateur et définit l'indicateur registered
- [ ] Le buffer accumule les données correctement
- [ ] extractCommand() extrait les messages complets
- [ ] extractCommand() gère les messages incomplets
- [ ] sendMessage() envoie les données au socket
- [ ] L'appartenance aux canaux est correctement suivie
- [ ] Pas de fuites mémoire (valgrind)

---

## Notes sur le Protocole IRC

### Règles du Pseudonyme (RFC 1459)
- Premier caractère: lettre (a-z, A-Z)
- Autres caractères: lettres, chiffres, spéciaux (-, [, ], \, `, ^, {, })
- Longueur maximale: 9 caractères
- Comparaison insensible à la casse

### Règles du Nom d'Utilisateur
- Aucun format strict dans l'IRC de base
- Inclut souvent ident/hostname
- Le serveur peut modifier le nom d'utilisateur

### Format des Messages
- Tous les messages se terminent par `\r\n`
- Longueur maximale de message: 512 octets (y compris `\r\n`)
- Les commandes sont insensibles à la casse

---

## Fichiers Associés
- `Server.hpp/cpp` - Gère les connexions clients
- `Channel.hpp/cpp` - Appartenance aux canaux
- `Command.hpp/cpp` - Contexte d'exécution des commandes
- `MessageParser.hpp/cpp` - Analyse les messages des clients
