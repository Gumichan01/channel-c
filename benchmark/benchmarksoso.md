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
|1/1               |         0.020s        |        **0.028s**        |         0.618s       |         **0.600s**      |
|10/10             |         0.240s        |        **0.536s**        |         0.732s       |         **0.828s**      |
|100/100           |         0.228s        |        **0.436s**        |         0.790s       |         **0.984s**      |
|1000/1000         |         0.308s        |        **0.440s**        |         0.906s       |         **1.168s**      |
|10000/10000       |         12.440s       |        **22.685s**       |         10.326s      |         **17.161s**     |
|10/1              |         0.265s        |        **0.508s**        |         0.653s       |         **0.680s**      |
|100/1             |         0.295s        |        **0.529s**        |         0.667s       |         **0.748s**      |
|1000/1            |         0.252s        |        **0.428s**        |         0.711s       |         **0.904s**      |
|10000/1           |         2.891s        |        **3.004s**        |         1.105s       |         **1.216s**      |
|1/10              |         0.163s        |        **0.328s**        |         0.706s       |         **0.744s**      |
|1/100             |         0.217s        |        **0.428s**        |         0.718s       |         **0.720s**      |
|1/1000            |         0.270s        |        **0.492s**        |         0.748s       |         **0.828s**      |
|1/10000           |         2.772s        |        **2.808s**        |         3.484s       |         **3.872s**      |


## Communication par lots ##

#### Multi-thread ####

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |Par lots (temps réel) |Par lots (temps système) |
|------------------|-----------------------|--------------------------|---------------------|-------------------------|
|1/1               |         0.020s        |        **0.028s**        |         0.015s       |         **0.020s**      |
|10/10             |         0.240s        |        **0.536s**        |         0.271s       |         **0.520s**      |
|100/100           |         0.228s        |        **0.436s**        |         0.315s       |         **0.568s**      |
|1000/1000         |         0.308s        |        **0.440s**        |         0.467s       |         **0.720s**      |
|10000/10000       |         12.440s       |        **22.685s**       |         12.969s      |         **24.002s**     |
|10/1              |         0.265s        |        **0.508s**        |         0.183s       |         **0.372s**      |
|100/1             |         0.295s        |        **0.529s**        |         0.307s       |         **0.596s**      |
|1000/1            |         0.252s        |        **0.428s**        |         0.204s       |         **0.364s**      |
|10000/1           |         2.891s        |        **3.004s**        |         2.895s       |         **3.032s**      |
|1/10              |         0.163s        |        **0.328s**        |         0.016s       |         **0.004**      |
|1/100             |         0.217s        |        **0.428s**        |         0.024s       |         **0.024s**      |
|1/1000            |         0.270s        |        **0.492s**        |         0.038s       |         **0.048s**      |
|1/10000           |         2.772s        |        **2.808s**        |         2.893s       |         **3.008s**      |

  

#### Multi-process ####

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |Par lots (temps réel) |Par lots (temps système) |
|------------------|-----------------------|--------------------------|----------------------|-------------------------|
|1/1               |         0.046s        |        **0.040s**        |         0.047s       |         **0.080s**      |
|10/10             |         0.110s        |        **0.196s**        |         0.363s       |         **0.788s**      |
|100/100           |         0.392s        |        **0.272s**        |         0.409s       |         **0.344s**      |
|1000/1000         |         0.843s        |        **0.556s**        |         0.879s       |         **0.584s**      |
|10/1              |         0.228s        |        **0.520s**        |         0.358s       |         **0.768s**      |
|100/1             |         0.370s        |        **0.560s**        |         0.430s       |         **0.588s**      |
|1000/1            |         0.426s        |        **0.284s**        |         0.731s       |         **0.496s**      |
|10000/1           |         3.530s        |        **3.020s**        |         3.329s       |         **2.876s**      |
|1/10              |         0.129s        |        **0.240s**        |         0.139s       |         **0.288s**      |
|1/100             |         0.358s        |        **0.500s**        |         0.337s       |         **0.484s**      |
|1/1000            |         0.750s        |        **0.536s**        |         0.741s       |         **0.500s**      |
|1/10000           |         3.467s        |        **2.952s**        |         3.700s       |         **3.100s**      |


## Canaux à une seule copie ##

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |1 seule copie (temps réel) |1 seule copie (temps système) |
|------------------|-----------------------|--------------------------|---------------------------|------------------------------|
|1/1               |         0.020s        |        **0.028s**        |           0.043s          |         **0.056s**           |
|10/10             |         0.240s        |        **0.536s**        |           0.157s          |         **0.324s**           |
|100/100           |         0.228s        |        **0.436s**        |           0.191s          |         **0.368s**           |
|1000/1000         |         0.308s        |        **0.440s**        |           0.278s          |         **0.416s**           |
|10000/10000       |         12.440s       |        **22.685s**       |           12.536s         |         **22.509s**          |
|10/1              |         0.265s        |        **0.508s**        |           0.235s          |         **0.488s**           |
|100/1             |         0.295s        |        **0.529s**        |           0.273s          |         **0.560s**           |
|1000/1            |         0.252s        |        **0.428s**        |           0.269s          |         **0.544s**          |
|10000/1           |         2.891s        |        **3.004s**        |           2.974s          |         **3.080s**          |
|1/10              |         0.163s        |        **0.328s**        |           0.262s          |         **0.536s**          |
|1/100             |         0.217s        |        **0.428s**        |           0.297s          |         **0.504s**          |
|1/1000            |         0.270s        |        **0.492s**        |           0.252s          |         **0.464s**          |
|1/10000           |         2.772s        |        **2.808s**        |           2.748s          |         **2.808s**          |




## Canaux non-bloquant ##

|Ecrivains/Lecteurs|Normaux (temps réel)   |Normaux (temps système)   |Non-Bloquant (temps réel) |Non-Bloquant (temps système) |
|------------------|-----------------------|--------------------------|--------------------------|-----------------------------|
|1/1               |         0.020s        |        **0.028s**        |           0.616s         |         **0.008s**          |
|10/10             |         0.240s        |        **0.536s**        |           0.015s         |         **0.008s**          |
|100/100           |         0.228s        |        **0.436s**        |           0.023s         |         **0.020s**          |
|1000/1000         |         0.308s        |        **0.440s**        |           0.071s         |         **0.096s**          |
|10000/10000       |         12.440s       |        **22.685s**       |           43.473s        |         **2:18.477s**       |
|10/1              |         0.265s        |        **0.508s**        |           0.014s         |         **0.004s**          |
|100/1             |         0.295s        |        **0.529s**        |           0.019s         |         **0.018s**          |
|1000/1            |         0.252s        |        **0.428s**        |           0.063s         |         **0.084s**          |
|10000/1           |         2.891s        |        **3.004s**        |           0.290s         |         **0.376s**          |
|1/10              |         0.163s        |        **0.328s**        |           0.606s         |         **0.000s**          |
|1/100             |         0.217s        |        **0.428s**        |           0.608s         |         **0.024s**          |
|1/1000            |         0.270s        |        **0.492s**        |           0.634s         |         **0.436s**          |
|1/10000           |         2.772s        |        **2.808s**        |           55.292s        |         **3:1.135s**        |
