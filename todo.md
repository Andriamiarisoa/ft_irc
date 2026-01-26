# TODO - ft_irc

## Configuration du projet

- [x] Créer le Makefile avec les règles : `$(NAME)`, `all`, `clean`, `fclean`, `re`
- [x] Configurer les flags de compilation : `-Wall -Wextra -Werror -std=c++98`
- [x] Structurer les fichiers sources (*.cpp, *.hpp)
- [x] Créer fichier de configuration optionnel

## Architecture de base

- [x] Implémenter la classe Server
- [x] Implémenter la classe Client
- [x] Implémenter la classe Channel
- [x] Gérer les arguments : `./ircserv <port> <password>`
- [x] Valider le port et le mot de passe au démarrage

## Gestion réseau (TCP/IP)

- [x] Créer le socket serveur avec `socket()`
- [x] Configurer le socket avec `setsockopt()`
- [x] Lier le socket au port avec `bind()`
- [x] Mettre le serveur en écoute avec `listen()`
- [x] Accepter les connexions avec `accept()`
- [x] Configurer tous les file descriptors en mode non-bloquant (`fcntl()` pour MacOS)

## Multiplexage I/O

- [x] Implémenter `select()` (ou équivalent : poll, kqueue, epoll)
- [x] Gérer multiple clients simultanément sans blocage
- [x] UN SEUL `select()` pour toutes les opérations (read, write, listen)
- [x] Préparer les fd_set avec FD_ZERO et FD_SET avant chaque appel
- [x] Calculer max_fd (plus grand descripteur + 1) pour nfds
- [x] Vérifier les descripteurs prêts avec FD_ISSET après select()
- [x] Gérer les timeout avec struct timeval
- [x] Reconstruire les fd_set à chaque itération (modifiés par select)

## Gestion des messages

- [x] Parser les commandes IRC reçues
- [x] Gérer la réception de données partielles (buffer)
- [x] Reconstruire les commandes complètes à partir des packets
- [x] Gérer les messages terminés par `\r\n`
- [ ] Tester avec `nc -C` et ctrl+D pour envoyer des données partielles

## Authentification

- [x] Vérifier le mot de passe de connexion
- [x] Implémenter la commande PASS
- [x] Implémenter la commande NICK (définir le pseudo)
- [x] Implémenter la commande USER (définir le username)
- [x] Gérer l'ordre d'authentification
- [x] Envoyer les messages de bienvenue (RPL_WELCOME, etc.)

## Gestion des canaux

- [x] Implémenter la commande JOIN (rejoindre un canal)
- [x] Implémenter la commande PART (quitter un canal)
- [x] Créer automatiquement les canaux si inexistants
- [x] Définir le premier utilisateur comme opérateur
- [x] Transmettre les messages à tous les membres du canal
- [x] Gérer la liste des membres par canal

## Messages privés

- [x] Implémenter la commande PRIVMSG (messages privés)
- [x] Implémenter la commande NOTICE (notifications sans réponse)
- [x] Supporter les messages utilisateur à utilisateur
- [x] Supporter les messages vers les canaux
- [x] Vérifier l'existence du destinataire

## Keep-Alive (PING/PONG)

- [x] Implémenter la commande PING (client → server)
- [x] Implémenter la commande PONG (réponse au PING)
- [ ] Détecter les clients inactifs (timeout)

## Opérateurs de canal

- [x] Gérer les privilèges d'opérateur (operator vs regular user)
- [x] Implémenter la commande KICK (éjecter un client)
- [x] Implémenter la commande INVITE (inviter un client)
- [x] Implémenter la commande TOPIC (changer/voir le topic)
- [x] Implémenter la commande MODE avec les modes suivants :
  - [x] `i` : Mode invite-only (invitation seulement)
  - [x] `t` : Restriction TOPIC aux opérateurs
  - [x] `k` : Clé du canal (mot de passe)
  - [x] `o` : Donner/retirer privilège d'opérateur
  - [x] `l` : Limite d'utilisateurs dans le canal

## Gestion des erreurs

- [x] Gérer la perte de connexion client
- [x] Gérer les erreurs de réception/envoi
- [ ] Gérer le manque de mémoire sans crash
- [x] Gérer les commandes invalides
- [x] Envoyer les codes d'erreur IRC appropriés
- [ ] Tester la bande passante faible
- [ ] Tester avec des données partielles

## Nettoyage et déconnexion

- [x] Implémenter la commande QUIT
- [x] Nettoyer les ressources à la déconnexion
- [x] Retirer le client de tous ses canaux
- [x] Notifier les autres utilisateurs
- [x] Fermer proprement les sockets avec `close()`
- [x] Gérer les signaux (SIGINT, SIGTERM) avec `signal()`/`sigaction()`

## Tests et validation

- [ ] Choisir un client IRC de référence (ex: irssi, WeeChat, HexChat)
- [x] Tester la connexion au serveur
- [x] Tester l'authentification
- [x] Tester la création/jointure de canaux
- [x] Tester les messages privés
- [x] Tester les commandes d'opérateur
- [x] Tester avec plusieurs clients simultanés
- [ ] Tester la stabilité (pas de crash, pas de quit inattendu)
- [ ] Vérifier le comportement identique à un serveur IRC officiel

## Code quality

- [x] Respecter la norme C++98
- [ ] Pas de fuite mémoire
- [x] Code propre et bien structuré
- [x] Commentaires appropriés
- [x] Gestion d'erreur robuste
- [x] Pas de forking
- [x] Utiliser les versions C++ des fonctions (<cstring> au lieu de <string.h>)
- [x] Utiliser Replies.hpp pour les codes numériques IRC
- [x] Utiliser Client::getPrefix() pour les préfixes utilisateur

## Bonus (seulement si la partie obligatoire est PARFAITE)

- [ ] Implémenter le transfert de fichiers
- [ ] Créer un bot IRC