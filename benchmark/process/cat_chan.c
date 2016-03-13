
#include "../../src/channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/fcntl.h>


#define BUFSIZE 1024
#define SIZE 8
#define FILENAME_SIZE 64

void readfile(char *ptr,channel_t * chan)
{
    int fd;
    int r, err;
    char file[FILENAME_SIZE];
    char buffer[BUFSIZE];

    strcpy(file,ptr);
    fd = open(file,O_RDONLY);

    if(fd == -1)
    {
        perror("read thread - open");
        channel_close(chan);
        return;
    }

    do{
        memset(buffer,0,BUFSIZE);
        r = read(fd,buffer,BUFSIZE);
        err = channel_send(chan,buffer);

        if(err == -1)
            perror("channel_send");

    }while(r > 0 && err != -1);

    channel_close(chan);
    close(fd);
}

void print(channel_t * chan)
{
    int err;
    char buffer[BUFSIZE];

    do{
        memset(buffer,0,BUFSIZE);
        err = channel_recv(chan,buffer);

        if(err > 0)
            write(STDIN_FILENO,buffer,BUFSIZE);

    }while(err > 0);
}


int main(int argc, char **argv)
{
    channel_t *chan = NULL;
    pid_t pid;

    if(argc != 2)
    {
        printf("usage: %s <file_name>\n",argv[0]);
        return EXIT_FAILURE;
    }

    chan = channel_create(BUFSIZE*sizeof(char),SIZE,CHANNEL_PROCESS_SHARED);

    if(chan == NULL)
    {
        perror("channel_create");
        return EXIT_FAILURE;
    }

    pid = fork();

    if(pid == 0)
    {
        readfile(argv[1],chan);
        exit(EXIT_SUCCESS);
    }
    else if(pid > 0)
    {
        print(chan);
        wait(NULL);
    }
    else
    {
        perror("fork");
    }

    channel_destroy(chan);
    return EXIT_SUCCESS;
}
