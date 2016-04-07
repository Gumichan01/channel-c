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


### Programme Forward ###

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

 - ***1 lecteur / 1 écrivain***

 * Version canal        : **real** 0.010s; **user** 0.000s; **sys** 0.020s;
 * Version tube         : **real** 0.025s; **user** 0.004s; **sys** 0.048s;
 * Programme Go equiv.  : *TODO*

 - ***10 lecteurs / 10 écrivains***

 * Version canal        : **real** 0.237s; **user** 0.084s; **sys** 0.464s;
 * Version tube         : **real** 0.183s; **user** 0.000s; **sys** 0.444s;
 * Programme Go equiv.  : *TODO*

 - **100 lecteurs / 100 écrivains***

 * Version canal        : **real** 0.233s; **user** 0.450s; **sys** 0.428s;
 * Version tube         : **real** 1.185s; **user** 0.000s; **sys** 4.368s;
 * Programme Go equiv.  : *TODO*

 - ***1 000 lecteurs / 1 000 écrivains***

 * Version canal        : **real** 0.387s; **user** 0.132s; **sys** 0.728s;
 * Version tube         : **real** 42.039s; **user** 0.060s; **sys** 2:37.962s;
 * Programme Go equiv.  : *TODO*

 - ***5 000 lecteurs / 5 000 écrivains***

 * Version canal        : **real** 3.509s; **user** 0.736s; **sys** 4.396s;
 * Version tube         : **real** 4:32.062s; **user** 0.124s; **sys** 15:02.048s;
 * Programme Go equiv.  : *TODO*

 - ***10 000 lecteurs / 10 000 écrivains***

 * Version canal        : **real** 14.136s; **user** 3.140s; **sys** 37.222s;
 * Version tube         : **real** 10:8.340s; **user** 0.352s; **sys** 27:39.060s;
 * Programme Go equiv.  : *TODO*

*Moralité* : Avec peu de lecteurs/écrivains, la différence de performance est très minime
entre les deux premières implémentations.
A partir de 100 lecteurs/écrivains, les canaux sont plus efficaces que les tubes (cf: tableau).
De plus, on constate que les versions avec les canaux passent moins de temps
en appel système que les version utilisant les tubes.

**Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

|        | 1/1|  10/10| 100/100| 1000/1000|  5000/5000|  10000/10000|
|--------|----|-------|--------|----------|-----------|-------------|
|Facteur | x2 |  ~x1  |   x5   |   x108   |    x77    |     x43     |

*Facteur moyen* : ***~ 40***.

**Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

|        | 1/1|  10/10| 100/100| 1000/1000|  5000/5000|  10000/10000|
|--------|----|-------|--------|----------|-----------|-------------|
|Facteur | x2 |  ~x1  |   x10  |   x354   |    x205   |     x44     |

*Facteur moyen* : ***~ 100***.


##### Plus d'écrivains que de lecteurs (TODO dernier) #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.143s; **user** 0.068s; **sys** 0.256s;
 * Version tube         : **real** 0.053s; **user** 0.008s; **sys** 0.140s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.268s; **user** 0.068s; **sys** 0.480s;
 * Version tube         : **real** 0.124s; **user** 0.004s; **sys** 0.352s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.293s; **user** 0.060s; **sys** 0.504s;
 * Version tube         : **real** 4.519s; **user** 0.012s; **sys** 17.581s;
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 3.168s; **user** 0.184s; **sys** 4.128s;
 * Version tube         : **real** 2:50.995s; **user** 0.084s; **sys** 8:52.313s;*TODO*
 * Programme Go equiv.  : *TODO*

 *Moralité* : Pour un petit nombre d'écrivains, les tubes sont légèrement plus
 efficaces que les canaux en dessous de 100 écrivains (x2).
 En revanche, lorsque le nombre d'écrivains devient plus important,
 les tubes deviennent inefficaces.
 Il s'est d'ailleurs avéré que vers ~250 écrivains,
 les performances de l'implémentation à base de tube était équivalentes à ce que
 faisait les canaux.

