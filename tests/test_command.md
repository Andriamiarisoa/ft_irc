# Tests de la Classe Command

## Vue d'ensemble

Ce fichier de test valide le bon fonctionnement de la classe abstraite `Command`, qui est la classe de base pour toutes les commandes IRC (NICK, JOIN, PRIVMSG, etc.).

## Compilation et Exécution

### Compiler les tests
```bash
g++ -std=c++98 -Wall -Wextra -Werror -I includes \
    tests/test_command.cpp \
    src/Command.cpp \
    src/Client.cpp \
    src/Server.cpp \
    src/Channel.cpp \
    src/MessageParser.cpp \
    -o tests/test_command
```

### Exécuter les tests
```bash
./tests/test_command
```

### Vérifier les fuites mémoire
```bash
valgrind --leak-check=full ./tests/test_command
```

---

## Architecture de Test

### TestCommand
Classe concrète dérivée de `Command` qui :
- Implémente `execute()` (vide, pour les tests)
- Expose les méthodes protégées via des wrappers publics :
  - `testSendReply()`
  - `testSendError()`
- Fournit des getters pour les attributs protégés

### Socket Pair
Les tests utilisent `socketpair()` pour créer une paire de sockets :
- Le client envoie sur `writeFd`
- Les tests lisent sur `readFd`
- Permet de vérifier le contenu réel des messages envoyés

---

## Tests Implémentés

### 1. `test_constructor_valid`
Vérifie que le constructeur stocke correctement :
- Pointeur `server`
- Pointeur `client`
- Vecteur `params`

### 2. `test_constructor_null_server`
Vérifie que le constructeur lance une exception si `server == NULL`.

### 3. `test_constructor_null_client`
Vérifie que le constructeur lance une exception si `client == NULL`.

### 4. `test_constructor_empty_params`
Vérifie que le constructeur accepte un vecteur de paramètres vide.

### 5. `test_sendReply_format_unregistered`
Vérifie le format de réponse pour un client non enregistré :
```
:irc.example.com 001 * :Welcome to IRC\r\n
```
- Le nickname est `*` pour les clients non enregistrés

### 6. `test_sendReply_format_registered`
Vérifie le format de réponse pour un client enregistré :
```
:irc.example.com 332 TestUser #channel :This is the topic\r\n
```
- Le nickname réel est utilisé

### 7. `test_sendReply_code_padding`
Vérifie que les codes numériques sont formatés sur 3 chiffres :

| Code | Formaté |
|------|---------|
| 1    | 001     |
| 42   | 042     |
| 461  | 461     |

### 8. `test_sendError_format`
Vérifie le format d'erreur avec `:` avant le message :
```
:irc.example.com 461 * :Not enough parameters\r\n
```

### 9. `test_sendReply_ends_with_crlf`
Vérifie que `sendReply()` termine toujours par `\r\n`.

### 10. `test_sendError_ends_with_crlf`
Vérifie que `sendError()` termine toujours par `\r\n`.

### 11. `test_polymorphic_destruction`
Vérifie que le destructeur virtuel fonctionne correctement :
```cpp
Command* basePtr = new TestCommand(...);
delete basePtr;  // Doit appeler le bon destructeur
```

---

## Format des Messages IRC

### sendReply (réponses)
```
:hostname code nickname message\r\n
```
Exemple : `:irc.example.com 001 john :Welcome to the IRC network\r\n`

### sendError (erreurs)
```
:hostname code nickname :message\r\n
```
Exemple : `:irc.example.com 461 john :Not enough parameters\r\n`

**Note** : `sendError` ajoute automatiquement `:` devant le message.

---

## Sortie Attendue

```
=== Tests de la classe Command ===

Testing Constructor with valid pointers... PASS
Testing Constructor with NULL server throws exception... PASS
Testing Constructor with NULL client throws exception... PASS
Testing Constructor with empty params... PASS
Testing sendReply format for unregistered client... PASS
Testing sendReply format for registered client... PASS
Testing sendReply code padding (3 digits)... PASS
Testing sendError format (with colon before message)... PASS
Testing sendReply ends with CRLF... PASS
Testing sendError ends with CRLF... PASS
Testing Polymorphic destruction (virtual destructor)... PASS

=== Tous les tests sont passés ! ===
```

---

## Ajouter un Nouveau Test

```cpp
void test_mon_nouveau_test() {
    TEST("Description du test");
    
    int readFd, writeFd;
    assert(createTestSocketPair(readFd, writeFd) == 0);
    
    Server server(6667, "password");
    Client client(writeFd);
    std::vector<std::string> params;
    
    TestCommand cmd(&server, &client, params);
    
    // Votre logique de test
    cmd.testSendReply(200, ":message");
    std::string received = readFromSocket(readFd);
    assert(/* votre condition */);
    
    close(readFd);
    close(writeFd);
    PASS();
}

// Dans main(), ajouter :
test_mon_nouveau_test();
```

---

## Dépendances

- `Command.hpp` / `Command.cpp`
- `Client.hpp` / `Client.cpp`
- `Server.hpp` / `Server.cpp`
- `Channel.hpp` / `Channel.cpp`
- `MessageParser.hpp` / `MessageParser.cpp`

## Notes

- Les tests utilisent `socketpair()` pour simuler des connexions réseau
- Les sockets sont fermés après chaque test pour éviter les fuites de descripteurs
- Valgrind confirme : **0 fuites mémoire**
