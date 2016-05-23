# Benchmark #

## Canaux minimaux ##

### Condition de test ###

- Compilateur utilisé : *gcc*.
- Options gcc : *-Wall -O3 -lm -lrt*.

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
On prendra la valeur médiane de ces mesures.


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

Le message aura la forme (id_processus,contenu_message[64]).
Le nombre de lecteurs/écrivains sera déterminé par l'utilisateur.
Un groupe d'écrivains enverra au maximum 30 000 messages.
Un groupe de lecteurs recevra autant de messages que possible.
Le thread transmetteur devra transiter au maximum 15 000 messages.
Au-delà, il ferme les canaux de transmission.
Il y aura toujours un seul thread "transmetteur".
*NB*: Dans le cadre du benchmark, aucun message relatif au messages reçus
ne sera affiché, à part les messages "Working..." et "End of program",
et ce, afin de minimiser au maximum les résultats parasites.

Pour la version implémentant les canaux,
en prenant en compte la taille de la donnée envoyée via le canal,
les tests seront effectués avec au maximum 256 messages par canal,
et ceux afin d'avoir un tampon de taille équivalente à
celle d'un tube (au moins 8192 octets d'après POSIX).


#### Comparaison ####

##### Autant de lecteurs que d'écrivains #####

 - ***1 lecteur / 1 écrivain***

 * Version canal        : **real** 0.010s; **sys** 0.020s;
 * Version tube         : **real** 0.025s; **sys** 0.048s;


 - ***10 lecteurs / 10 écrivains***

 * Version canal        : **real** 0.204s; **sys** 0.376s;
 * Version tube         : **real** 0.205s; **sys** 0.528s;


 - ***100 lecteurs / 100 écrivains***

 * Version canal        : **real** 0.303s; **sys** 0.544s;
 * Version tube         : **real** 1.205s; **sys** 4.376s;


 - ***1 000 lecteurs / 1 000 écrivains***

 * Version canal        : **real** 0.420s; **sys** 0.660s;
 * Version tube         : **real** 42.843s; **sys** 2:41.842s;


 - ***5 000 lecteurs / 5 000 écrivains***

 * Version canal        : **real** 5.505s; **sys** 8.525s;
 * Version tube         : **real** 4:32.062s; **sys** 15:17.413s;


 - ***10 000 lecteurs / 10 000 écrivains***

 * Version canal        : **real** 21.477s; **sys** 48.899s;
 * Version tube         : **real** 10:8.340s; **sys** 27:39.060s;


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

 * Version canal        : **real** 0.133s; **sys** 0.224s;
 * Version tube         : **real** 0.051s; **sys** 0.148s;


 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.254s; **sys** 0.448s;
 * Version tube         : **real** 0.116s; **sys** 0.332s;


 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.315s; **sys** 0.460s;
 * Version tube         : **real** 4.048s; **sys** 15.553s;


 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 4.115s; **sys** 4.735s;
 * Version tube         : **real** 3:46.253s; **sys** 12:39.359s;


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

 * Version canal        : **real** 0.118s; **sys** 0.220s;
 * Version tube         : **real** 0.182s; **sys** 0.452s;


 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.218s; **sys** 0.492s;
 * Version tube         : **real** 1.424s; **sys** 4.168s;


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.332s; **sys** 0.544s;
 * Version tube         : **real** 31.015s; **sys** 1:40.190s;


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 2.257s; **sys** 2.652s;
 * Version tube         : **real** 5:49.263s; **sys** 19:03.803s;


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
l'on exécute les programmes dans un contexte multi-processus.
Pour autant, les programmes multi-processus utilisant les canaux seront compilés
avec l'option *-pthread*, du fait que les canaux utilisent des fonctions relatives
à ***pthread***.


### Programme Forward - Comparaisons ###

