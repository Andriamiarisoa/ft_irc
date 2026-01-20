# Tests de la Classe Client

## Vue d'ensemble

Ce fichier de test valide le bon fonctionnement de la classe `Client` qui gère les connexions clients IRC.

## Compilation et Exécution

### Compiler les tests
```bash
g++ -std=c++98 -Wall -Wextra -Werror -I includes tests/test_client.cpp src/Client.cpp src/Channel.cpp -o tests/test_client
```

### Exécuter les tests
```bash
./tests/test_client
```

### Vérifier les fuites mémoire
```bash
valgrind --leak-check=full ./tests/test_client
```

---

## Tests Implémentés

### 1. `test_constructor`
Vérifie l'état initial d'un client nouvellement créé :
- `fd` correctement stocké
- `nickname` et `username` vides
- `authenticated = false`
- `registered = false`

### 2. `test_authentication_flow`
Vérifie le flux d'authentification :
- État initial non authentifié
- Après `authenticate()`, le client est authentifié mais pas encore enregistré

### 3. `test_registration_flow`
Vérifie le flux complet d'enregistrement IRC :
1. `PASS` → authentifié, non enregistré
2. `NICK` → nickname défini, non enregistré (USER manquant)
3. `USER` → enregistré (les 3 étapes complétées)

### 4. `test_nickname_validation`
Vérifie la validation du nickname selon la RFC 1459 :

| Cas | Résultat attendu |
|-----|------------------|
| `Alice` | ✅ Valide |
| `Bob123` | ✅ Valide |
| `Test-User` | ✅ Valide |
| `a[]` | ✅ Valide (caractères spéciaux RFC) |
| `a{}` | ✅ Valide |
| `""` (vide) | ❌ Rejeté |
| `123abc` | ❌ Rejeté (commence par chiffre) |
| `-test` | ❌ Rejeté (commence par tiret) |
| `VeryLongNickname` | ❌ Rejeté (> 9 caractères) |
| `test@user` | ❌ Rejeté (@ invalide) |

### 5. `test_buffer_operations`
Vérifie les opérations de base sur le buffer :
- Ajout de données au buffer
- Extraction d'une commande complète (terminée par `\r\n`)
- Buffer vide après extraction

### 6. `test_multiple_commands_in_buffer`
Vérifie l'extraction de plusieurs commandes :
```
"NICK alice\r\nUSER alice 0 * :Alice\r\nJOIN #test\r\n"
```
Doit extraire 3 commandes séparément dans l'ordre.

### 7. `test_incomplete_command`
Vérifie le comportement avec des commandes incomplètes :
- `"NICK test"` sans `\r\n` → retourne chaîne vide
- Après ajout de `\r\n` → extrait la commande complète

### 8. `test_buffer_overflow_protection`
Vérifie la protection contre le dépassement de buffer :
- Limite de 1024 octets
- Si dépassée, le buffer est vidé (protection)

### 9. `test_order_independence`
Vérifie que l'ordre NICK/USER n'a pas d'importance :
- `USER` avant `NICK` → non enregistré
- Puis `NICK` → enregistré

### 10. `test_no_auth_no_registration`
Vérifie qu'un client ne peut pas s'enregistrer sans authentification :
- NICK + USER sans PASS → non enregistré
- Après PASS → enregistré

---

## Sortie Attendue

```
=== Tests de la classe Client ===

Testing Constructor... PASS
Testing Authentication flow... PASS
Testing Registration flow (PASS -> NICK -> USER)... PASS
Testing Nickname validation (RFC 1459)... PASS
Testing Buffer operations... PASS
Testing Multiple commands in buffer... PASS
Testing Incomplete command in buffer... PASS
Testing Buffer overflow protection... PASS
Testing Order independence (USER before NICK)... PASS
Testing No auth = no registration... PASS

=== Tous les tests sont passés ! ===
```

---

## Ajouter un Nouveau Test

```cpp
void test_mon_nouveau_test() {
    TEST("Description du test");
    Client client(1);
    
    // Votre logique de test avec assert()
    assert(condition);
    
    PASS();
}

// Dans main(), ajouter :
test_mon_nouveau_test();
```

---

## Dépendances

- `Client.hpp` / `Client.cpp`
- `Channel.hpp` / `Channel.cpp` (pour la compilation)

## Notes

- Les tests utilisent `assert()` pour les vérifications
- En cas d'échec, le programme s'arrête immédiatement avec une erreur
- Valgrind confirme : **0 fuites mémoire**
