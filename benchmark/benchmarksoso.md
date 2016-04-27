# Benchmark Somaya #

### Condition de test ###

Les tests ont été effectués sur une machine x86-64 avec 
un processeur Intel Core i5 quadri-coeurs à 2.30GHz, 4 Gio de mémoire vive,
sur la distribution Ubuntu 12.04 64bits.
On effectuera 5 mesures différentes et seule la meilleure sera retenue.

## Canaux minimaux ##

# Programmes fwd pour les threads #

##### Autant de lecteurs que d'écrivains #####

 - ***1 lecteur / 1 écrivain***

 * Version canal        : **real** 0.019s; **sys** 0.024s;
 * Version tube         : **real** 0.026s; **sys** 0.040s;


 - ***10 lecteurs / 10 écrivains***

 * Version canal        : **real** 0.078s; **sys** 0.152s;
 * Version tube         : **real** 0.232s; **sys** 0.780s;


 - ***100 lecteurs / 100 écrivains***

 * Version canal        : **real** 0.113s; **sys** 0.212s;
 * Version tube         : **real** 1.960s; **sys** 7.324s;


 - ***1 000 lecteurs / 1 000 écrivains***

 * Version canal        : **real** 0.237s; **sys** 0.344s;
 * Version tube         : **real** 32.426s; **sys** 1:57.539s;


 - ***5 000 lecteurs / 5 000 écrivains***

 * Version canal        : **real** 3.225s; **sys** 3.584s;
 * Version tube         : **real** 2:58.405s; **sys** 8:30.792s;


 - ***10 000 lecteurs / 10 000 écrivains***

 * Version canal        : **real** 11.954s; **sys** 21.537s;
 * Version tube         : **real** 6:28.249s; **sys** 14:21.866s;



##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.257s; **sys** 0.524s;
 * Version tube         : **real** 0.059s; **sys** 0.192s;


 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.289s; **sys** 0.534s;
 * Version tube         : **real** 0.450s; **sys** 1.724s;


 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.238s; **sys** 0.388s;
 * Version tube         : **real** 6.290s; **sys** 24.162s;


 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 2.804s; **sys** 2.872s;
 * Version tube         : **real** 3:34.264s; **sys** 9:15:943s;

##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.253s; **sys** 0.480s;
 * Version tube         : **real** 0.170s; **sys** 0.544s;


 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.172s; **sys** 0.304s;
 * Version tube         : **real** 1.673s; **sys** 6.388s;


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.167s; **sys** 0.316s;
 * Version tube         : **real** 24.151s; **sys** 1:27.745s;


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 2.647s; **sys** 2.644s;
 * Version tube         : **real** 4:56.827s; **sys** 8:54.541s;



 
