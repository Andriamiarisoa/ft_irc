# Classe MessageParser - Description de la Tâche

## Vue d'ensemble
La classe `MessageParser` est une classe utilitaire avec des méthodes statiques pour analyser les messages du protocole IRC et créer les objets Command appropriés. Elle agit comme une fabrique (factory) pour les objets Command.

## Emplacement de la Classe
- **En-tête**: `includes/MessageParser.hpp`
- **Implémentation**: `src/MessageParser.cpp`

---

## Méthodes Statiques

### parse(const std::string& line, Server* srv, Client* cli)
```cpp
static Command* parse(const std::string& line, Server* srv, Client* cli)
```

**Objectif**: Analyser un message IRC et créer l'objet Command correspondant

**TODO**:
- [x] Extraire le préfixe s'il est présent (commence par :)
- [x] Extraire le mot de commande (premier mot après le préfixe)
- [x] Extraire les paramètres (mots restants)
- [x] Convertir la commande en majuscules pour la comparaison
- [x] Créer et retourner l'objet Command approprié basé sur le mot de commande
- [x] Retourner NULL pour les commandes inconnues/invalides

**Étapes d'Analyse**:
1. Vérifier si la ligne commence par ':' (préfixe)
   - Si oui: extraire le préfixe et le retirer de la ligne
2. Diviser la ligne par des espaces
3. Le premier mot est la commande
4. Les mots restants sont les paramètres
5. Gérer le paramètre final (commence par ':')
6. Créer la sous-classe Command appropriée

**Mappage des Commandes**:
- "PASS" → new PassCommand(srv, cli, params)
- "NICK" → new NickCommand(srv, cli, params)
- "USER" → new UserCommand(srv, cli, params)
- "JOIN" → new JoinCommand(srv, cli, params)
- "PART" → new PartCommand(srv, cli, params)
- "PRIVMSG" → new PrivmsgCommand(srv, cli, params)
- "KICK" → new KickCommand(srv, cli, params)
- "INVITE" → new InviteCommand(srv, cli, params)
- "TOPIC" → new TopicCommand(srv, cli, params)
- "MODE" → new ModeCommand(srv, cli, params)
- "QUIT" → new QuitCommand(srv, cli, params)
- Inconnue → NULL

**Exemples de Messages**:
```
PASS secretpass
NICK johndoe
USER john 0 * :John Doe
JOIN #general
PRIVMSG #general :Hello everyone
```

---

### splitParams(const std::string& str)
```cpp
static std::vector<std::string> splitParams(const std::string& str)
```

**Objectif**: Diviser le message en paramètres

**TODO**:
- [x] Initialiser un vecteur vide pour les résultats
- [x] Itérer à travers la chaîne
- [x] Diviser par les caractères d'espace
- [x] Gérer le paramètre final (commence par ':')
  - Tout après ':' est un seul paramètre (espaces inclus)
- [x] Ignorer les espaces consécutifs
- [x] Retourner le vecteur de paramètres

**Exemples**:
```
"NICK john" → ["NICK", "john"]
"USER john 0 * :John Doe" → ["USER", "john", "0", "*", "John Doe"]
"PRIVMSG #general :Hello world" → ["PRIVMSG", "#general", "Hello world"]
"JOIN #general,#test" → ["JOIN", "#general,#test"]
```

**Cas Spéciaux**:
- Chaîne vide → vecteur vide
- Seulement des espaces → vecteur vide
- Plusieurs espaces consécutifs → traiter comme un seul espace
- Paramètre final avec ':' → tout après ':' est un seul param

---

### extractPrefix(const std::string& line)
```cpp
static std::string extractPrefix(const std::string& line)
```

**Objectif**: Extraire le préfixe d'un message IRC

**TODO**:
- [x] Vérifier si la ligne commence par ':'
- [x] Si oui: trouver le premier espace
- [x] Retourner la sous-chaîne de 1 à la position de l'espace
- [x] Si pas de préfixe: retourner une chaîne vide

**Format du Préfixe**:
- Serveur: `:irc.server`
- Client: `:nick!user@host`

**Exemples**:
```
":nick!user@host PRIVMSG #chan :hello" → "nick!user@host"
":irc.server 001 nick :Welcome" → "irc.server"
"NICK john" → ""
```

**Note**: Le préfixe est rarement envoyé par les clients, principalement utilisé dans les messages serveur

---

## Priorité d'Implémentation

### Phase 1 - Analyse de Base
1. splitParams() - division de base par espaces
2. parse() - extraction de commande de base
3. Création d'objets Command pour les commandes courantes

### Phase 2 - Paramètre Final
4. Gérer le paramètre final ':' dans splitParams()
5. Tester avec les commandes PRIVMSG et USER

### Phase 3 - Gestion du Préfixe
6. Implémentation de extractPrefix()
7. Retirer le préfixe de la ligne avant l'analyse

### Phase 4 - Toutes les Commandes
8. Ajouter tous les 11 mappages de commandes
9. Gérer les commandes inconnues (retourner NULL)

