# Répartition des Tâches du Projet IRC

## Date de Début: 18 Janvier 2026
## ⚠️ DEADLINE: 23 Janvier 2026 (5 JOURS)
## ⏰ Temps de travail estimé: 50-60 heures par personne (10h/jour)

### 📅 Répartition horaire
- **Heures de base**: 8h00-18h00 (10h/jour) = 50h sur 5 jours
- **Heures supplémentaires** (débogage, révisions): +10h selon besoins
- **Total réaliste**: 50-60 heures

---

## 👥 Équipe & Disponibilités

### 🔴 YASSER
- **Disponibilité**: PAS DISPONIBLE le mardi à partir de 18h
- **Horaires**: Flexible les autres jours

### 🟢 NERO (Coordination recommandée)
- **Disponibilité**: 6/7 jours - 24/24h
- **Rôle suggéré**: Point de contact principal, coordination

### 🟡 BAHOLY
- **Disponibilité**: 8h-17h en semaine + 2 jours complets (24/24)
- **Horaires**: Limité mais prévisible

---

## � JOUR 1 - Infrastructure & Core (18 Janvier)
**⏰ Objectif: Fondation du serveur IRC**

### 👥 Toute l'équipe travaille ensemble sur:

#### 👤 NERO
**Server** (`includes/Server.hpp` & `src/Server.cpp`)
- Gestion du socket serveur et du réseau
- Boucle d'événements principale avec poll()
- Gestion des connexions clients
- Gestion des canaux (map)
- Démarrage/arrêt du serveur

#### 👤 YASSER
**Client** (`includes/Client.hpp` & `src/Client.cpp`)
- Gestion de la connexion client
- État d'authentification et d'enregistrement
- Buffer de messages
- Appartenance aux canaux

**Command** (classe de base - `includes/Command.hpp` & `src/Command.cpp`)
- Classe abstraite pour toutes les commandes
- Méthodes communes (sendReply, sendError)
- Pattern Command

#### 👤 BAHOLY
**MessageParser** (`includes/MessageParser.hpp` & `src/MessageParser.cpp`)
- Analyse des messages IRC
- Extraction des commandes et paramètres
- Factory pour créer les objets Command
- Gestion du parsing des paramètres

### ✅ Checkpoint Jour 1 (18h00)
- Server accepte les connexions
- Client créé pour chaque connexion
- MessageParser parse les commandes de base
- Infrastructure prête pour les commandes

---

## 📅 JOUR 2 - Authentification (19 Janvier)
**⏰ Objectif: Permettre aux utilisateurs de se connecter**

### 👥 Toute l'équipe travaille ensemble sur:

#### 👤 NERO
**PassCommand** (`includes/PassCommand.hpp` & `src/PassCommand.cpp`)
- Authentification par mot de passe
- Validation du mot de passe serveur

**QuitCommand** (`includes/QuitCommand.hpp` & `src/QuitCommand.cpp`)
- Déconnexion propre du serveur
- Diffusion du QUIT à tous les canaux
- Nettoyage des ressources

#### 👤 YASSER
**NickCommand** (`includes/NickCommand.hpp` & `src/NickCommand.cpp`)
- Définition/changement de pseudonyme
- Validation du format du nickname
- Vérification d'unicité
- Messages de bienvenue

#### 👤 BAHOLY
**UserCommand** (`includes/UserCommand.hpp` & `src/UserCommand.cpp`)
- Définition du nom d'utilisateur
- Complétion de l'enregistrement
- Séquence de bienvenue (RPL_WELCOME, etc.)

### ✅ Checkpoint Jour 2 (18h00)
- PassCommand, NickCommand, UserCommand fonctionnels
- Utilisateurs peuvent s'authentifier complètement
- QuitCommand permet la déconnexion propre

---

## 📅 JOUR 3 - Canaux & Communication (20 Janvier)
**⏰ Objectif: Créer et utiliser les canaux**

### 👥 Toute l'équipe travaille ensemble sur:

#### 👤 NERO
**Channel** (`includes/Channel.hpp` & `src/Channel.cpp`)
- Gestion des membres et opérateurs
- Modes du canal (+i, +t, +k, +l, +o)
- Sujet et clé
- Diffusion des messages

