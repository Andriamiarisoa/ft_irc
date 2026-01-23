# TODO - ft_irc

## Configuration du projet

- [ ] Créer le Makefile avec les règles : `$(NAME)`, `all`, `clean`, `fclean`, `re`
- [ ] Configurer les flags de compilation : `-Wall -Wextra -Werror -std=c++98`
- [ ] Structurer les fichiers sources (*.cpp, *.hpp)
- [ ] Créer fichier de configuration optionnel

## Architecture de base

- [ ] Implémenter la classe Server
- [x] Implémenter la classe Client
- [ ] Implémenter la classe Channel
- [ ] Gérer les arguments : `./ircserv <port> <password>`
- [ ] Valider le port et le mot de passe au démarrage

## Gestion réseau (TCP/IP)

- [ ] Créer le socket serveur avec `socket()`
- [ ] Configurer le socket avec `setsockopt()`
- [ ] Lier le socket au port avec `bind()`
- [ ] Mettre le serveur en écoute avec `listen()`
- [ ] Accepter les connexions avec `accept()`
- [ ] Configurer tous les file descriptors en mode non-bloquant (`fcntl()` pour MacOS)

## Multiplexage I/O

- [ ] Implémenter `select()` (ou équivalent : poll, kqueue, epoll)
- [ ] Gérer multiple clients simultanément sans blocage
- [ ] UN SEUL `select()` pour toutes les opérations (read, write, listen)
- [ ] Préparer les fd_set avec FD_ZERO et FD_SET avant chaque appel
- [ ] Calculer max_fd (plus grand descripteur + 1) pour nfds
- [ ] Vérifier les descripteurs prêts avec FD_ISSET après select()
- [ ] Gérer les timeout avec struct timeval
- [ ] Reconstruire les fd_set à chaque itération (modifiés par select)

## Gestion des messages

- [ ] Parser les commandes IRC reçues
- [ ] Gérer la réception de données partielles (buffer)
- [ ] Reconstruire les commandes complètes à partir des packets
- [ ] Gérer les messages terminés par `\r\n`
- [ ] Tester avec `nc -C` et ctrl+D pour envoyer des données partielles

## Authentification

- [ ] Vérifier le mot de passe de connexion
- [ ] Implémenter la commande PASS
- [x] Implémenter la commande NICK (définir le pseudo)
- [ ] Implémenter la commande USER (définir le username)
- [ ] Gérer l'ordre d'authentification
- [ ] Envoyer les messages de bienvenue (RPL_WELCOME, etc.)

## Gestion des canaux

- [x] Implémenter la commande JOIN (rejoindre un canal)
- [x] Implémenter la commande PART (quitter un canal)
- [x] Créer automatiquement les canaux si inexistants
- [x] Définir le premier utilisateur comme opérateur
- [x] Transmettre les messages à tous les membres du canal
- [x] Gérer la liste des membres par canal

## Messages privés

- [ ] Implémenter la commande PRIVMSG (messages privés)
- [ ] Supporter les messages utilisateur à utilisateur
- [ ] Supporter les messages vers les canaux
- [ ] Vérifier l'existence du destinataire

## Opérateurs de canal

- [ ] Gérer les privilèges d'opérateur (operator vs regular user)
- [ ] Implémenter la commande KICK (éjecter un client)
- [x] Implémenter la commande INVITE (inviter un client)
- [ ] Implémenter la commande TOPIC (changer/voir le topic)
- [ ] Implémenter la commande MODE avec les modes suivants :
  - [ ] `i` : Mode invite-only (invitation seulement)
  - [ ] `t` : Restriction TOPIC aux opérateurs
  - [ ] `k` : Clé du canal (mot de passe)
  - [ ] `o` : Donner/retirer privilège d'opérateur
  - [ ] `l` : Limite d'utilisateurs dans le canal

## Gestion des erreurs

- [ ] Gérer la perte de connexion client
- [ ] Gérer les erreurs de réception/envoi
- [ ] Gérer le manque de mémoire sans crash
- [ ] Gérer les commandes invalides
- [ ] Envoyer les codes d'erreur IRC appropriés
- [ ] Tester la bande passante faible
- [ ] Tester avec des données partielles

## Nettoyage et déconnexion

- [ ] Implémenter la commande QUIT
- [ ] Nettoyer les ressources à la déconnexion
- [ ] Retirer le client de tous ses canaux
- [ ] Notifier les autres utilisateurs
- [ ] Fermer proprement les sockets avec `close()`
- [ ] Gérer les signaux (SIGINT, SIGTERM) avec `signal()`/`sigaction()`

## Tests et validation

- [ ] Choisir un client IRC de référence (ex: irssi, WeeChat, HexChat)
- [ ] Tester la connexion au serveur
- [ ] Tester l'authentification
- [ ] Tester la création/jointure de canaux
- [ ] Tester les messages privés
- [ ] Tester les commandes d'opérateur
- [ ] Tester avec plusieurs clients simultanés
- [ ] Tester la stabilité (pas de crash, pas de quit inattendu)
- [ ] Vérifier le comportement identique à un serveur IRC officiel

## Code quality

- [ ] Respecter la norme C++98
- [ ] Pas de fuite mémoire
- [ ] Code propre et bien structuré
- [ ] Commentaires appropriés
- [ ] Gestion d'erreur robuste
- [ ] Pas de forking
- [ ] Utiliser les versions C++ des fonctions (<cstring> au lieu de <string.h>)

## Bonus (seulement si la partie obligatoire est PARFAITE)

- [ ] Implémenter le transfert de fichiers
- [ ] Créer un bot IRC