##### Autant de lecteurs que d'écrivains #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.026s; **sys** 0.036s;
 * Version tube         : **real** 0.022s; **sys** 0.048s;


 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.307s; **sys** 0.624s;
 * Version tube         : **real** 0.222s; **sys** 0.548s;


 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.478s; **sys** 0.652s;
 * Version tube         : **real** 1.283s; **sys** 4.168s;


 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.958s; **sys** 0.648s;
 * Version tube         : **real** 44.224s; **sys** 2:23.929s;


 - 10 000 lecteurs / 10 000 écrivains
 (Pas de test -> la version canal fait planter la machine)


 **Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

 |        | 1/1|   10/10| 100/100|  1000/1000|
 |--------|----|--------|--------|-----------|
 |Facteur | ~1 |  x1.3  |   x3   |    x46    |

*Facteur moyen* : ***~ 13***.

 **Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

 |        | 1/1| 10/10|  100/100| 1000/1000|
 |--------|----|------|---------|----------|
 |Facteur | ~1 |  ~1  |    x6   |   x222   |

*Facteur moyen* : ***~ 57***.

*Moralité* : Sur un petit nombre d'écrivains/lecteurs, le programme utilisant
les tubes est légèrement plus efficace que son équivalent avec les canaux.
En revanche, la version tube devient beaucoup moins efficace que la version canal
autour de 1000 écrivains.

##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.234s; **sys** 0.460s;
 * Version tube         : **real** 0.039s; **sys** 0.088s;


 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.411s; **sys** 0.452s;
 * Version tube         : **real** 0.114s; **sys** 0.032s;


 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.710s; **sys** 0.392s;
 * Version tube         : **real** 4.002s; **sys** 12.977s;


 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 4.177s; **sys** 3.368s;
 * Version tube         : **real** 3:34.840s; **sys** 12:1.953s;


 **Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

 |        |    1/10|   1/100| 1/1000|    1/10000|
 |--------|--------|--------|-------|-----------|
 |Facteur | x(1/6) | x(1/3) |   x6  |    x51    |

 *Facteur moyen* : ***~ 15***.

 **Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

 |        |     1/10|    1/100|   1/1000|   1/10000|
 |--------|---------|---------|---------|----------|
 |Facteur |  x(1/5) | x(1/14) |   x33   |   x214   |

*Facteur moyen* : ***~ 60***.


##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.206s; **sys** 0.376s;
 * Version tube         : **real** 0.183s; **sys** 0.432s;


 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.396s; **sys** 0.424s;
 * Version tube         : **real** 1.438s; **sys** 4.908s;


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.735s; **sys** 0.428s;
 * Version tube         : **real** 31.991s; **sys** 1:59.679s;


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 4.107s; **sys** 3.248s;
 * Version tube         : **real** 6:0.354s; **sys** 20:12.560s;


 **Comparatif des différences de performances entre les canaux et les tubes (temps réel)** :

 |        |    1/10| 1/100| 1/1000|    1/10000|
 |--------|--------|------|-------|-----------|
 |Facteur | x(1/5) |  x4  |  x43  |    x87    |

 *Facteur moyen* : ***~ 34***.

 **Comparatif des différences de performances entre les canaux et les tubes (temps système)** :

 |        |   1/10|  1/100|   1/1000|   1/10000|
 |--------|-------|-------|---------|----------|
 |Facteur |  x1.1 |  x11  |   x33   |   x279   |

*Facteur moyen* : ***~ 80***.

> En moyenne (de tous les cas)

*Facteur moyen (temps réel)* : ***~ 20***.
*Facteur moyen (temps système)* : ***~ 66***.


### Moralité ###

  Dans un contexte multi-processus, le programme utilisant les canaux globaux est,
dans l'ensemble, plus efficaces que le programme équivalent avec les tubes.


## Canaux synchrones ##

### Conditions de test ###

  Içi seront testées les performances des canaux synchrones
avec celles les canaux asynchrones, et ceux, dans un contexte multi-thread.
Ce tpe de canal ne fonctionne pas en multi-processus.
Seule la communication entre un écrivain et le transmetteur est synchrone.
La communication via le canal des lecteurs reste quant à lui asynchrone.