#### 👤 YASSER
**JoinCommand** (`includes/JoinCommand.hpp` & `src/JoinCommand.cpp`)
- Rejoindre des canaux
- Vérification des permissions (invite, clé, limite)
- Création de nouveaux canaux
- Premier membre = opérateur

**PartCommand** (`includes/PartCommand.hpp` & `src/PartCommand.cpp`)
- Quitter des canaux
- Message de départ optionnel
- Diffusion aux membres

#### 👤 BAHOLY
**PrivmsgCommand** (`includes/PrivmsgCommand.hpp` & `src/PrivmsgCommand.cpp`)
- Envoi de messages aux canaux
- Messages privés entre utilisateurs
- Validation des cibles

### ✅ Checkpoint Jour 3 (18h00)
- Channel créé et géré
- JoinCommand, PartCommand fonctionnels
- PrivmsgCommand permet la communication
- **Serveur IRC fonctionnel pour usage basique**

---

## 📅 JOUR 4 - Modération (21 Janvier)
**⏰ Objectif: Outils de modération des canaux**

### 👥 Toute l'équipe travaille ensemble sur:

#### 👤 NERO
**KickCommand** (`includes/KickCommand.hpp` & `src/KickCommand.cpp`)
- Expulsion d'utilisateurs des canaux
- Vérification des permissions opérateur
- Raison d'expulsion

#### 👤 YASSER
**InviteCommand** (`includes/InviteCommand.hpp` & `src/InviteCommand.cpp`)
- Invitation d'utilisateurs dans les canaux
- Gestion du mode +i (invitation uniquement)
- Liste des invités

#### 👤 BAHOLY
**TopicCommand** (`includes/TopicCommand.hpp` & `src/TopicCommand.cpp`)
- Affichage du sujet du canal
- Modification du sujet
- Mode +t (restriction opérateur)

### ✅ Checkpoint Jour 4 (18h00)
- KickCommand, InviteCommand, TopicCommand fonctionnels
- Modération de base opérationnelle

---

## 📅 JOUR 5 - Modes Avancés & Finalisation (22 Janvier)
**⏰ Objectif: Compléter et tester le serveur**

### 👥 Toute l'équipe travaille ensemble sur:

#### 👤 NERO
**ModeCommand - Partie 1** (`includes/ModeCommand.hpp` & `src/ModeCommand.cpp`)
- Modes +i, +t (invite only, topic restricted)
- Parser les changements de mode

#### 👤 YASSER
**ModeCommand - Partie 2**
- Modes +k, +l (key, user limit)
- Validation des paramètres

#### 👤 BAHOLY
**ModeCommand - Partie 3**
- Mode +o (operator)
- Vérification des permissions
- Intégration complète

### 🧪 Après-midi: Tests & Débogage (Toute l'équipe)
- Tests d'intégration avec clients IRC réels
- Correction des bugs critiques
- Validation de toutes les commandes
- Préparation de la livraison

### ✅ Checkpoint Final Jour 5 (18h00)
- ModeCommand complet et fonctionnel
- Tous les tests passent
- Serveur IRC prêt pour livraison
- Documentation minimale complétée

---

## 📋 Ordre d'Implémentation - Approche Collaborative

### Jour 1 - Infrastructure (Toute l'équipe)
- **NERO**: Server | **YASSER**: Client + Command | **BAHOLY**: MessageParser

### Jour 2 - Authentification (Toute l'équipe)
- **NERO**: PassCommand + QuitCommand | **YASSER**: NickCommand | **BAHOLY**: UserCommand

### Jour 3 - Canaux & Communication (Toute l'équipe)
- **NERO**: Channel | **YASSER**: JoinCommand + PartCommand | **BAHOLY**: PrivmsgCommand

### Jour 4 - Modération (Toute l'équipe)
- **NERO**: KickCommand | **YASSER**: InviteCommand | **BAHOLY**: TopicCommand
- ⚠️ **MARDI 21 JANVIER - YASSER indisponible après 18h**
  - Standup: 8h00 (horaire normal)
  - Checkpoint: 16h00 (horaire normal)
  - YASSER doit avoir InviteCommand bien avancée avant 18h
  - NERO/BAHOLY peuvent finaliser si nécessaire en soirée

### Jour 5 - Modes & Finalisation (Toute l'équipe)
- **NERO, YASSER, BAHOLY**: ModeCommand (collaboration sur commande complexe)
- **Après-midi**: Tests & débogage en équipe

---

