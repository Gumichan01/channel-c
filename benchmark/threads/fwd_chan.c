
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

    while(1)
    {
        err = channel_recv(chan_r,&tmp);
        if(err <= 0)
        {
            break;
        }
        /*else if(err > 0)
          printf("%s",tmp.content);*/
    }

    pthread_exit(NULL);
}

void * sendm(void * ptr)
{
    int err, nb = 0;
    msg_t tmp;

    tmp.pid = getpid();
    sprintf(tmp.content," %ld @ %d !\n",pthread_self(),tmp.pid);

    while(nb < MAX_MSG)
    {
        err = channel_send(chan_s,&tmp);

        if(err == -1)
        {
            channel_close(chan_s);
            break;
        }
        nb++;
    }

    pthread_exit(NULL);
}

void * forward(void * ptr)
{
    int err;
    msg_t tmp;

    while((err = channel_recv(chan_s,&tmp)) != 0)
    {
        if(err == -1)
        {
            perror("FWD - error channel_recv");
            channel_close(chan_s);
            channel_close(chan_r);
            break;
        }

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
    chan_s = channel_create(sizeof(msg_t),NB_MSG,0);
    chan_r = channel_create(sizeof(msg_t),NB_MSG,0);

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