### Programme Forward - Comparaisons ###


|Ecrivains/Lecteurs|Asynchrone (temps réel)|Asynchrone (temps système)|Synchrone (temps réel)| Synchrone (temps système) |
|------------------|-----------------------|--------------------------|-----------------------|--------------------------|
|1/1               |         0.015s        |        **0.012s**        |         1.100s        |         **0.940s**       |
|10/10             |         0.271s        |        **0.556s**        |         1.332s        |         **1.624s**       |
|100/100           |         0.304s        |        **0.568s**        |         1.351s        |         **1.720s**       |
|1000/1000         |         0.442s        |        **0.660s**        |         1.500s        |         **1.808s**       |
|10000/10000       |        22.172s        |       **51.895s**        |        25.311s        |        **59.940s**       |
|10/1              |         0.117s        |        **0.208s**        |         1.228s        |         **1.308s**       |
|100/1             |         0.310s        |        **0.544s**        |         1.250s        |         **1.332s**       |
|1000/1            |         0.327s        |        **0.552s**        |         1.303s        |         **1.456s**       |
|10000/1           |         3.399s        |        **4.192s**        |         7.164s        |        **11.940s**       |
|1/10              |         0.113s        |        **0.232s**        |         1.232s        |         **1.184s**       |
|1/100             |         0.259s        |        **0.440s**        |         1.257s        |         **1.344s**       |
|1/1000            |         0.329s        |        **0.544s**        |         1.290s        |         **1.480s**       |
|1/10000           |         4.437s        |        **6.396s**        |         6.043s        |         **8.905s**       |


**Comparatif des différences de performances entre les canaux synchrones et asynchrones (temps réel)** :

| Ecrivain ø Lecteurs |   ==  | > |  <  |
|---------------------|-------|---|-----|
|       Facteur       |  x17  | x5|  x8 |

*Facteur moyen* : ***~ 10***.

**Comparatif des différences de performances entre les canaux synchrones et asynchrones (temps système)** :

| Ecrivain ø Lecteurs |   ==  | > |  <  |
|---------------------|-------|---|-----|
|       Facteur       |  x17  | x4|  x3 |

*Facteur moyen* : ***~ 8***.


### Moralité ###

  Les canaux synchrones ne sont pas plus efficaces que les canaux asynchrones
vis-à-vis du programme test.


## Communication par lots ##

### Précisions concernant la communication par lot ###

  Les canaux avec communication par lots sont conçus de manière à envoyer
une suite de données en un appel de fonction.
Dans le cas présent, une interface spécifique à la communication par lots a
été défini.

### Conditions de test ###

  Içi, seront comparées les performances des canaux avec l'interface de
communication par lots avec les canaux *"classiques"* dans différents
contextes d'exécution. Il est important de noter que l'envoi de données
par le transmeteur au lecteur se fait par lots, tandis que
la lecture se fait normalement.
La communication ecrivain/transmetteur reste classique.

### Programme Forward - Comparaisons ###

#### Multi-thread ####

|Ecrivains/Lecteurs|Normaux (temps réel)|Normaux (temps système)|Par lots (temps réel)|Par lots (temps système)|
|------------------|--------------------|-----------------------|---------------------|------------------------|
|***1/1***         |         0.015s     |        **0.012s**     |        0.013s       |        **0.004s**      |
|10/10             |         0.271s     |        **0.556s**     |        0.076s       |        **0.104s**      |
|100/100           |         0.304s     |        **0.568s**     |        0.315s       |        **0.480s**      |
|1000/1000         |         0.442s     |        **0.660s**     |        0.458s       |        **0.608s**      |
|10000/10000       |        22.172s     |       **51.895s**     |       22.535s       |       **54.891s**      |
|10/1              |         0.117s     |        **0.208s**     |        0.069s       |        **0.120s**      |
|100/1             |         0.310s     |        **0.544s**     |        0.313s       |        **0.476s**      |
|1000/1            |         0.327s     |        **0.552s**     |        0.308s       |        **0.468s**      |
|10000/1           |         3.399s     |        **4.192s**     |        4.207s       |        **5.664s**      |
|***1/10***        |         0.113s     |        **0.232s**     |        0.012s       |        **0.016s**      |
|***1/100***       |         0.259s     |        **0.440s**     |        0.018s       |        **0.028s**      |
|***1/1000***      |         0.329s     |        **0.544s**     |        0.047s       |        **0.044s**      |
|***1/10000***     |         4.437s     |        **6.396s**     |        3.310s       |        **4.340s**      |

  Dans l'ensemble, la communication par lots ne semble pas apporter un gain