## 🔧 Dépendances Entre Les Tâches

```
Server + Client + MessageParser + Command
    ↓
PassCommand + NickCommand + UserCommand
    ↓
Channel + JoinCommand + PartCommand
    ↓
PrivmsgCommand
    ↓
KickCommand + InviteCommand + TopicCommand + QuitCommand
    ↓
ModeCommand
```

---

## 📝 Notes Importantes

1. **NERO** doit livrer Server le Jour 1 pour débloquer les autres
2. **YASSER** peut commencer les commandes d'auth dès que Command est prêt
3. **BAHOLY** peut commencer dès que Channel et les commandes de base sont prêtes
4. **Communication régulière** essentielle entre les membres de l'équipe
5. **MARDI (Jour 4)**: Adapter les horaires pour YASSER (indispo après 18h)
6. **BAHOLY**: Privilégier ses horaires 8h-17h pour les tâches critiques
7. **NERO**: Disponible 24/7 - peut débloquer les autres en soirée/nuit

---

## 🎯 Objectifs par Jour (Toute l'équipe)

### Jour 1 - Objectif Collectif
✅ Créer l'infrastructure complète:
- Accepter et gérer les connexions clients
- Parser et router les commandes
- Foundation prête pour les commandes

### Jour 2 - Objectif Collectif
✅ Authentification complète:
- PASS, NICK, USER fonctionnels
- Utilisateurs peuvent se connecter
- QUIT pour déconnexion propre

### Jour 3 - Objectif Collectif
✅ Canaux et communication:
- Créer et rejoindre des canaux
- Envoyer des messages (canaux et privés)
- IRC pleinement utilisable

### Jour 4 - Objectif Collectif
✅ Modération des canaux:
- KICK, INVITE, TOPIC opérationnels
- Opérateurs peuvent gérer leurs canaux

### Jour 5 - Objectif Collectif
✅ Finalisation:
- MODE command complet (+i, +t, +k, +l, +o)
- Tests d'intégration réussis
- Serveur IRC production-ready

---

## 📊 Charge de Travail Estimée

| Jour | Focus | Complexité | Heures de base | Heures effectives* |
|------|-------|------------|----------------|-------------------|
| Jour 1 | Infrastructure | ⭐⭐⭐⭐⭐ Très Haute | 10h (8h-18h) | 10-12h |
| Jour 2 | Authentification | ⭐⭐⭐⭐ Haute | 10h (8h-18h) | 10-11h |
| Jour 3 | Canaux & Comm | ⭐⭐⭐⭐ Haute | 10h (8h-18h) | 10-12h |
| Jour 4 | Modération | ⭐⭐⭐ Moyenne | 10h (8h-18h) | 10h |
| Jour 5 | Modes & Tests | ⭐⭐⭐⭐ Haute | 10h (8h-18h) | 10-12h |

**Total par personne**: 50-57 heures sur 5 jours

*Heures effectives incluent débordements possibles pour débogage/reviews

### 👥 Répartition par Personne

**NERO** (disponible 24/7):
- Heures de base: 50h (5 jours × 10h)
- Peut faire +10-15h supplémentaires en soirée/nuit pour débloquer l'équipe
- **Total estimé**: 60-65h

**YASSER** (indisponible mardi après 18h):
- Heures de base: 50h (5 jours × 10h)
- Mardi: 10h seulement (pas de débordement)
- **Total estimé**: 50-55h

**BAHOLY** (8h-17h sauf 2 jours 24/24):
- Jours normaux (3 jours): 9h × 3 = 27h
- Jours complets (2 jours - J1 & J5): 12h × 2 = 24h
- **Total estimé**: 51-56h

**Avantage de l'approche collaborative**:
- Moins de blocages (équipe complète disponible)
- Entraide immédiate sur les problèmes
- Progression synchronisée
- Meilleure qualité de code (peer review en direct)

---

## ✅ Points de Synchronisation

### Checkpoint 1 (Fin Jour 1 - 18h00)
- [ ] Server accepte les connexions
- [ ] Client créé pour chaque connexion
- [ ] MessageParser parse les commandes
- [ ] **Git**: Toutes features J1 mergées dans `develop`
- [ ] **Git**: Tag `v1.0-checkpoint1` sur `main`

