# Benchmark #

## Canaux minimaux ##

### Condition de test ###

- Compilateur utilisé : *gcc*.
- Options gcc : *-Wall -O3*.

  Sauf mention contraire, tous les tests sont effectué avec
l'option d'optimisation *-O3*.
Pour les programme multithreads, l'option *-pthread* a été ajouté.
Pour le programme équivalent en Go,... *TODO*

  Les tests on été effectués sur sur machine x86-64 avec
un processeur Intel Core I3 quadri-coeurs à 2.53GHz, 4 Gio de mémoire vive,
sur la distribution Ubuntu 12.04 64bits.
Une machine témoin *TODO configuration machine* a également été utilisée pour
affiner les résultats.  
En terme de calcul, la commande */bin/time* est utilisée. Mais un script sera utilisé
afin d'automatiser les tests.
Afin d'éviter les fausses mesures inhérents à la politique d'ordonnancement
du noyau, une série de 8 mesures est effectué.
Seul le meilleur résultat est retenu.


### Programme Cat ###

  Le programme *cat* multithread permet d'afficher le contenu d'un fichier
en utilisant deux threads :
  - un thread de lecture
  - un thread d'affichage.


#### Comparaison ####

- Petit fichier

 * Version canal        : **real** 0.003s; **user** 0.000s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.000s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.005s; **user** 0.004s; **sys** 0.000s;


- Gros fichier

 * Version canal        : **real** 18.104s; **user** 0.016s; **sys** 2.212s;
 * Version tube         : **real** 18.142s; **user** 0.009s; **sys** 1.980s;
 * Version socket Unix  : **real** 18.187s; **user** 0.008s; **sys** 2.020s;


#### Moralité ####

  Peu de différences quelque soit le fichier donné. Cette suite de tests n'importe
rien de très pertinent.

### Programme Copy ###

  Le programme *copy* multithread copie un fichier vers un autre.
Il utilise un thread de lecture du fichier source et un
thread d'écriture du fichier destination.

#### Comparaison ####

- Petit fichier

 * Version canal        : **real** 0.003s; **user**	0.004s; **sys**	0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.004s; **user** 0.000s; **sys** 0.000s;


 - Gros fichier

  * Version canal        : **real** 0.063s; **user** 0.004s; **sys** 0.084s;
  * Version tube         : **real** 0.057s; **user** 0.008s; **sys** 0.068s;
  * Version socket Unix  : **real** 0.061s; **user** 0.004s; **sys** 0.072s;


#### Moralité ####

Même constat.


### Programme Forward (***TODO refaire le Benchmark***) ###

  Le programme *fwd* va permettre la communication entre
un groupe de threads écrivains et un groupe de threads lecteurs.
On ne se péoccupe pas de savoir qui dans le groupe de lecteurs va recevoir
le message. On sait juste que n'importe quel message envoyé par un ecrivain
sera reçu par un lecteur quelconque.

La communication entre ces deux groupes sera assuré par un
thread "transmetteur". Concrétement, tous les écrivains écriront un message
au thread transmetteur. Ce thread, qui aura reçu les messages,
les enverra au groupe de lecteurs.

Le message aura la forme (id_processus,contenu_message[16]).  
Le nombre de lecteurs/écrivains sera déterminé par l'utilisateur.  
Un groupe d'écrivains enverra au maximum 30 000 messages.
Un groupe de lecteurs recevra autant de messages que possible.
Le thread transmetteur devra transiter au maximum 15 000 messages.
Au-delà, il ferme les canaux de transmission.
Il y aura toujours un seul thread "transmetteur".  
*NB*: Dans le cadre du benchmark, aucun message relatif au messages reçus
ne sera affiché, à part les messages "Working..." et "End of program",
et ce, afin d'éviter des résultats parasites.

Pour la version implémentant les canaux, les tests seront effectués avec
au maximum 256 messages par canal, et ceux afin d'avoir
un tampon de taille équivalente à celui d'un tube (au moins 8192 d'après POSIX).


