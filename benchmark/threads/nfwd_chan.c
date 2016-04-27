
#include "../../src/channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

#define SIZE_MSG 64
#define NB_MSG 256
#define MAX_MSG 30000
#define MAX_FWD_MSG 15000
#define MAX_THREADS 16000

typedef struct msg_t
{
      pid_t pid;
      char content[SIZE_MSG];

}msg_t;

channel_t *chan_s = NULL;
channel_t *chan_r = NULL;
int n = 0;

void * receive(void * ptr)
{
    int err;
    msg_t tmp;
    channel_set chset;

    chset.chan = chan_r;
    chset.events = CHANNEL_EVENT_READ|CHANNEL_EVENT_CLOSE;

    while(1)
    {
        chset.revents = CHANNEL_EVENT_NOEVT;
        err = channel_select(&chset,1,500);

        if(err == -1)
        {
            perror("recv - channel_select()");
            channel_close(chan_s);
            channel_close(chan_r);
            break;
        }
        else if(err == 0)
            continue;

        if(CHAN_READ_EVT(chset.revents))
        {
            err = channel_recv(chan_r,&tmp);
            if(err <= 0)
            {
                break;
            }
            /*else if(err > 0)
              printf("%s",tmp.content);*/
        }
        else if(CHAN_CLOSE_EVT(chset.revents))
        {
            channel_close(chan_r);
            break;
        }
    }

    pthread_exit(NULL);
}

void * sendm(void * ptr)
{
    channel_set chset;
    int err, nb = 0;
    msg_t tmp;

    tmp.pid = getpid();
    sprintf(tmp.content," %ld @ %d !\n",pthread_self(),tmp.pid);

    chset.chan = chan_s;
    chset.events = CHANNEL_EVENT_WRITE|CHANNEL_EVENT_CLOSE;
    chset.revents = CHANNEL_EVENT_NOEVT;

    while(nb < MAX_MSG)
    {
        chset.revents = CHANNEL_EVENT_NOEVT;

        err = channel_select(&chset,1,CHANNEL_TIME_WAIT);

        if(err == -1)
        {
            perror("channel_select()");
        }
        else if(err == 0)
            continue;

        if(CHAN_WRITE_EVT(chset.revents))
        {
            err = channel_send(chan_s,&tmp);

            if(err == -1)
            {
                channel_close(chan_s);
                break;
            }
            nb++;
        }

        if(CHAN_CLOSE_EVT(chset.revents))
            break;
    }

    pthread_exit(NULL);
}

void * forward(void * ptr)
{
    int err;
    msg_t tmp;
    channel_set s_chset, r_chset;

    s_chset.chan = chan_s;
    s_chset.events = CHANNEL_EVENT_READ|CHANNEL_EVENT_CLOSE;

    r_chset.chan = chan_r;
    r_chset.events = CHANNEL_EVENT_WRITE|CHANNEL_EVENT_CLOSE;

    while(n < MAX_FWD_MSG)
    {
        s_chset.revents = CHANNEL_EVENT_NOEVT;
        r_chset.revents = CHANNEL_EVENT_NOEVT;

        err = channel_select(&s_chset,1,CHANNEL_TIME_WAIT);

        if(err == -1)
        {
            perror("FWD - 1 channel_select()");
            channel_close(chan_s);
            channel_close(chan_r);
            break;
        }
        else if(err == 0)
            continue;

        if(CHAN_READ_EVT(s_chset.revents))
        {
            err = channel_recv(chan_s,&tmp);

            if(err == -1)
            {
                perror("FWD - error channel_recv");
                channel_close(chan_s);
                channel_close(chan_r);
                break;
            }
        }
        else if(CHAN_CLOSE_EVT(s_chset.revents))
        {
            channel_close(chan_r);
            break;
        }

        err = channel_select(&r_chset,1,CHANNEL_TIME_WAIT);

        if(err == -1)
        {
            perror("FWD - 1 channel_select()");
            channel_close(chan_s);
            channel_close(chan_r);
            break;
        }
        else if(err == 0)
            continue;

        if(CHAN_WRITE_EVT(r_chset.revents))
        {
            err = channel_send(chan_r,&tmp);

            if(err == -1)
            {
                perror("FWD - error channel_send");
                channel_close(chan_s);
                channel_close(chan_r);
                break;
            }

            n += 1;

            if(n >= MAX_FWD_MSG)
            {
                channel_close(chan_s);
                channel_close(chan_r);
            }
        }
        else if(CHAN_CLOSE_EVT(r_chset.revents))
        {
            channel_close(chan_s);
            break;
        }
    }

  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    int i, err;
    int nb_writers, nb_readers;
    pthread_t th;
    pthread_t thr[MAX_THREADS];
    pthread_t thw[MAX_THREADS];

    if(argc != 3)
    {
        printf("usage : %s <nb_writers> <nb_readers>\n",argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    nb_writers = atoi(argv[1]);
    nb_readers = atoi(argv[2]);
    chan_s = channel_create(sizeof(msg_t),NB_MSG,CHANNEL_PROCESS_NONBLOCK);
    chan_r = channel_create(sizeof(msg_t),NB_MSG,CHANNEL_PROCESS_NONBLOCK);

    // Create every threads
    for(i = 0; i < nb_writers; i++)
    {
        err = pthread_create(&thw[i],NULL,sendm,NULL);

        if(err != 0)
            perror("pthread_create");
    }

    for(i = 0; i < nb_readers; i++)
    {
        pthread_create(&thr[i],NULL,receive,NULL);
    }

    pthread_create(&th,NULL,forward,NULL);

    printf("Working...\n");

    // Wait for every threads
    for(i = 0; i < nb_writers; i++)
    {
        pthread_join(thw[i],NULL);
    }

    for(i = 0; i < nb_readers; i++)
    {
        pthread_join(thr[i],NULL);
    }

    pthread_join(th,NULL);
    channel_destroy(chan_s);
    channel_destroy(chan_r);
    printf("End of program\n");

    return EXIT_SUCCESS;
}
