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

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 10 000 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*


#### Plus d'écrivains que de lecteurs ####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*


#### Plus de lecteurs que d'écrivains ####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*

 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version tube         : **real** 0.003s; **user** 0.004s; **sys** 0.000s;
 * Version socket Unix  : *TODO*
 * Programme Go equiv.  : *TODO*


### Conclusion ###

*TODO* nombre de lecteurs = nombre d'écrivains.  
*TODO* nombre de lecteurs < nombre d'écrivains.  
*TODO* nombre de lecteurs > nombre d'écrivains.  










--
