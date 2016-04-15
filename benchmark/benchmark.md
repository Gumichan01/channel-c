# Benchmark #

## Canaux minimaux ##

### Condition de test ###

- Compilateur utilisé : *gcc*.
- Options gcc : *-Wall -O3*.

  Sauf mention contraire, tous les tests sont effectué avec
l'option d'optimisation *-O3*.
Pour les programme multithreads, l'option *-pthread* a été ajouté.

  Les tests on été effectués sur une machine x86-64 avec
un processeur Intel Core I3 quadri-coeurs à 2.53GHz, 4 Gio de mémoire vive,
sur la distribution Ubuntu 12.04 64bits.
Deux machines témoins ont également été utilisées pour
affiner les résultats.  
En terme de calcul, la commande */bin/time* est utilisée. Mais un script sera utilisé
afin d'automatiser les tests.
Afin d'éviter les fausses mesures inhérents à la politique d'ordonnancement
du noyau, une série de 8 mesures est effectué.
Seul le meilleur résultat est retenu.


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


 - ***10 lecteurs / 10 écrivains***

 * Version canal        : **real** 0.204s; **user** 0.048s; **sys** 0.376s;
 * Version tube         : **real** 0.205s; **user** 0.000s; **sys** 0.528s;


 - ***100 lecteurs / 100 écrivains***

 * Version canal        : **real** 0.303s; **user** 0.100s; **sys** 0.544s;
 * Version tube         : **real** 1.205s; **user** 0.000s; **sys** 4.376s;


 - ***1 000 lecteurs / 1 000 écrivains***

 * Version canal        : **real** 0.420s; **user** 0.112s; **sys** 0.660s;
 * Version tube         : **real** 42.843s; **user** 0.048s; **sys** 2:41.842s;


 - ***5 000 lecteurs / 5 000 écrivains***

 * Version canal        : **real** 5.505s; **user** 0.084s; **sys** 8.525s;
 * Version tube         : **real** 4:32.062s; **user** 0.124s; **sys** 15:17.413s;


 - ***10 000 lecteurs / 10 000 écrivains***

 * Version canal        : **real** 21.477s; **user** 0.204s; **sys** 48.899s;
 * Version tube         : **real** 10:8.340s; **user** 0.352s; **sys** 27:39.060s;


*Moralité* : Avec peu de lecteurs/écrivains, la différence de performance est très minime
entre les deux premières implémentations.
A partir de 100 lecteurs/écrivains, les canaux sont plus efficaces que les tubes (cf: tableau).
De plus, on constate que les versions avec les canaux passent moins de temps
en appel système que les version utilisant les tubes.

**Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

|        | 1/1|  10/10| 100/100| 1000/1000|  5000/5000|  10000/10000|
|--------|----|-------|--------|----------|-----------|-------------|
|Facteur | x2 |  ~x1  |   x3   |   x102   |    x49    |     x28     |

*Facteur moyen* : ***~ 30***.

**Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

|        | 1/1|   10/10|  100/100| 1000/1000|  5000/5000|  10000/10000|
|--------|----|--------|---------|----------|-----------|-------------|
|Facteur | x2 |  x1.5  |    x8   |   x245   |    x108   |     x34     |

*Facteur moyen* : ***~ 70***.


##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.133s; **user** 0.020s; **sys** 0.224s;
 * Version tube         : **real** 0.051s; **user** 0.004s; **sys** 0.148s;


 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.254s; **user** 0.048s; **sys** 0.448s;
 * Version tube         : **real** 0.116s; **user** 0.016s; **sys** 0.332s;


 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.315s; **user** 0.136s; **sys** 0.460s;
 * Version tube         : **real** 4.048s; **user** 0.008s; **sys** 15.553s;


 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 4.115s; **user** 0.10s; **sys** 4.735s;
 * Version tube         : **real** 3:46.253s; **user** 0.052s; **sys** 12:39.359s;


 *Moralité* : Pour un petit nombre d'écrivains, les tubes sont légèrement plus
 efficaces que les canaux en dessous de 100 écrivains (x2).
 En revanche, lorsque le nombre d'écrivains devient plus important,
 les tubes deviennent inefficaces.


 **Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

 |        |     1/10|      1/100|    1/1000|      1/10000|
 |--------|---------|-----------|----------|-------------|
 |Facteur | ~x(1/3) |   x(1/2)  |   x12    |     x28     |

 *Facteur moyen* : ***~ 10***.

 **Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

 |        |      1/10|     1/100|     1/1000|     1/10000|
 |--------|----------|----------|-----------|------------|
 |Facteur |  ~x(1/3) |    ~x1   |    x34    |    x160    |

