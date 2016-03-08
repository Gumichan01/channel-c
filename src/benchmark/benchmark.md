# Benchmark #

## Programme Cat ##

  Le programme *cat* multithread permet d'afficher le contenu d'un fichier
  en utilisant deux threads :
  - un thread de lecture
  - un thread d'affichage.


### Comparaison ###

- Petit fichier

 * Version canal        : **real** 0.003s; **user**  0.000s; **sys**	0.000s;
 * Version tube         : **real** 0.003s; **user**  0.000s; **sys**	0.000s;
 * Version socket Unix  : **real** 0.005s; **user**  0.004s; **sys**    0.000s;
 * Programme Go equiv.  : *TODO*


- Gros fichier

 * Version canal        : **real** 18.104s; **user**	0.016s; **sys**	2.212s;
 * Version tube         : **real** 18.142s; **user**	0.009s; **sys**	1.980s;
 * Version socket Unix  : **real** 19.880s; **user**    0.008s; **sys** 1.272s;
 * Programme Go equiv.  : *TODO*


### Conclusion ###

Peu de différences quelque soit le fichier donné.
*TODO*

## Programme Copy ##

Le programme *copy* multithread copie un fichier vers un autre.
Il utilise un thread de lecture du fichier source et un
thread d'écriture du fichier destination.

### Comparaison ###

- Petit fichier

 * Version canal        : **real** 0.003s; **user**	0.004s; **sys**	0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : **real** 0.004s; **user** 0.000s; **sys** 0.000s;
 * Programme Go equiv.  : *TODO*


- Gros fichier

 * Version canal        : **real** 0.101s; **user**	0.008s; **sys**	0.124s;
 * Version tube         : **real** 0.093s; **user** 0.016s; **sys** 0.116s;
 * Version socket Unix  : **real** 0.598s; **user** 0.008s; **sys** 0.096s;
 * Programme Go equiv.  : *TODO*


### Conclusion ###

La différence est très minime entre les canaux et les tubes.  
En revanche les performances son bien meilleures que
les versions utilisant les sockets de la famille Unix. *TODO*


## Programme Forward ##

  Le programme *fwd* va permettre la communication entre
un groupe de threads écrivains et un groupe de threads lecteurs.
On ne se péoccupe pas de savoir qui dans le groupe de lecteurs va recevoir
le message. On sait juste que n'importe quel message envoyé par un ecrivain
sera reçu par un lecteur quelconque.

La communication entre ces deux groupes sera assuré par un
thread "transmetteur". Concrétement, tous les écrivains écriront un message
au thread médiateur. Ce thread, qui aura reçu les messages,
les enverra au groupe de lecteurs.

Le message aura la forme (id_processus,contenu_message[16]).  
Le nombre de lecteurs/écrivains sera déterminé par l'utilisateur.  
Un groupe d'écrivains enverra au maximum 30 000 messages.
Un groupe de lecteurs recevra autant de messages que possible.
Le thread transmetteur devra transiter au maximum 15 000 messages.
Au-delà, il ferme les canaux de transmission.
Il y aura toujours un seul thread "transmetteur".  
Aucun message ne sera affiché, et ce, afin d'éviter des résultats parasites.

Pour la version implémentant les canaux, les tests seront effectués avec
au maximum 256 messages par canal, et ceux afin d'avoir
un tampon de taille équivalente à celui d'un tube.


### Comparaison ###

#### Autant de lecteurs que d'écrivains ####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.750s; **user** 0.108s; **sys** 0.268s;
 * Version tube         : **real** 0.716s; **user** 0.028s; **sys** 0.136s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.771s; **user** 0.060s; **sys** 0.252s;
 * Version tube         : **real** 0.718s; **user** 0.028s; **sys** 0.308s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.796s; **user** 0.076s; **sys** 0.508s;
 * Version tube         : **real** 0.989s; **user** 0.052s; **sys** 1.132s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 9.610s; **user** 2.364s; **sys** 12.017s;
 * Version tube         : **real** 42.386s; **user** 0.200s; **sys** 2:20.037s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 10 000 écrivains

 * Version canal        : **real** 1:29.151s; **user** 12.321s; **sys** 2:6.788s;
 * Version tube         : **real** 10:8.340s; **user** 0.352s; **sys** 27:39.060s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

*Moralité* : Avec peu de lecteurs/écrivains, la différence de performance est très minime
entre les deux premières implémentations.
Pour un grand nombre de lecteurs/écrivains, les canaux sont plus efficaces que les tubes.

#### Plus d'écrivains que de lecteurs ####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.717s; **user** 0.068s; **sys** 0.288s;
 * Version tube         : **real** 0.700s; **user** 0.024s; **sys** 0.216s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.753s; **user** 0.092s; **sys** 0.476s;
 * Version tube         : **real** 0.683s; **user** 0.028s; **sys** 0.432s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 4.906s; **user** 0.900s; **sys** 3.576s;
 * Version tube         : **real** 1.897s; **user** 0.020s; **sys** 6.200s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 40.204s; **user** 9.289s; **sys** 26.402s;
 * Version tube         : **real** 2:50.995s; **user** 0.084s; **sys** 8:52.313s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 *Moralité* : Pour un petit nombre d'écrivains, les tubes sont légèrement plus efficaces
 que les canaux, en particuliers pour 1000 écrivains.  
 En revanche, lorsque le nombre d'écrivains devient plus important,
 les tubes deviennent beaucoup moins efficaces.

#### Plus de lecteurs que d'écrivains ####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.756s; **user** 0.088s; **sys** 0.196s;
 * Version tube         : **real** 0.714s; **user** 0.008s; **sys** 0.192s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.771s; **user** 0.060s; **sys** 0.296s;
 * Version tube         : **real** 0.866s; **user** 0.044s; **sys** 0.720s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 4.500s; **user** 0.892s; **sys** 3.652s;
 * Version tube         : **real** 31.379s; **user** 0.144s; **sys** 1:42.442s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 1:0.638s; **user** 6.648s; **sys** 52.083s;
 * Version tube         : **real** 6:46.751s; **user** 0.260s; **sys** 16:46.751s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

*Moralité* : Sur un petit nombre de lecteurs/écrivains le constat est le même
que pour le cas précédent. En revanche, l'implémentation avec les tubes
fait encore pire que le cas précédent dans avec un grand nombre de lecteurs/écrivains.

### Conclusion ###

Sur des groupes de lecteurs/écrivains relativement petits,
diffusant une grande quantité de données
(notamment dans la troisième suites de tests, avec contention),
les performances des implémentations avec les canaux et les tubes sont semblables.
En revanche, avec un très grand nombre de threads, un programme avec une communication
inter-thread basée les canaux est bien plus efficace qu'un programme basé sur les tubes.

*TODO* COmparaison avec le Go.










--
