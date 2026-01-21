# Tests MessageParser - Documentation

## 📋 Vue d'ensemble

Ce fichier décrit les tests unitaires pour la classe `MessageParser` implémentée dans `src/MessageParser.cpp`. Ces tests sont **isolés** et ne dépendent pas des autres classes du projet (Command, Server, Client).

## 🎯 Fonctions testées

Les tests couvrent les fonctions de parsing implémentées directement dans `MessageParser.cpp` :

| Fonction | Description |
|----------|-------------|
| `extractPrefix()` | Extrait le préfixe d'un message IRC (ex: `:nick!user@host`) |
| `splitParams()` | Découpe un message IRC en tokens (commande + paramètres) |
| `removePrefix()` | Fonction helper qui retire le préfixe du message |
| `toUpperCase()` | Conversion en majuscules pour insensibilité à la casse |

## 🔧 Compilation et exécution

```bash
# Compiler les tests
g++ -std=c++98 -Wall -Wextra -Werror tests/test_messageparser.cpp -o test_messageparser

# Exécuter les tests
./test_messageparser
```

## 📊 Cas de tests

### 1. Extraction du préfixe (`extractPrefix`)

| Test | Input | Expected | Description |
|------|-------|----------|-------------|
| `test_extractPrefix_emptyString` | `""` | `""` | Chaîne vide |
| `test_extractPrefix_noPrefix` | `"NICK john"` | `""` | Pas de préfixe |
| `test_extractPrefix_simplePrefix` | `":nick PRIVMSG #chan :msg"` | `":nick"` | Préfixe simple |
| `test_extractPrefix_prefixOnly` | `":onlyprefix"` | `":onlyprefix"` | Préfixe seul sans commande |
| `test_extractPrefix_prefixWithMultipleSpaces` | `":prefix    COMMAND"` | `":prefix"` | Préfixe avec espaces multiples |

### 2. Commandes de base (PASS, NICK, USER)

| Test | Input | Tokens attendus |
|------|-------|-----------------|
| `test_splitParams_PASS` | `"PASS secretpassword"` | `["PASS", "secretpassword"]` |
| `test_splitParams_NICK` | `"NICK john_doe"` | `["NICK", "john_doe"]` |
| `test_splitParams_USER_basic` | `"USER john 0 * :John Doe"` | `["USER", "john", "0", "*", ":John Doe"]` |

### 3. Commandes avec plusieurs paramètres (USER)

| Test | Input | Tokens attendus |
|------|-------|-----------------|
| `test_splitParams_USER_multipleParams` | `"USER username hostname servername :Real Name Here"` | `["USER", "username", "hostname", "servername", ":Real Name Here"]` |

### 4. Paramètre final avec espaces (PRIVMSG)

| Test | Input | Dernier token |
|------|-------|---------------|
| `test_splitParams_PRIVMSG_simpleMessage` | `"PRIVMSG #channel :Hello World!"` | `":Hello World!"` |
| `test_splitParams_PRIVMSG_longMessage` | `"PRIVMSG nick :This is a very long message..."` | Message complet préservé |
| `test_splitParams_PRIVMSG_colonInMessage` | `"PRIVMSG #channel :Time is 12:30:00 now"` | `":Time is 12:30:00 now"` |
| `test_splitParams_PRIVMSG_emptyTrailing` | `"PRIVMSG #channel :"` | `":"` |

### 5. Commandes de canal (JOIN, PART)

| Test | Input | Tokens attendus |
|------|-------|-----------------|
| `test_splitParams_JOIN_singleChannel` | `"JOIN #channel"` | `["JOIN", "#channel"]` |
| `test_splitParams_JOIN_withKey` | `"JOIN #channel secretkey"` | `["JOIN", "#channel", "secretkey"]` |
| `test_splitParams_JOIN_multipleChannels` | `"JOIN #chan1,#chan2,#chan3"` | `["JOIN", "#chan1,#chan2,#chan3"]` |
| `test_splitParams_PART_singleChannel` | `"PART #channel"` | `["PART", "#channel"]` |
| `test_splitParams_PART_withMessage` | `"PART #channel :Goodbye!"` | `["PART", "#channel", ":Goodbye!"]` |

### 6. Commandes de mode avec drapeaux (MODE)

| Test | Input | Tokens attendus |
|------|-------|-----------------|
| `test_splitParams_MODE_queryChannel` | `"MODE #channel"` | `["MODE", "#channel"]` |
| `test_splitParams_MODE_setInviteOnly` | `"MODE #channel +i"` | `["MODE", "#channel", "+i"]` |
| `test_splitParams_MODE_setTopic` | `"MODE #channel +t"` | `["MODE", "#channel", "+t"]` |
| `test_splitParams_MODE_setKey` | `"MODE #channel +k secretkey"` | `["MODE", "#channel", "+k", "secretkey"]` |
| `test_splitParams_MODE_operatorGrant` | `"MODE #channel +o nickname"` | `["MODE", "#channel", "+o", "nickname"]` |
| `test_splitParams_MODE_setLimit` | `"MODE #channel +l 25"` | `["MODE", "#channel", "+l", "25"]` |
| `test_splitParams_MODE_multipleFlags` | `"MODE #channel +it"` | `["MODE", "#channel", "+it"]` |
| `test_splitParams_MODE_removeFlag` | `"MODE #channel -i"` | `["MODE", "#channel", "-i"]` |
| `test_splitParams_MODE_mixedFlags` | `"MODE #channel +o-v nick1 nick2"` | `["MODE", "#channel", "+o-v", "nick1", "nick2"]` |