important en terme de performance. Pour autant, en observant les cas où
l'on a un seul thread écrivain,on constate un gain de performace non négligeable
des canaux avec communication par lots par rapports au canaux normaux.
On sait que le transmetteur renvoie les messages aux lecteurs si, et seulement si,
son tableau de messages à envoyer est plein.
On sait aussi que les lecteurs, doivent attendre une donnée disponible
en s'endormant sur une variable de conition. Au vu du fait que l'envoi de message
aux lecteurs se fait par lots, l'essentiel du travail d'envoi se fait au niveau
de l'écrivain. De ce fait, plus il y a d'écrivains, plus la performance diminue.
Par conséquent, le nombre total d'envoi de données est réduit.
De plus, le fait qu'il y ait un seul thread écrivain limite grandement
les situations de concurrence à l'envoi des données au transmetteur.


#### Multi-processus ####

|Ecrivains/Lecteurs|Normaux (temps réel)|Normaux (temps système)|Par lots (temps réel)|Par lots (temps système)|
|------------------|--------------------|-----------------------|---------------------|------------------------|
|1/1               |         0.026s     |        **0.036s**     |        0.026s       |        **0.032s**      |
|10/10             |         0.307s     |        **0.624s**     |        0.116s       |        **0.208s**      |
|100/100           |         0.478s     |        **0.652s**     |        0.395s       |        **0.416s**      |
|1000/1000         |         0.958s     |        **0.648s**     |        1.043s       |        **0.560s**      |
|10/1              |         0.234s     |        **0.460s**     |        0.101s       |        **0.152s**      |
|100/1             |         0.411s     |        **0.452s**     |        0.422s       |        **0.388s**      |
|1000/1            |         0.710s     |        **0.392s**     |        0.753s       |        **0.340s**      |
|10000/1           |         4.177s     |        **3.368s**     |        4.238s       |        **3.292s**      |
|***1/10***        |         0.206s     |        **0.376s**     |        0.027s       |        **0.028s**      |
|1/100             |         0.396s     |        **0.424s**     |        0.422s       |        **0.480s**      |
|1/1000            |         0.735s     |        **0.428s**     |        0.732s       |        **0.388s**      |
|1/10000           |         4.107s     |        **3.248s**     |        4.316s       |        **3.288s**      |

  Les différences sont très minimes dans l'ensemble entre les canaux avec
communication par lots et les canaux normaux. En revanche, on constate que dans
le cas d'un petit nombre d'écrivains/lecteurs que la communication par lots est
plus efficace que la communication basique.


### Moralité ###

Pour un petit nombre de threads/processus, les canaux avec communication
par lots sont plus efficaces que les canaux classiques (x2). Pour un grand nombre
de threads/processus, la différence est très minime entre les deux implémentations.


## Canaux à une seule copie ##

### Précisions concernant les canaux à une seule copie ###

Les canaux basique fournis effectuait en interne deux copies
(ecrivain → canal et canal → lecteur). Les canaux à une seule copie suppriment
une de ces deux copies.
Dans le cas présent, l'adresse de la donnée est directement stockée dans le canal.


### Conditions de test ###

  Içi, seront comparées les performances des canaux à une seule copie
avec les canaux *"classiques"* dans un contexte multi-thread.
les deux canaux sont à une seule copie.