### Phase 5 - Cas Limites
10. Gérer les lignes vides
11. Gérer les messages mal formés
12. Gérer les commandes insensibles à la casse
13. Nettoyer les espaces blancs

---

## Liste de Vérification des Tests

- [ ] Commandes de base analysées correctement (PASS, NICK, USER)
- [ ] Commandes avec plusieurs paramètres (USER)
- [ ] Paramètre final avec espaces (PRIVMSG)
- [ ] Commandes de canal (JOIN, PART)
- [ ] Commandes de mode avec drapeaux (MODE)
- [ ] Commandes inconnues retournent NULL
- [ ] Messages vides gérés
- [ ] Noms de commandes insensibles à la casse
- [ ] Espaces multiples consécutifs gérés
- [ ] Extraction du préfixe fonctionne

---

## Notes sur le Protocole IRC

### Format de Message (RFC 1459)
```
message    =  [ ":" prefix SPACE ] command [ params ] crlf
prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
command    =  1*letter / 3digit
params     =  *14( SPACE middle ) [ SPACE ":" trailing ]
middle     =  nospcrlfcl *( ":" / nospcrlfcl )
trailing   =  *( ":" / " " / nospcrlfcl )
```

### Exemples du RFC 1459

#### Sans Préfixe
```
PASS secretpasswordhere
NICK Wiz
USER guest 0 * :Ronnie Reagan
JOIN #foobar
PRIVMSG #foobar :Hello World!
```

#### Avec Préfixe (depuis le serveur)
```
:irc.server 001 WiZ :Welcome to the Internet Relay Network WiZ
:Angel PRIVMSG Wiz :Hello are you receiving this message ?
:WiZ NICK Kilroy
```

#### Exemples Complexes
```
USER guest 0 * :Ronnie Reagan
JOIN #foo,#bar fubar,foobar
MODE #channel +o user
KICK #channel user :You were kicked
INVITE user #channel
```

---

## Détails d'Implémentation

### Traitement des Chaînes
- Utiliser `std::string::find()` pour la recherche
- Utiliser `std::string::substr()` pour l'extraction
- Utiliser `std::string::find_first_not_of()` pour le nettoyage
- Considérer `std::istringstream` pour la tokenisation

### Gestion de la Mémoire
- Retourner un nouveau Command* (l'appelant doit delete)
- L'appelant (Server) est responsable du nettoyage
- Ne pas fuiter de mémoire sur les erreurs d'analyse

### Gestion des Erreurs
- Messages invalides → retourner NULL
- Paramètres mal formés → retourner NULL
- Le serveur gérera NULL en n'exécutant pas
- Considérer la journalisation des erreurs d'analyse

---

## Exemple de Flux d'Implémentation

### Message d'Entrée
```
"PRIVMSG #general :Hello everyone"
```

### Étapes d'Analyse
1. Pas de préfixe (ne commence pas par ':')
2. Diviser par espace: ["PRIVMSG", "#general", ":Hello", "everyone"]
3. Trouver le paramètre final commençant par ':'
4. Fusionner à partir de ':' : ["PRIVMSG", "#general", "Hello everyone"]
5. Commande = "PRIVMSG"
6. Params = ["#general", "Hello everyone"]
7. Créer: new PrivmsgCommand(srv, cli, params)

---

## Fichiers Associés
- `Command.hpp/cpp` - Classe de base pour toutes les commandes
- `Server.hpp/cpp` - Appelle parse() pour créer les commandes
- `Client.hpp/cpp` - Source des messages
- Toutes les classes de commandes - Créées par le parseur

---

## Pièges Courants d'Analyse

### Attention À:
- **Messages vides**: Vérifier la longueur de la ligne
- **Seulement des espaces**: Nettoyer avant l'analyse
- **Paramètres manquants**: Retourner NULL si invalide
- **Espaces supplémentaires**: Gérer plusieurs espaces consécutifs
- **Sensibilité à la casse**: Convertir la commande en majuscules
- **Dépassement de tampon**: Messages IRC max 512 octets
- **Fuites mémoire**: Toujours supprimer Command après execute()

### Particularités IRC:
- Les commandes sont insensibles à la casse (NICK = nick = NiCk)
- Les paramètres sont sensibles à la casse (sauf les noms de canaux)
- Le paramètre final préserve les espaces
- Maximum 15 paramètres (RFC 1459)
- Les noms de canaux commencent par # ou &

---

## Commandes de Test

### Authentification
```
PASS password
NICK john
USER john 0 * :John Doe
```

### Opérations sur les Canaux
```
JOIN #general
JOIN #general,#test key1,key2
PART #general :Leaving
TOPIC #general :New topic here
```

### Messagerie
```
PRIVMSG #general :Hello world
PRIVMSG john :Private message
```

### Modération
```
KICK #general john :Bad behavior
INVITE john #general
MODE #general +i
MODE #general +o john
```

### Déconnexion
```
QUIT :Goodbye
```
