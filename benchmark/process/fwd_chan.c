
#include "../../src/channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE_MSG 64
#define NB_MSG 256
#define MAX_MSG 30000
#define MAX_FWD_MSG 15000

typedef struct msg_t
{
    pid_t pid;
    char content[SIZE_MSG];

}msg_t;

int n = 0;

void receive(channel_t *chan_r)
{
    int err;
    msg_t tmp;

    do{
        err = channel_recv(chan_r,&tmp);

        /*if(err > 0)
            printf("%s",tmp.content);*/

    }while(err > 0);
}

void sendm(channel_t *chan_s)
{
    int err, nb = 0;
    msg_t tmp;

    tmp.pid = getpid();
    sprintf(tmp.content,"Process no %d sent a message!\n",tmp.pid);

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
}

void forward(channel_t *chan_s, channel_t *chan_r)
{
    int err;
    msg_t tmp;

    while((err = channel_recv(chan_s,&tmp)) != 0)
    {
        if(err == -1)
        {
            perror("FWD - error channel_recv");
            break;
        }

        err = channel_send(chan_r,&tmp);

        if(err == -1)
        {
            perror("FWD - error channel_send");
            break;
        }

        n += 1;

        if(n >= MAX_FWD_MSG)
        {
            break;
        }
    }

    channel_close(chan_s);
    channel_close(chan_r);
}

int main(int argc, char **argv)
{
    pid_t pid;
    int i, nb_writers, nb_readers;
    channel_t *chan_s = NULL;
    channel_t *chan_r = NULL;

    if(argc != 3)
    {
        printf("usage : %s <nb_writers> <nb_readers>\n",argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    nb_writers = atoi(argv[1]);
    nb_readers = atoi(argv[2]);
    chan_s = channel_create(sizeof(msg_t),NB_MSG,CHANNEL_PROCESS_SHARED);
    chan_r = channel_create(sizeof(msg_t),NB_MSG,CHANNEL_PROCESS_SHARED);

    if(chan_s == NULL)
        perror("channel_create chan_s");

    if(chan_r == NULL)
        perror("channel_create chan_r");

    printf("Working...\n");
    for(i = 0; i < nb_writers; i++)
    {
        pid = fork();

        if(pid > 0)
        {
            continue;
        }
        else if(pid == 0)
        {
            sendm(chan_s);
            exit(0);
        }
        else
        {
            perror("send fork");
            return -1;
        }
    }

    for(i = 0; i < nb_readers; i++)
    {
        pid = fork();

        if(pid > 0)
        {
            continue;
        }
        else if(pid == 0)
        {
            receive(chan_r);
            exit(0);
        }
        else
        {
            perror("recv fork");
            return -1;
        }
    }

    forward(chan_s, chan_r);
    // Waiting for child processes
    for(i = 0; i < nb_writers; i++){ waitpid(-1,NULL,0); }
    for(i = 0; i < nb_readers; i++){ waitpid(-1,NULL,0); }

    printf("End of program\n");
    channel_destroy(chan_s);
    channel_destroy(chan_r);

    exit(EXIT_SUCCESS);
}