### Programme Forward - Comparaisons ###

|Ecrivains/Lecteurs|Normaux (temps réel)|Normaux (temps système)| 1 copie (temps réel)| 1 copie (temps système)|
|------------------|--------------------|-----------------------|---------------------|------------------------|
|1/1               |         0.015s     |        **0.012s**     |        0.016s       |        **0.020s**      |
|10/10             |         0.271s     |        **0.556s**     |        0.270s       |        **0.500s**      |
|100/100           |         0.304s     |        **0.568s**     |        0.306s       |        **0.552s**      |
|1000/1000         |         0.442s     |        **0.660s**     |        0.416s       |        **0.580s**      |
|10000/10000       |        22.172s     |       **51.895s**     |       22.073s       |       **50.767s**      |
|10/1              |         0.117s     |        **0.208s**     |        0.098s       |        **0.160s**      |
|100/1             |         0.310s     |        **0.544s**     |        0.308s       |        **0.484s**      |
|1000/1            |         0.327s     |        **0.552s**     |        0.343s       |        **0.532s**      |
|10000/1           |         3.399s     |        **4.192s**     |        5.608s       |        **8.769s**      |
|1/10              |         0.113s     |        **0.232s**     |        0.115s       |        **0.196s**      |
|1/100             |         0.259s     |        **0.440s**     |        0.295s       |        **0.508s**      |
|1/1000            |         0.329s     |        **0.544s**     |        0.336s       |        **0.516s**      |
|1/10000           |         4.437s     |        **6.396s**     |        4.917s       |        **7.364s**      |

Peu de différence entre les deux implémentations.

## Canaux non-bloquants ##

### Précisions concernant les canaux non-bloquants ###

Les canaux non-bloquants permettent, comme leur nom l'indique, d'effectuer une
communication inter-thread/inter-processus non bloquante.
La définition de ce type de communication implique le développement d'une fonction
permettant d'effectuer une attente "passive" de la disponibilité d'un ensemble de
canaux.


### Conditions de test ###

  Içi, seront comparées les performances des canaux non-bloquants avec les canaux
bloquants *"classiques"* dans le programme forward multi-thread.
Les deux canaux du programmee sont non-bloquants.


### Programme Forward - Comparaisons ###

|Ecrivains/Lecteurs|Normaux (temps réel)|Normaux (temps système)| Non-bloquant (réel) | Non-bloquant (système) |
|------------------|--------------------|-----------------------|---------------------|------------------------|
|1/1               |         0.015s     |        **0.012s**     |        0.640s       |        **0.008s**      |
|10/10             |         0.271s     |        **0.556s**     |        0.623s       |        **0.032s**      |
|100/100           |         0.304s     |        **0.568s**     |        0.648s       |        **0.104s**      |
|1000/1000         |         0.442s     |        **0.660s**     |        0.415s       |        **0.616s**      |
|10/1              |         0.117s     |        **0.208s**     |        0.013s       |        **0.000s**      |
|100/1             |         0.310s     |        **0.544s**     |        0.018s       |        **0.016s**      |
|1000/1            |         0.327s     |        **0.552s**     |        0.046s       |        **0.044s**      |
|10000/1           |         3.399s     |        **4.192s**     |        0.355s       |        **0.444s**      |
|1/10              |         0.113s     |        **0.232s**     |        0.640s       |        **0.000s**      |
|1/100             |         0.259s     |        **0.440s**     |        0.643s       |        **0.020s**      |
|1/1000            |         0.329s     |        **0.544s**     |        0.648s       |        **0.452s**      |

  On constate que les canaux non-bloquants sont moins efficaces que les canaux
bloquants.


## Conclusion ##

  Les canaux, dans leur version la plus minimale, sont beaucoup plus performants
que les tubes, aussi bien dans un contexte multi-thread que multi-processus.
Certaines extensions permettent, si elles sont utilisées à bon escient,
d'améliorer ces performances (la communication par lots, les canaux non-bloquants).

--
