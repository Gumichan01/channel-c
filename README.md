
# Projet Système 2016 #

[![Build Status](https://travis-ci.org/Gumichan01/channel-c.svg?branch=master)](https://travis-ci.org/Gumichan01/channel-c)


Etudiants :
 - Somaya Ben Khemis
 - Luxon Jean-Pierre
 - Kenny-Eric Rabarisoanaivo

## Présentation ##

Ce proget consiste en une implémentation d'un moyen de communication entre
différents threads/processus à travers les canaux à la [*Go*](https://golang.org/).

## Fonctionnalités ##

 - Canaux de base pour la communication inter-thread
 - Canaux globaux pour la communication inter-processus
 - Communication par lots
 - Canaux synchrones
 - Canaux à copie unique (expérimental)
 - Canaux non-bloquants

## Compilation ##

Pour générer le fichier bibliothèque associé au canal,
exécutez la commande suivante :

    $ make

Cela générera un fichier *channel.a* qui servira de fichier bibliothèque.

Pour compiler le programme *mandelbrot*, exécutez la commande suivante :

    $ make mandelbrot

En ce qui concerne le benchmark, et le programme exemple,
allez dans l'un des sous-repertoires suivants :
 - *benchmark/threads/*
 - *benchmark/process/*
 - *example/*

Dans chacun de ces repertoires vous trouverez un *Makefile*. La commande :

    $ make

devrait suffir pour compiler l'ensemble des programmes benchmark.
La procédure est identique pour exécuter le programme exemple.


## Benchmark ##

Voir le fichier *benchmark/benchmark.md*.
