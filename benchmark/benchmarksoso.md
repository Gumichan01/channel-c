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


## Canaux globaux ##
 
### Programme forward les process ###

##### Autant de lecteurs que d'écrivains #####

 - 1 lecteur / 1 écrivain

 * Version canal        : **real** 0.040s; **sys** 0.048s;
 * Version tube         : **real** 0.023s; **sys** 0.048s;


 - 10 lecteurs / 10 écrivains

 * Version canal        : **real** 0.117s; **sys** 0.212s;
 * Version tube         : **real** 0.229s; **sys** 0.720s;


 - 100 lecteurs / 100 écrivains

 * Version canal        : **real** 0.218s; **sys** 0.148s;
 * Version tube         : **real** 1.708s; **sys** 5.988s;


 - 1 000 lecteurs / 1 000 écrivains

 * Version canal        : **real** 0.650s; **sys** 0.428s;
 * Version tube         : **real** 35.082s; **sys** 1:59.967s;


 - 10 000 lecteurs / 10 000 écrivains
 (Pas de test -> la version canal fait planter la machine)


##### Plus d'écrivains que de lecteurs #####

 - 1 lecteur / 10 écrivains

 * Version canal        : **real** 0.136s; **sys** 0.196s;
 * Version tube         : **real** 0.037s; **sys** 0.056s;


 - 1 lecteur / 100 écrivains

 * Version canal        : **real** 0.186s; **sys** 0.144s;
 * Version tube         : **real** 0.429s; **sys** 1.172s;


 - 1 lecteur / 1 000 écrivains

 * Version canal        : **real** 0.444s; **sys** 0.308s;
 * Version tube         : **real** 1.906s; **sys** 2.980s;


 - 1 lecteur / 10 000 écrivains

 * Version canal        : **real** 3.379s; **sys** 2.816s;
 * Version tube         : **real** 2:20.636s; **sys** 4:35.281s;

##### Plus de lecteurs que d'écrivains #####

 - 10 lecteurs / 1 écrivain

 * Version canal        : **real** 0.132s; **sys** 0.252s;
 * Version tube         : **real** 0.188s; **sys** 0.480s;


 - 100 lecteurs / 1 écrivain

 * Version canal        : **real** 0.190s; **sys** 0.164s;
 * Version tube         : **real** 1.745s; **sys** 4.740s;


 - 1 000 lecteurs / 1 écrivain

 * Version canal        : **real** 0.459s; **sys** 0.292s;
 * Version tube         : **real** 27.670s; **sys** 1:19.589s;


 - 10 000 lecteurs / 1 écrivain

 * Version canal        : **real** 3.471s; **sys** 2.576s;
 * Version tube         : **real** 5:12.422s; **sys** 9:42.376s;


## Canaux synchrones ##


|Ecrivains/Lecteurs|Asynchrone (temps réel)|Asynchrone (temps système)|Synchrone (temps réel)|Synchrone (temps système)|
|------------------|-----------------------|--------------------------|----------------------|-------------------------|
|1/1               |         0.020s        |        **0.020s**        |         0.620s       |         **0.528s**      |
|10/10             |         0.095s        |        **0.184s**        |         0.701s       |         **0.844s**      |
|100/100           |         0.103s        |        **0.220s**        |         0.687s       |         **0.868s**      |
|1000/1000         |         0.246s        |        **0.352s**        |         0.710s       |         **0.988s**      |
|10000/10000       |         12.179s       |        **22.537s**       |         10.788s      |         **18.621s**     |
|10/1              |         0.100s        |        **0.200s**        |         0.532s       |         **0.624s**      |
|100/1             |         0.104s        |        **0.216s**        |         0.591s       |         **0.716s**      |
|1000/1            |         0.151s        |        **0.280s**        |         0.456s       |         **0.624s**      |
|10000/1           |         2.731s        |        **2.872s**        |         1.775s       |         **1.944s**      |
|1/10              |         0.118s        |        **0.236s**        |         0.432s       |         **0.528s**      |
|1/100             |         0.109s        |        **0.228s**        |         0.706s       |         **0.668s**      |
|1/1000            |         0.142s        |        **0.272s**        |         0.580s       |         **0.688s**      |
|1/10000           |         2.734s        |        **2.848s**        |         3.147s       |         **3.380s**      |


## Communication par lots ##

