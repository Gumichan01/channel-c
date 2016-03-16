
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
int fdw[2];
int fdr[2];

void receive()
{
    int r;
    msg_t tmp;

    do{
        r = read(fdr[0],&tmp,sizeof(msg_t));

        /*if(r > 0)
        {
            tmp.content[r] = '\0';
            printf("%s",tmp.content);
        }*/

    }while(r > 0);
}

void sendm()
{
    int err, nb = 0;
    msg_t tmp;

    memset(&tmp,0,sizeof(msg_t));

    tmp.pid = getpid();
    sprintf(tmp.content,"Process no %d sent a message!\n",tmp.pid);

    while(nb < MAX_MSG)
    {
        err = write(fdw[1],&tmp,sizeof(msg_t));

        if(err == -1)
        {
            break;
        }
        nb++;
    }
}

void forward()
{
    int r, err;
    msg_t tmp;

    while((r = read(fdw[0],&tmp,sizeof(msg_t))) > 0)
    {
        err = write(fdr[1],&tmp,r);

        if(err == -1)
        {
            perror("FWD - error write");
            break;
        }

        n += 1;

        if(n >= MAX_FWD_MSG)
        {
            break;
        }
    }
    close(fdw[0]);
    close(fdw[1]);
    close(fdr[0]);
    close(fdr[1]);
}

int main(int argc, char **argv)
{
    pid_t pid;
    int i, nb_writers, nb_readers;

    if(argc != 3)
    {
        printf("usage : %s <nb_writers> <nb_readers>\n",argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    nb_writers = atoi(argv[1]);
    nb_readers = atoi(argv[2]);

    if(pipe(fdw) == -1)
    {
        perror("fdw - perror");
        return EXIT_FAILURE;
    }

    if(pipe(fdr) == -1)
    {
        perror("fdr - perror");
        return EXIT_FAILURE;
    }

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
            close(fdr[0]);
            close(fdr[1]);
            close(fdw[0]);
            sendm();
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
            close(fdr[1]);
            close(fdw[0]);
            close(fdw[1]);
            receive();
            exit(0);
        }
        else
        {
            perror("recv fork");
            return -1;
        }
    }

    forward();
    // Waiting for child processes
    for(i = 0; i < nb_writers; i++){ waitpid(-1,NULL,0); }
    for(i = 0; i < nb_readers; i++){ waitpid(-1,NULL,0); }

    printf("End of program\n");
    close(fdr[1]);
    close(fdr[0]);
    close(fdw[0]);
    close(fdw[1]);
    exit(EXIT_SUCCESS);
}