##### Plus de lecteurs que d'écrivains (TODO dernier) #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.074s; **user** 0.036s; **sys** 0.100s;
 * Version tube         : **real** 0.180s; **user** 0.008s; **sys** 0.404s;
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.183s; **user** 0.068s; **sys** 0.232s;
 * Version tube         : **real** 1.424s; **user** 0.012s; **sys** 4.168s;
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.263s; **user** 0.076s; **sys** 0.332s;
 * Version tube         : **real** 31.015s; **user** 0.020s; **sys** 1:40.190s;
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 1:0.638s; **user** 6.648s; **sys** 52.083s;
 * Version tube         : **real** 6:46.751s; **user** 0.260s; **sys** 16:46.751s; *TODO*
 * Programme Go equiv.  : *TODO*

*Moralité* : Avec plus de lecteurs que d'écrivans, l'implémentation utilisant
les cansux est bien plus efficaces que l'implémentation utilisant les tubes.

#### Moralité ####

  Sur des groupes de lecteurs/écrivains relativement petits, diffusant une
grande quantité de données, les performances des implémentations avec les canaux
et les tubes sont semblables. En revanche, avec un très grand nombre de threads,
un programme avec une communication inter-thread basée les canaux est bien plus
efficace qu'un programme équivalent basé sur les tubes.

*TODO* Comparaison avec le Go.

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
 * Version socket Unix  : **real** 0.003s; **user** 0.000s; **sys** 0.000s;


- Gros fichier

 * Version canal        : **real** 18.058s; **user** 0.020s; **sys** 2.124s;
 * Version tube         : **real** 18.063s; **user** 0.012s; **sys** 1.668s;
 * Version socket Unix  : **real** 18.432s; **user** 0.012s; **sys** 1.860s;*TODO*


### Programme Copy - Comparaisons ###

- Petit fichier

 * Version canal        : **real** 0.002s; **user** 0.000s; **sys** 0.000s;
 * Version tube         : **real** 0.002s; **user** 0.000s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.003s; **user** 0.000s; **sys** 0.000s;


- Gros fichier

 * Version canal        : **real** 0.097s; **user** 0.032s; **sys** 0.068s;
 * Version tube         : **real** 0.089s; **user** 0.004s; **sys** 0.108s;
 * Version socket Unix  : **real** 0.109s; **user** 0.004s; **sys** 0.072s;


### Programme Forward - Comparaisons ###

##### Autant de lecteurs que d'écrivains #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.026s; **user** 0.008s; **sys** 0.036s;
 * Version tube         : **real** 0.022s; **user** 0.004s; **sys** 0.048s;
 * Programme Go equiv.  : *TODO*

 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.225s; **user** 0.056s; **sys** 0.360s;
 * Version tube         : **real** 0.226s; **user** 0.004s; **sys** 0.484s;
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.367s; **user** 0.040s; **sys** 0.152s;
 * Version tube         : **real** 1.413s; **user** 0.048s; **sys** 3.772s;
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.958s; **user** 0.004s; **sys** 0.648s;
 * Version tube         : **real** 44.224s; **user** 0.064s; **sys** 2:23.929s;
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 10 000 écrivains
 (Pas de test -> la version canal fait planter la machine)


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
chaque programme. Il est important de noter que les options de compilation
fourni lors de la création de l'exécutable Go peuventt influer sur les resultats.


### Programme Forward - Comparaisons ###

*TODO*

### Moralité ###

*TODO*


## Communication par lots ##

### Conditions de test ###

  Içi, on peut aisément effectuer des comparaisons avec les versions utilisant
les tubes dans la mesure où le système fourni des fonctions pour l'envoi de
données par lots (*readv()* et *writev()*). La vérification d'une version Go
pour cela n'a pas encore été faite.  
Pour autant il est intéressant de comparer les performances de cette
implémentation avec les versions *"classiques"* des canaux dans différents
contextes d'exécution.

### Programme Forward - Comparaisons ###

> *TODO*

> - Canaux normaux multi-thread

> - Canaux normaux multi-processus

> - Par lot multi-thread

> - Par lot multi-processus

> - Tube


### Moralité ###

*TODO*

## Conclusion ##

***TODO***

--
