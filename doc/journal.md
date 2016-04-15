
# Journal de bord #

## DONE ##

> - ***Version minimale OK***

> - ***Canaux globaux OK***

> - ***Communication par lots OK***

> - ***Canaux synchrones***

> - ***Canaux à copie unique (synchrones et asynchrones)***


## DOING ##

**Luxon**

> - Programme test performance (petites données avec/sans contention,
    grosses données) > faire programme exemple


## TODO ##

**Luxon**

> - ***Canaux non-bloquants*** +  Benchmark

> - Programmer le *forward* en Go (synchrone et asynchrone)

> - Faire le bfwd_chan en multi-processus

> Faire le rapport

**Luxon**, **Somaya**, **Kenny**

> Benchmark

> - Version minimale (finir certains benchmark)

> - Canaux globaux
  Dans un contexte multi-processus. Cela ne sert pas à grand chose de le tester
  en multi-thread, c'est forcément moins efficace.

> - Communication par lots
  Comparer les canaux avec communication par lots avec les cannaux classiques
  dans un contexte multi-thread et multi-processus.

> - Canaux synchrones
  Comparer les canaux synchrones avec les canaux asynchrones dans un
 contexte multi-thread et multi-processus.

> - Canaux à une seule copie
  Comparaison avec les canux classiques en multi-thread et multi-processus

> - Canaux non-bloquants
  Comparaison avec les canaux bloquants classiques


NB 1 : canal classique : canal minimal ou global.

NB 2 : Protocol à suivre pour le benchmark :
        - Utiliser la commande ***time*** ou ***/bin/time***
        - Effectuer une série d'au moins 5 mesures (on peut monter à 8 si possible).
        - On prend la valeur **médiane** de chaque temps (*réel* et *système*)
        - Le temps "*user*" ne sert à rien, pas besoin de le prendre en compte.

NB 3 : Il est important que l'on fasse tous (tous les 3) les benchmarks.
Avec 3 machines différentes, on pourra faire des comparaisons.

NB 4 : Le but de ce benchmark n'est pas seulement de savoir qui est plus rapide.
        Il faut aussi savoir expliquer **en quoi telle implémentation est plus rapide**,
        et ainsi connaitre les éléments qui constituent
        le **goulot d'étranglement** de chaque version.
        Petit élément de base pour expliquer les différences de performances :
        - Quels sont les appels systèmes qui font ramer telle version ?
        - Quels sont les appels standards qui font ramer telle version ?
        - En quoi les canaux (d'un certain types) sont plus/moins efficaces que
        les canaux d'un autre type dans tel contexte ?
        - En quoi tel type de canal fait mieux/moins bien que les tubes dans tel
        contexte d'exécution ?

        N'héitez pas à faire une analyse approfondie de la trace d'exécution du
        programme.

NB 5 : *strace* et *ltrace* sont nos amis (RTFM !).

--