*Facteur moyen* : ***~ 40-50***.

##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.118s; **user** 0.040s; **sys** 0.220s;
 * Version tube         : **real** 0.182s; **user** 0.004s; **sys** 0.452s;


 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.218s; **user** 0.060s; **sys** 0.492s;
 * Version tube         : **real** 1.424s; **user** 0.012s; **sys** 4.168s;


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.332s; **user** 0.096s; **sys** 0.544s;
 * Version tube         : **real** 31.015s; **user** 0.020s; **sys** 1:40.190s;


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 2.257s; **user** 0.112s; **sys** 2.652s;
 * Version tube         : **real** 5:49.263s; **user** 0.148s; **sys** 19:03.803s;


*Moralité* : Avec plus de lecteurs que d'écrivans, le programme utilisant
les canaux est bien plus efficace que celui utilisant les tubes.

**Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

|        |  10/1| 100/1|   1000/1|     10000/1|
|--------|------|------|---------|------------|
|Facteur | x1.5 |  x6  |   x90   |    x154    |

*Facteur moyen* : ***~ 62***.

**Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

|        | 10/1|    100/1|     1000/1|     10000/1|
|--------|-----|---------|-----------|------------|
|Facteur |  x2 |    x8   |    x184   |    x431    |

*Facteur moyen* : ***~ 156***.

> En moyenne (de tous les cas)

 *Facteur moyen (temps réel)* : ***~ 30-40***.
 *Facteur moyen (temps système)* : ***~ 100***.

#### Moralité ####

  Sur des groupes de lecteurs/écrivains relativement petits, diffusant une
grande quantité de données, les performances des implémentations avec les canaux
et les tubes sont semblables. En revanche, avec un très grand nombre de threads,
un programme avec une communication inter-thread basée les canaux est bien plus
efficace qu'un programme équivalent basé sur les tubes.


## Canaux globaux ##

### Conditions de test ###

  Les conditions de tests sont identiques. La seule différence vient du fait que
l'on exécute les programmes dans un contexte multi-processus. De plus, il n'y a
de comparaison avec les implémentations en Go.  
Pour autant, les programmes multi-processus utilisant les canaux seront compilés
avec l'option *-pthread*


### Programme Forward - Comparaisons ###

##### Autant de lecteurs que d'écrivains (TODO ↓) #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.026s; **user** 0.008s; **sys** 0.036s;
 * Version tube         : **real** 0.022s; **user** 0.004s; **sys** 0.048s;


 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.225s; **user** 0.056s; **sys** 0.360s;
 * Version tube         : **real** 0.226s; **user** 0.004s; **sys** 0.484s;


 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.367s; **user** 0.040s; **sys** 0.152s;
 * Version tube         : **real** 1.413s; **user** 0.048s; **sys** 3.772s;


 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.958s; **user** 0.004s; **sys** 0.648s;
 * Version tube         : **real** 44.224s; **user** 0.064s; **sys** 2:23.929s;


 - 10 000 lecteurs / 10 000 écrivains
 (Pas de test -> la version canal fait planter la machine)


##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 1 lecteur / 100 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 1 lecteur / 1 000 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 1 lecteur / 10 000 écrivains

 * Version canal        : *TODO*
 * Version tube         : *TODO*



##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 100 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : *TODO*
 * Version tube         : *TODO*



### Moralité ###

*TODO*


## Canaux synchrones ##

### Conditions de test ###

  Il est difficile de faire un benchmark sur ce type de canal dans la mesure où
les éléments de comparaisons s'utilisent de manière asynchrone. Le seul élément
de comparaison sur lequel le benchmark peut s'appuyer est la version Go de
chaque programme. Il est important de noter que les options de compilation
fourni lors de la création de l'exécutable Go peuvent influer sur les resultats.
Un point intéressant serait de comparer les performance des canaux synchrones
avec celles les canaux asynchrones, et ceux, aussi bien dans un contexte
multi-thread que dans un contexte multi-processus.


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