### Checkpoint 2 (Fin Jour 2 - 18h00)
- [ ] PassCommand, NickCommand, UserCommand fonctionnels
- [ ] Authentification complète opérationnelle
- [ ] **Git**: Toutes features J2 mergées dans `develop`
- [ ] **Git**: Tag `v1.0-checkpoint2` sur `main`

### Checkpoint 3 (Fin Jour 3 - 18h00)
- [ ] Channel créé et géré
- [ ] JoinCommand, PartCommand fonctionnels
- [ ] PrivmsgCommand permet la communication
- [ ] **Git**: Toutes features J3 mergées dans `develop`
- [ ] **Git**: Tag `v1.0-checkpoint3` sur `main`

### Checkpoint 4 (Fin Jour 4 - 18h00)
- [ ] Toutes les commandes de modération fonctionnelles (KICK, INVITE, TOPIC)
- [ ] **Git**: Toutes features J4 mergées dans `develop`
- [ ] **Git**: Tag `v1.0-checkpoint4` sur `main`

### Checkpoint 5 (Fin Jour 5 - 18h00) - LIVRAISON FINALE
- [ ] ModeCommand complet
- [ ] Tests d'intégration passent
- [ ] Serveur IRC complet et livrable
- [ ] **Git**: Toutes features mergées dans `develop`
- [ ] **Git**: Merge final `develop` → `main`
- [ ] **Git**: Tag `v1.0-release` sur `main`
- [ ] **Git**: Repository propre (branches feature supprimées)

---

## 🤝 Coordination

⚠️ **DEADLINE: 5 JOURS - RYTHME INTENSIF REQUIS**

### ⚠️ Contraintes de Disponibilité
- **Mardi 21 Janvier (Jour 4)**: YASSER indisponible après 18h
  - Standups aux horaires normaux (8h00 et 16h00)
  - YASSER doit terminer sa tâche avant 18h
  - NERO et BAHOLY peuvent continuer la soirée si nécessaire
- **BAHOLY**: Limité à 8h-17h sauf 2 jours complets
  - Privilégier les tâches importantes en journée
  - Planifier ses 2 jours 24/24 pour les jours critiques (Jour 1 et Jour 5)
- **NERO**: Disponible en continu - point de contact principal
  - Peut travailler soir/nuit pour débloquer l'équipe

- **Réunions quotidiennes** OBLIGATOIRES
  - **08h00**: Standup matin - objectifs du jour (15 min)
  - **16h00**: Checkpoint - revue des avancements (15 min)
- **Communication continue** via chat/Discord
- **Disponibilité** requise 8h-20h pour questions/blocages
- **Prioriser le fonctionnel** sur la perfection

---

## 🔀 Stratégie Git Flow

### Structure des Branches

```
main (production-ready)
  │
  ├── develop (intégration)
       │
       ├── feature/server-core (NERO - Jour 1)
       │   └── Server: socket, poll, event loop
       │
       ├── feature/client-management (YASSER - Jour 1)
       │   └── Client: connexion, auth, buffer
       │
       ├── feature/command-base (YASSER - Jour 1)
       │   └── Command: classe abstraite, sendReply
       │
       ├── feature/message-parser (BAHOLY - Jour 1)
       │   └── MessageParser: parsing IRC, factory
       │
       ├── feature/pass-command (NERO - Jour 2)
       │   └── PassCommand: authentification serveur
       │
       ├── feature/quit-command (NERO - Jour 2)
       │   └── QuitCommand: déconnexion propre
       │
       ├── feature/nick-command (YASSER - Jour 2)
       │   └── NickCommand: pseudonyme, validation
       │
       ├── feature/user-command (BAHOLY - Jour 2)
       │   └── UserCommand: username, bienvenue
       │
       ├── feature/channel-management (NERO - Jour 3)
       │   └── Channel: membres, modes, broadcast
       │
       ├── feature/join-command (YASSER - Jour 3)
       │   └── JoinCommand: rejoindre canaux
       │
       ├── feature/part-command (YASSER - Jour 3)
       │   └── PartCommand: quitter canaux
       │
       ├── feature/privmsg-command (BAHOLY - Jour 3)
       │   └── PrivmsgCommand: messages canaux/privés
       │
       ├── feature/kick-command (NERO - Jour 4)
       │   └── KickCommand: expulsion utilisateurs
       │
       ├── feature/invite-command (YASSER - Jour 4)
       │   └── InviteCommand: invitations canaux
       │
       ├── feature/topic-command (BAHOLY - Jour 4)
       │   └── TopicCommand: gestion sujet canal
       │
       └── feature/mode-command (NERO+YASSER+BAHOLY - Jour 5)
           ├── mode-invite-topic (+i, +t)
           ├── mode-key-limit (+k, +l)
           └── mode-operator (+o)
```