#### Comparaison ####

##### Autant de lecteurs que d'écrivains #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.010s; **user** 0.000s; **sys** 0.020s;
 * Version tube         : **real** 0.025s; **user** 0.004s; **sys** 0.048s;
 * Programme Go equiv.  : *TODO*

 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.194s; **user** 0.052s; **sys** 0.364s;
 * Version tube         : **real** 0.183s; **user** 0.000s; **sys** 0.444s;
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.233s; **user** 0.450s; **sys** 0.428s;
 * Version tube         : **real** 1.185s; **user** 0.000s; **sys** 4.368s;
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.387s; **user** 0.132s; **sys** 0.728s;
 * Version tube         : **real** 42.039s; **user** 0.060s; **sys** 2:37.962s;
 * Programme Go equiv.  : *TODO*

 - 5 000 lecteurs / 5 000 écrivains

 * Version canal        : **real** 3.509s; **user** 0.736s; **sys** 7.396s;
 * Version tube         : **real** 4:32.062s; **user** 0.124s; **sys** 15:02.048s;
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 10 000 écrivains

 * Version canal        : **real** 14.136s; **user** 3.140s; **sys** 37.222s;
 * Version tube         : **real** 10:8.340s; **user** 0.352s; **sys** 27:39.060s;
 * Programme Go equiv.  : *TODO*

*Moralité* : Avec peu de lecteurs/écrivains, la différence de performance est très minime
entre les deux premières implémentations.
Pour un grand nombre de lecteurs/écrivains, les canaux sont plus efficaces que les tubes.
De plus, on constate que les versions avec les canaux passent moins de temps
en appel système que les version utilisant les tubes.

##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.717s; **user** 0.068s; **sys** 0.288s;
 * Version tube         : **real** 0.700s; **user** 0.024s; **sys** 0.216s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.753s; **user** 0.092s; **sys** 0.476s;
 * Version tube         : **real** 0.683s; **user** 0.028s; **sys** 0.432s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 4.906s; **user** 0.900s; **sys** 3.576s;
 * Version tube         : **real** 1.897s; **user** 0.020s; **sys** 6.200s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 40.204s; **user** 9.289s; **sys** 26.402s;
 * Version tube         : **real** 2:50.995s; **user** 0.084s; **sys** 8:52.313s;
 * Programme Go equiv.  : *TODO*

 *Moralité* : Pour un petit nombre d'écrivains, les tubes sont légèrement plus
 efficaces que les canaux, en particuliers pour 1000 écrivains.
 Cependant, cela se fait  au prix d'un temps plus important
 consacré aux appels systèmes.  
 En revanche, lorsque le nombre d'écrivains devient plus important,
 les tubes deviennent inefficaces.
 Il s'est d'ailleurs avéré qu'à partir de ~1500 écrivains,
 les performances de l'implémentation à base de tube s'éffondraient.

##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.756s; **user** 0.088s; **sys** 0.196s;
 * Version tube         : **real** 0.714s; **user** 0.008s; **sys** 0.192s;
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.771s; **user** 0.060s; **sys** 0.296s;
 * Version tube         : **real** 0.866s; **user** 0.044s; **sys** 0.720s;
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 4.500s; **user** 0.892s; **sys** 3.652s;
 * Version tube         : **real** 31.379s; **user** 0.144s; **sys** 1:42.442s;
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 1:0.638s; **user** 6.648s; **sys** 52.083s;
 * Version tube         : **real** 6:46.751s; **user** 0.260s; **sys** 16:46.751s;
 * Programme Go equiv.  : *TODO*

*Moralité* : Sur un petit nombre de lecteurs/écrivains le constat est le même
que pour le cas précédent. En revanche, l'implémentation avec les tubes
fait encore pire que le cas précédent dans avec un grand nombre de lecteurs.

#### Moralité ####

  Sur des groupes de lecteurs/écrivains relativement petits,