### 7. Messages vides et invalides

| Test | Input | Expected |
|------|-------|----------|
| `test_splitParams_emptyString` | `""` | `[]` (vecteur vide) |
| `test_splitParams_onlySpaces` | `"     "` | `[]` ou géré sans crash |
| `test_splitParams_onlyPrefix` | `":onlyprefix"` | `[]` |
| `test_splitParams_prefixWithSpaces` | `":prefix    "` | `[]` |

### 8. Insensibilité à la casse

| Test | Input | Output |
|------|-------|--------|
| `test_caseInsensitive_lowercase` | `"nick"` | `"NICK"` |
| `test_caseInsensitive_mixedCase` | `"NiCk"` | `"NICK"` |
| `test_caseInsensitive_uppercase` | `"NICK"` | `"NICK"` |

### 9. Espaces multiples consécutifs

| Test | Input | Comportement attendu |
|------|-------|---------------------|
| `test_multipleSpaces_betweenParams` | `"NICK    john"` | Espaces ignorés, 2 tokens |
| `test_multipleSpaces_multiple` | `"MODE   #channel   +o   nick"` | 4 tokens corrects |
| `test_multipleSpaces_beforeTrailing` | `"PRIVMSG #channel   :Hello"` | Message trailing correct |
| `test_multipleSpaces_leadingSpaces` | `"   NICK john"` | Géré sans crash |

### 10. Commandes additionnelles

| Test | Input | Description |
|------|-------|-------------|
| `test_splitParams_KICK` | `"KICK #channel nick :Reason"` | 4 tokens avec raison |
| `test_splitParams_INVITE` | `"INVITE nick #channel"` | 3 tokens |
| `test_splitParams_TOPIC_query` | `"TOPIC #channel"` | Query sans nouveau topic |
| `test_splitParams_TOPIC_set` | `"TOPIC #channel :New topic"` | Avec nouveau topic |
| `test_splitParams_QUIT_noMessage` | `"QUIT"` | Sans message |
| `test_splitParams_QUIT_withMessage` | `"QUIT :Leaving"` | Avec message |

### 11. Cas limites avec préfixe

| Test | Input | Description |
|------|-------|-------------|
| `test_prefix_fullHostmask` | `":nick!user@host.com CMD"` | Hostmask complet |
| `test_prefix_serverSource` | `":irc.server.net 001 nick :Welcome"` | Serveur comme source |
| `test_prefix_colonInMessage` | `"PRIVMSG #chan :URL http://..."` | Colon dans le message |

### 12. Caractères spéciaux

| Test | Input | Description |
|------|-------|-------------|
| `test_specialChars_nickWithUnderscore` | `"NICK john_doe_123"` | Underscore dans nick |
| `test_specialChars_channelWithAmpersand` | `"JOIN &channel"` | Channel avec & |
| `test_specialChars_messageWithSpecialChars` | Message avec @, #, $, % | Tous préservés |

## ✅ Checklist des fonctionnalités testées

- [x] Commandes de base analysées correctement (PASS, NICK, USER)
- [x] Commandes avec plusieurs paramètres (USER)
- [x] Paramètre final avec espaces (PRIVMSG)
- [x] Commandes de canal (JOIN, PART)
- [x] Commandes de mode avec drapeaux (MODE)
- [x] Commandes inconnues retournent NULL (testé indirectement)
- [x] Messages vides gérés
- [x] Noms de commandes insensibles à la casse
- [x] Espaces multiples consécutifs gérés
- [x] Extraction du préfixe fonctionne

## 🔍 Notes importantes

### Pourquoi des tests isolés ?

Les tests sont **isolés** des autres composants du projet pour :
1. Pouvoir tester `MessageParser` indépendamment
2. Ne pas avoir de dépendances sur les implémentations de `Command`, `Server`, `Client`
3. Faciliter le debugging des problèmes de parsing
4. Permettre les tests même si les autres classes ne sont pas finies

### Format des messages IRC

Un message IRC suit ce format :
```
[:prefix] <command> [params] [:trailing]
```

- **Préfixe** (optionnel) : Commence par `:`, indique la source
- **Commande** : Nom de commande ou code numérique
- **Paramètres** : Séparés par espaces
- **Trailing** : Commence par `:`, peut contenir des espaces

### Exemples de parsing

```
Input:  ":nick!user@host PRIVMSG #channel :Hello World!"
Prefix: ":nick!user@host"
Tokens: ["PRIVMSG", "#channel", ":Hello World!"]

Input:  "JOIN #channel secretkey"
Prefix: ""
Tokens: ["JOIN", "#channel", "secretkey"]
```

## 📈 Résultat attendu

```
╔══════════════════════════════════════════════════════════════╗
║           MessageParser Unit Tests                           ║
╚══════════════════════════════════════════════════════════════╝

=== extractPrefix - Empty string ===
[PASS] Empty string returns empty prefix
...

══════════════════════════════════════════════════════════════
                        TEST SUMMARY                          
══════════════════════════════════════════════════════════════
Passed: XX
Failed: 0
Total:  XX

✓ All tests passed!
```

## 👥 Auteur

Tests créés pour le projet IRC - Équipe BAHOLY