#### Multi-thread ####

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |Par lots (temps réel) |Par lots (temps système) |
|------------------|-----------------------|--------------------------|----------------------|-------------------------|
|1/1               |         0.020s        |        **0.020s**        |         0.011s       |         **0.012s**      |
|10/10             |         0.095s        |        **0.184s**        |         0.100s       |         **0.220s**      |
|100/100           |         0.103s        |        **0.220s**        |         0.223s       |         **0.392s**      |
|1000/1000         |         0.246s        |        **0.352s**        |         0.264s       |         **0.368s**      |
|10000/10000       |         12.179s       |        **22.537s**       |         11.727s      |         **20.713s**     |
|10/1              |         0.100s        |        **0.200s**        |         0.218s       |         **0.388s**      |
|100/1             |         0.104s        |        **0.216s**        |         0.105s       |         **0.216s**      |
|1000/1            |         0.151s        |        **0.280s**        |         0.139s       |         **0.256s**      |
|10000/1           |         2.731s        |        **2.872s**        |         2.919s       |         **3.024s**      |
|1/10              |         0.118s        |        **0.236s**        |         0.012s       |         **0.016s**      |
|1/100             |         0.109s        |        **0.228s**        |         0.011s       |         **0.012s**      |
|1/1000            |         0.142s        |        **0.272s**        |         0.041s       |         **0.052s**      |
|1/10000           |         2.734s        |        **2.848s**        |         1.965s       |         **1.872s**      |

  

#### Multi-process ####

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |Par lots (temps réel) |Par lots (temps système) |
|------------------|-----------------------|--------------------------|----------------------|-------------------------|
|1/1               |         0.020s        |        **0.020s**        |         0.038s       |         **0.056s**      |
|10/10             |         0.095s        |        **0.184s**        |         0.128s       |         **0.240s**      |
|100/100           |         0.103s        |        **0.220s**        |         0.305s       |         **0.192s**      |
|1000/1000         |         0.246s        |        **0.352s**        |         0.873s       |         **0.592s**      |
|10000/10000       |         12.179s       |        **22.537s**       |         -.---s       |         **-.---s**      |
|10/1              |         0.100s        |        **0.200s**        |         0.213s       |         **0.460s**      |
|100/1             |         0.104s        |        **0.216s**        |         0.194s       |         **0.160s**      |
|1000/1            |         0.151s        |        **0.280s**        |         0.466s       |         **0.332s**      |
|10000/1           |         2.731s        |        **2.872s**        |         3.656s       |         **3.072s**      |
|1/10              |         0.118s        |        **0.236s**        |         0.147s       |         **0.308s**      |
|1/100             |         0.109s        |        **0.228s**        |         0.168s       |         **0.156s**      |
|1/1000            |         0.142s        |        **0.272s**        |         0.454s       |         **0.320s**      |
|1/10000           |         2.734s        |        **2.848s**        |         3.439s       |         **2.880s**      |


## Canaux à une seule copie ##

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |1 copie (temps réel) |1 copie (temps système) |
|------------------|-----------------------|--------------------------|---------------------|------------------------|
|1/1               |         0.020s        |        **0.020s**        |         0.606s      |         **0.004s**     |
|10/10             |         0.095s        |        **0.184s**        |         0.014s      |         **0.000s**     |
|100/100           |         0.103s        |        **0.220s**        |         0.020s      |         **0.012s**     |
|1000/1000         |         0.246s        |        **0.352s**        |         0.051s      |         **0.052s**     |
|10000/10000       |         12.179s       |        **22.537s**       |         3.755s      |         **5.872s**     |
|10/1              |         0.100s        |        **0.200s**        |         0.014s      |         **0.004s**     |
|100/1             |         0.104s        |        **0.216s**        |         0.017s      |         **0.012s**     |
|1000/1            |         0.151s        |        **0.280s**        |         0.029s      |         **0.024s**     |
|10000/1           |         2.731s        |        **2.872s**        |         0.224s      |         **0.292s**     |
|1/10              |         0.118s        |        **0.236s**        |         0.605s      |         **0.000s**     |
|1/100             |         0.109s        |        **0.228s**        |         0.606s      |         **0.020s**     |
|1/1000            |         0.142s        |        **0.272s**        |         0.626s      |         **0.392s**     |
|1/10000           |         2.734s        |        **2.848s**        |         9.308s      |         **28.666s**    |