diffusant une grande quantité de données
(notamment dans la troisième suites de tests, avec contention),
les performances des implémentations avec les canaux et les tubes sont semblables.
En revanche, avec un très grand nombre de threads, un programme avec une communication
inter-thread basée les canaux est bien plus efficace qu'un programme basé sur les tubes.

*TODO* COmparaison avec le Go.

## Canaux globaux ##

### Conditions de test ###

  Les conditions de tests sont identiques. La seule différence vient du fait que
l'on exécute les programmes dans un contexte multi-processus. De plus, il n'y a
de comparaison avec les implémentations en Go.    
Pour autant, les programmes multi-processu utilisant les canaux seront compilés
avec l'option *-pthread*

### Programme Cat - Comparaisons ###

- Petit fichier

 * Version canal        : **real** 0.004s; **user** 0.000s; **sys** 0.004s;
 * Version tube         : **real** 0.003s; **user** 0.000s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.000s; **user** 0.000s; **sys** 0.000s;*TODO*


- Gros fichier

 * Version canal        : **real** 18.058s; **user** 0.020s; **sys** 2.124s;
 * Version tube         : **real** 18.063s; **user** 0.012s; **sys** 1.668s;
 * Version socket Unix  : **real** 0.000s; **user** 0.000s; **sys** 0.000s;*TODO*


### Programme Copy - Comparaisons ###

- Petit fichier

 * Version canal        : **real** 0.002s; **user** 0.000s; **sys** 0.000s;
 * Version tube         : **real** 0.002s; **user** 0.000s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.000s; **user** 0.000s; **sys** 0.000s;*TODO*


- Gros fichier

 * Version canal        : **real** 0.097s; **user** 0.032s; **sys** 0.068s;
 * Version tube         : **real** 0.089s; **user** 0.004s; **sys** 0.108s;
 * Version socket Unix  : **real** 0.000s; **user** 0.000s; **sys** 0.000s;*TODO*


### Programme Forward - Comparaisons ###

##### Autant de lecteurs que d'écrivains #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.026s; **user** 0.008s; **sys** 0.036s;
 * Version tube         : **real** 0.019s; **user** 0.000s; **sys** 0.036s;
 * Programme Go equiv.  : *TODO*

 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.948s; **user** 0.280s; **sys** 2.288s;
 * Version tube         : **real** 0.173s; **user** 0.024s; **sys** 0.416s;
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.470s; **user** 0.400s; **sys** 0.620s;
 * Version tube         : **real** 1.185s; **user** 0.012s; **sys** 3.876s;
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.958s; **user** 0.004s; **sys** 0.648s;
 * Version tube         : **real** 44.215s; **user** 0.064s; **sys** 2:43.538s;
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 10 000 écrivains
 (Pas de test -> la version canal fait planter le machine)


##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 100 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 1 000 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 10 000 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*


##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*
 * Programme Go equiv.  : *TODO*


### Moralité ###

*TODO*


## Canaux synchrones ##

### Conditions de test ###

  Il est difficile de faire un benchmark sur ce type de canal dans la mesure où
les éléments de comparaisons s'utilisent de manière asynchrone. Le seul élément
de comparaison sur lequel le benchmark peut s'appuyer est la version Go de
chaque programme. Il est important de noter que selon les options de compilation
fourni lors de la création de l'exécutable Go peut influer sur les resultats.

### Programme Cat - Comparaisons ###

*TODO*

### Programme Copy - Comparaisons ###

*TODO*

### Programme Forward - Comparaisons ###

*TODO*

### Moralité ###

*TODO*


## Communication par lots ##

### Conditions de test ###

*TODO*

### Programme Cat - Comparaisons ###

*TODO*

### Programme Copy - Comparaisons ###

*TODO*

### Programme Forward - Comparaisons ###

*TODO*


### Moralité ###

*TODO*

## Conclusion ##

***TODO***

--
