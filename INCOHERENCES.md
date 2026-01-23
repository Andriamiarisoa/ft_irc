# Incohérences et Modifications Nécessaires

Ce document liste les incohérences détectées et les modifications nécessaires suite à l'ajout des nouvelles classes (22 Janvier 2026).

---

## 🔴 Modifications Obligatoires

### 1. MessageParser.cpp - Ajouter les nouvelles commandes

**Fichier** : `src/MessageParser.cpp`

**État actuel** : Les commandes NOTICE, PING, PONG ne sont pas reconnues.

**Action requise** : Ajouter les includes et les cas dans `createCommand()` :

```cpp
// Ajouter aux includes
#include "../includes/NoticeCommand.hpp"
#include "../includes/PingCommand.hpp"
#include "../includes/PongCommand.hpp"

// Ajouter dans createCommand() après les autres commandes
else if (cmd == "NOTICE") {
    return new NoticeCommand(srv, cli, params);
}
else if (cmd == "PING") {
    return new PingCommand(srv, cli, params);
}
else if (cmd == "PONG") {
    return new PongCommand(srv, cli, params);
}
```

**Assigné à** : BAHOLY (responsable de MessageParser)

---

### 2. Makefile - Ajouter les nouveaux fichiers source

**Fichier** : `Makefile`

**État actuel** : Les nouveaux fichiers .cpp ne sont pas compilés.

**Action requise** : Ajouter aux sources :
```makefile
SRCS += src/NoticeCommand.cpp \
        src/PingCommand.cpp \
        src/PongCommand.cpp
```

**Assigné à** : NERO (coordination)

---

### 3. Server.hpp - Conflit Git non résolu

**Fichier** : `includes/Server.hpp`

**État actuel** : Marqueurs de conflit Git présents (`<<<<<<< HEAD`, `=======`, `>>>>>>> feature/Channel`)

```cpp
<<<<<<< HEAD
/*   Updated: 2026/01/22 15:10:35 by herrakot         ###   ########.fr       */
=======
/*   Updated: 2026/01/22 15:32:23 by herrakot         ###   ########.fr       */
>>>>>>> feature/Channel
```

Et plus bas :
```cpp
<<<<<<< HEAD
    void disconnectClient(int fd);
=======
>>>>>>> feature/Channel
    // ...
<<<<<<< HEAD
    Client* getClientByNick(const std::string& nick);
=======
    void disconnectClient(int fd);
>>>>>>> feature/Channel
```

**Action requise** : Résoudre le conflit de merge manuellement. La version finale devrait contenir :
- `void disconnectClient(int fd);`
- `Client* getClientByNick(const std::string& nick);`

**Assigné à** : NERO (responsable de Server)

---

## 🟡 Recommandations (Non Obligatoires)

### 4. Utiliser getPrefix() dans les commandes existantes

**Fichiers concernés** : 
- `src/PrivmsgCommand.cpp`
- `src/JoinCommand.cpp`
- `src/PartCommand.cpp`
- `src/QuitCommand.cpp`
- `src/KickCommand.cpp`
- `src/InviteCommand.cpp`
- `src/TopicCommand.cpp`
- `src/ModeCommand.cpp`
- `src/NickCommand.cpp`

**État actuel** : Construction manuelle du prefix dans chaque commande :
```cpp
std::string prefix = ":" + client->getNickname() + "!" + 
                     client->getUsername() + "@" + client->getHostname();
```

**Recommandation** : Remplacer par :
```cpp
std::string prefix = client->getPrefix();
```

**Assigné à** : Chaque responsable de sa commande

---

### 5. Intégrer Replies.hpp dans les commandes

**Fichiers concernés** : Toutes les commandes

**État actuel** : Les codes numériques sont construits manuellement.

**Recommandation** : Utiliser les macros de `Replies.hpp` :
```cpp
#include "Replies.hpp"

// Au lieu de
sendError(461, cmd + " :Not enough parameters");

// Utiliser
client->sendMessage(ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN"));
```

**Assigné à** : ALL (refactoring progressif)

---

### 6. Documentation tasks/ incomplète

**État actuel** : Pas de fichiers de tâches pour les nouvelles commandes.

**Recommandation** : Créer les fichiers suivants :
- `tasks/BAHOLY/NoticeCommand.md`
- `tasks/ALL/PingPongCommand.md` (ou séparer)

**Assigné à** : Créateur de la documentation

---

## 🟢 Mises à jour effectuées

Les fichiers suivants ont été mis à jour automatiquement :

| Fichier | Modification |
|---------|--------------|
| `includes/Client.hpp` | Ajout de `getPrefix()` |
| `src/Client.cpp` | Implémentation de `getPrefix()` |
| `docs/diagram/uml.puml` | Ajout NoticeCommand, PingCommand, PongCommand, getPrefix() |
| `docs/diagram/uml.drawio` | Mêmes ajouts visuels |

---

## 📊 Résumé des Actions

| Priorité | Action | Fichier | Assigné |
|----------|--------|---------|---------|
| 🔴 Haute | Résoudre conflit Git | `includes/Server.hpp` | NERO |
| 🔴 Haute | Ajouter commandes au parser | `src/MessageParser.cpp` | BAHOLY |
| 🔴 Haute | Mettre à jour Makefile | `Makefile` | NERO |
| 🟡 Moyenne | Utiliser getPrefix() | Toutes les commandes | ALL |
| 🟡 Moyenne | Intégrer Replies.hpp | Toutes les commandes | ALL |
| 🟢 Basse | Créer docs tâches | `tasks/` | - |

---

## ⚠️ Notes Importantes

1. **Ne pas merger tant que le conflit Server.hpp n'est pas résolu**
2. **Tester la compilation après modification du Makefile**
3. **Les nouvelles commandes (NOTICE, PING, PONG) ne fonctionneront pas tant que MessageParser n'est pas mis à jour**
