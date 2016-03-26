
#include "../src/channel.h"

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
    int err = 1;
    msg_t tmp;

    while(err > 0)
    {
        err = channel_recv(chan_r,&tmp);
        if(err == -1)
        {
            break;
        }
        else if(err > 0)
        {
            fflush(stdout);
            write(STDOUT_FILENO,tmp.content,strlen(tmp.content));
            fflush(stdout);
        }
    }

    pthread_exit(NULL);
}

void * sendm(void * ptr)
{
    int prods = *((int *) ptr);
    int err, nb = 0;
    msg_t tmp;

    tmp.pid = getpid();
    sprintf(tmp.content," %ld @ %d !\n",pthread_self(),tmp.pid);

    while(nb < prods)
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

void forward(int prods, int nb_writers)
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

        usleep(100*1000);

        n += 1;

        if(n >= prods * nb_writers)
        {
            channel_close(chan_s);
            channel_close(chan_r);
            break;
        }
    }
}

void multithread(int nb_prod, int nb_writers, int nb_readers)
{
    int i;
    pthread_t th;
    // Create every threads
    for(i = 0; i < nb_writers; i++)
    {
        pthread_create(&th,NULL,sendm,&nb_prod);
    }

    for(i = 0; i < nb_readers; i++)
    {
        pthread_create(&th,NULL,receive,NULL);
    }

    forward(nb_prod, nb_writers);

    // Wait for every threads
    for(i = 0; i < nb_writers; i++)
    {
        pthread_join(th,NULL);
    }

    for(i = 0; i < nb_readers; i++)
    {
        pthread_join(th,NULL);
    }

}

void multiprocess(int nb_prod, int nb_writers, int nb_readers)
{

}


int main(int argc, char **argv)
{
    int multiproc = 0, sync = 0;
    int nb_writers, nb_readers, nb_prod;
    const char *usage = "./prod -w nbwriters -r nb_readers -n nb_prod [-p] [-s]\n";

    nb_writers = 0;
    nb_readers = 0;
    nb_prod = 0;

    while(1)
    {
        int opt = getopt(argc, argv, "w:r:n:sp");
        if(opt < 0)
            break;

        switch(opt) {
        case 'p':
            multiproc = 1;
            break;
        case 's':
            sync = 1;
            break;

        case 'w':
            nb_writers = atoi(optarg);
            break;

        case 'r':
            nb_readers = atoi(optarg);
            break;

        case 'n':
            nb_prod = atoi(optarg);
            break;

        default:
            fprintf(stderr, "%s\n", usage);
            exit(1);
        }
    }

    if(nb_writers == 0 || nb_readers == 0 || nb_prod == 0)
    {
        fprintf(stderr, "%s\n", usage);
        exit(1);
    }

    srand(time(NULL));
    chan_s = channel_create(sizeof(msg_t),(sync ? 0:nb_prod),
                            (multiproc ? CHANNEL_PROCESS_SHARED : 0));
    chan_r = channel_create(sizeof(msg_t),(sync ? 0:nb_prod),
                            (multiproc ? CHANNEL_PROCESS_SHARED : 0));

    printf("Working...\n");

    if(multiproc)
        multiprocess(nb_prod,nb_writers,nb_readers);
    else
        multithread(nb_prod,nb_writers,nb_readers);

    printf("End of program\n");
    channel_destroy(chan_s);
    channel_destroy(chan_r);

    return EXIT_SUCCESS;
}
