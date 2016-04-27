
# Notes #

## Canaux non-bloquants ##

### Spécification ###

- Option ***CHANNEL_PROCESS_NONBLOCK*** pour ce type de canal.

- Lecture/écriture non bloquantes :
> Si le canal est plein en écriture -> retourner -1  
Si le canal est vide en lecture -> retourner -1

- Définir un algorithme pour l'équivalent du *select()*

    int channel_select(channel_set *ch, nchan_t nchannels, int timeout);

NB : Cette function s'inspire plus de la fonction *poll()*,
plus moderne et moins complexe, que de *select()*.

> Structure de channel_set :

    channel_set
    {
        channel_t *chan;
        short event;
        short revents;
    }


*chan* : le canal.  
*event* : l'évenement attendu parmi ceux-là.

    #define CHANNEL_EVENT_NOEVT 0
    #define CHANNEL_EVENT_READ  1
    #define CHANNEL_EVENT_WRITE 2
    #define CHANNEL_EVENT_CLOSE 4

> nchan_t

    typedef unsigned long nchan_t;

> long timeout

Ce sera un entier qui indique le temps en milliseconde.

Il y aura des macros :

    #define CHANNEL_TIME_WAIT   -1  // Pas d'attente
    #define CHANNEL_TIME_NOWAIT  0  // Attente infinie


> code de base pour utiliser *channel_select()* avec un seul canal
(avec plusieurs canaux, utiliser un tableau)

    int n;
    channel_set chset;

    chset.chan = chan_s;
    chset.event = CHANNEL_EVENT_WRITE | CHANNEL_EVENT_READ;
    chset.revents = CHANNEL_EVENT_NOEVT;

    n = channel_select(&chset, 1, CHANNEL_TIME_WAIT);

    if(n > 0)
    {
        if(CHAN_READ_EVT(chset.revents))
        {
            // Receive data from the channel
        }

        if(CHAN_WRITE_EVT(chset.revents))
        {
            // Send data into the channel
        }

        if(CHAN_CLOSE_EVT(chset.revents))
        {
            // The channel is closed,
        }

        // Do something

    }
    else if(n == 0)
    {
        // No event
    }
    else
    {
        // Error
    }


### Algorithme de *channel_select()* ###


> Soient les fonctions génériques en pseudo-code

    getCurrentTime();

Permet d'obtenir le temps courant.

    channel_event_occurred(struct channel *chan,short ev);

**  
Vérifie si un évennement donnée à eu lieu dans un canal donné en paramètre.
 -  1 s'il est disponible selon l'évennement.
 -  0 pas d'evennemet associé.


    channel_poll_event(struct channel_set *chset);

**  
Pour un ensemble d'évennements donnés, vérifie s'il sont disponibles
 - 1 si au moins un evennement a eu lieu.
 - 0 sinon.


> Code de *channel_event_occurred()*

 Variable locale :

    event : int

**begin**

    switch(ev)
    do
        case CHANNEL_EVENT_CLOSE :
        do
            if(channel_is_closed(chan))
            then
                event <- 1
            done
        done

        case CHANNEL_EVENT_READ :
        do
            if(channel_is_empty(chan) = 0)
                event <- 1
        done

        case CHANNEL_EVENT_WRITE :
        do
            if(channel_is_full(chan) = 0 && channel_is_closed(chan) = 0)
                event <- 1
        done

        default case : do nothing
    done

    return event

**end**


> Code de *channel_poll_event()*

Variables locales

    clev : int
    wrev : int
    rdev : int

**begin**

    if((chset->events & CHANNEL_EVENT_CLOSE) = CHANNEL_EVENT_CLOSE)
    then
        clev <- channel_event_occurred(chset->chan,CHANNEL_EVENT_CLOSE);

        if(clev = 1)
        then
            chset->revents <- chset->revents | CHANNEL_EVENT_CLOSE;
        done
    done

    if((chset->events & CHANNEL_EVENT_READ) = CHANNEL_EVENT_READ)
    {
        rdev <- channel_event_occurred(chset->chan,CHANNEL_EVENT_READ);

        if(rdev = 1)
        {
            chset->revents <- chset->revents | CHANNEL_EVENT_READ;
        }
    }

    if((chset->events & CHANNEL_EVENT_WRITE) = CHANNEL_EVENT_WRITE)
    then
        wrev <- channel_event_occurred(chset->chan,CHANNEL_EVENT_WRITE);

        if(wrev = 1)
        then
            chset->revents <- chset->revents | CHANNEL_EVENT_WRITE;
        done
    done

    return clev || rdev || wrev

**end**


> Code de *channel_select()*

Variables locales

    i : nchan_t;            // Variable de boucle
    hasevent : int;
    ev : int;
    current : long;

**begin**

    ev <- 0
    hasevent <- 0
    current  <- getCurrentTime()

    while(1)
    do
        for(i from 0 to nchannels-1, +1)
        do
            ev <- channel_poll_event(&chsets[i])

            if(ev = -1)
            then
                return -1
            else if(ev = 1)
            then
                hasevent <- ev;
            done
        done

        if(hasevent = 0)
        then
            if(timeout = CHANNEL_TIME_NOWAIT)
            then
                // No event
                return 0

            else if(timeout > 0 && (getCurrentTime() - current > timeout))
            then
                // No event and timeout expired
                return 0
            done

            sleep during 10 ms
        else
            return hasevent;    // 1
    done
**end**





> Changement au niveau de l'implémentation du canal

Dans la structure  : rien.

Dans le fichier *.c* : la structure *channel_set*,les macros,
un nouveau type (un alias) + 2 fonctions :

 - *channel_select()*, sans surprise
 - *channel_poll(channel_set *ch, nbchan_t nbchannels)*, boucle *FOR*



--