### Cycle de Vie d'une Branche

```
1. Création depuis develop
   │
2. Développement (commits fréquents)
   │
3. Synchronisation avec develop (rebase)
   │
4. Pull Request vers develop
   │
5. Code Review (max 1h)
   │
6. Corrections si nécessaire
   │
7. Approbation
   │
8. Merge (squash) vers develop
   │
9. Suppression de la feature branch
   │
10. Tests d'intégration sur develop
```

### Branches Principales

#### 🌱 `main`
- Branche de production
- **Toujours stable et fonctionnelle**
- Merge uniquement depuis `develop` après validation
- Tags pour chaque checkpoint (v1.0-checkpoint1, v1.0-checkpoint2, etc.)

#### 🌿 `develop`
- Branche d'intégration
- Reçoit tous les merges des features
- Tests d'intégration exécutés ici
- Merge vers `main` à chaque checkpoint validé

### Branches de Fonctionnalités (Feature Branches)

#### Convention de Nommage
```
feature/<nom-descriptif>
```

**Exemples**:
- `feature/server-core`
- `feature/client-management`
- `feature/pass-command`
- `feature/join-command`
- `feature/mode-command-invite`

#### Règles
1. **Créer depuis `develop`**
   ```bash
   git checkout develop
   git pull origin develop
   git checkout -b feature/nom-fonctionnalite
   ```

2. **Commits fréquents et atomiques**
   ```bash
   git add <fichiers>
   git commit -m "feat: description claire de la fonctionnalité"
   git push origin feature/nom-fonctionnalite
   ```

3. **Synchronisation régulière avec develop**
   ```bash
   # Plusieurs fois par jour
   git checkout develop
   git pull origin develop
   git checkout feature/nom-fonctionnalite
   git rebase develop
   ```

### Conventions de Commits (Conventional Commits)

```
<type>(<scope>): <description courte>

[corps optionnel]

[footer optionnel]
```

#### Types de commits
- `feat`: Nouvelle fonctionnalité (ex: `feat(server): add socket initialization`)
- `fix`: Correction de bug (ex: `fix(client): resolve buffer overflow`)
- `refactor`: Refactoring sans changement de fonctionnalité
- `test`: Ajout ou modification de tests
- `docs`: Documentation uniquement
- `style`: Formatage, points-virgules manquants, etc.
- `chore`: Maintenance, configuration

#### Exemples
```bash
feat(server): implement poll-based event loop
feat(command): add base Command class with sendReply
fix(parser): handle empty message parameters
refactor(channel): optimize member lookup
test(join): add unit tests for channel join
```

### Workflow de Développement Quotidien

#### Matin (9h00 - Après Standup)

1. **Synchroniser avec develop**
   ```bash
   git checkout develop
   git pull origin develop
   ```

2. **Créer ou revenir sur feature branch**
   ```bash
   # Nouvelle feature
   git checkout -b feature/ma-tache
   
   # Continuer une feature
   git checkout feature/ma-tache
   git rebase develop  # Intégrer les changements
   ```

3. **Développer et commiter régulièrement**
   ```bash
   # Toutes les 1-2h ou à chaque fonctionnalité terminée
   git add .
   git commit -m "feat(scope): description"
   git push origin feature/ma-tache
   ```

#### Après-midi / Fin de journée

4. **Pull Request vers develop**
   - Ouvrir PR sur GitHub/GitLab
   - Description claire de ce qui a été implémenté
   - Assigner un reviewer (voir section Reviews)
   - Lier aux issues/taches concernées

5. **Review de code**
   - Reviewer assigné doit répondre sous 1h
   - Corrections si nécessaires
   - Approbation

6. **Merge vers develop**
   - **Squash and merge** pour garder historique propre
   - Supprimer la feature branch après merge

### Rôles de Review

#### Matrice de Review

| Auteur | Reviewer Principal | Reviewer Secondaire |
|--------|-------------------|--------------------|
| NERO | YASSER | BAHOLY |
| YASSER | BAHOLY | NERO |
| BAHOLY | NERO | YASSER |

#### Checklist de Review

**Reviewer doit vérifier**:
- [ ] Code compile sans erreurs ni warnings
- [ ] Respecte les conventions de nommage C++
- [ ] Pas de fuites mémoire (new/delete balancés)
- [ ] Gestion des erreurs appropriée
- [ ] Code lisible et commenté si nécessaire
- [ ] Tests manuels effectués (si applicable)
- [ ] Pas de code commenté ou debug inutile

**Temps de review**: MAX 1h - Si plus complexe, reviewer peut demander une session de pair programming

### Gestion des Conflits

#### Prévention
1. **Synchroniser souvent** avec develop (2-3x par jour)
2. **Communiquer** les fichiers sur lesquels on travaille
3. **PRs petites et fréquentes** plutôt que grosses et rares

#### Résolution
```bash
# Si conflit lors du rebase
git checkout develop
git pull origin develop
git checkout feature/ma-branche
git rebase develop

# Résoudre les conflits dans les fichiers
# Puis:
git add <fichiers-résolus>
git rebase --continue
git push origin feature/ma-branche --force-with-lease
```

**⚠️ En cas de blocage**: Appeler NERO (disponible 24/7) pour aide

### Checkpoints & Merges vers Main

À chaque checkpoint (18h00 chaque jour):

1. **Toutes les features du jour doivent être mergées dans develop**
2. **Tests d'intégration sur develop**
   ```bash
   git checkout develop
   make
   ./ircserv 6667 password
   # Tests manuels avec client IRC
   ```

3. **Si tests OK: Merge develop → main**
   ```bash
   git checkout main
   git merge develop --no-ff
   git tag v1.0-checkpoint-X
   git push origin main --tags
   ```

4. **Si tests KO**: Debug en équipe, fix rapide, retest

### Commandes Git Utiles

```bash
# Voir l'état des branches
git branch -a

# Voir l'historique graphique
git log --graph --oneline --all

# Synchroniser toutes les branches
git fetch --all

# Revenir à un commit précédent (urgence)
git reset --hard <commit-hash>

# Sauvegarder travail en cours sans commit
git stash
git stash pop

# Voir les différences
git diff develop...feature/ma-branche
```

### Organisation des Branches par Jour
**`nb: le nom de branche est au choix du responsable de la tache`**
#### Jour 1
- `feature/server-core` (*NERO*)
- `feature/client-management` (*YASSER*)
- `feature/message-parser` (*BAHOLY*)
- `feature/command-base` (*YASSER*)

#### Jour 2
- `feature/pass-command` (*NERO*)
- `feature/quit-command` (*NERO*)
- `feature/nick-command` (*YASSER*)
- `feature/user-command` (*BAHOLY*)

#### Jour 3
- `feature/channel-management` (*NERO*)
- `feature/join-command` (*YASSER*)
- `feature/part-command` (*YASSER*)
- `feature/privmsg-command` (*BAHOLY*)

#### Jour 4
- `feature/kick-command` (*NERO*)
- `feature/invite-command` (*YASSER*)
- `feature/topic-command` (*BAHOLY*)

#### Jour 5
- `feature/mode-command-core` (*NERO*, *YASSER*, *BAHOLY* - collaboration)
- `feature/integration-tests` (Toute l'équipe)

### 🚨 Règles d'Or Git Flow

1. **JAMAIS de commit direct sur `main` ou `develop`**
2. **TOUJOURS passer par une feature branch + PR**
3. **1 feature branch = 1 fonctionnalité claire**
4. **Synchroniser avec develop AVANT de faire une PR**
5. **Supprimer les feature branches après merge**
6. **Commits atomiques et messages clairs**
7. **Ne JAMAIS forcer push sur develop ou main**
8. **En cas de doute: demander avant de merger**
- **Tests manuels** prioritaires, tests unitaires si temps disponible

### Planning Journalier Suggéré
- **08h00**: Standup - objectifs du jour (15 min)
- **08h15-12h00**: Développement intensif
- **12h00-13h00**: Pause déjeuner
- **13h00-16h00**: Développement + intégration
- **16h00-16h15**: Standup de fin - revue des avancements (15 min)
- **16h15-18h00**: PRs, reviews, merges vers develop
- **18h00**: Checkpoint quotidien

---
