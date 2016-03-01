
#include "../channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <pthread.h>

#define BUFSIZE 1024
#define FILENAME_SIZE 256


channel_t *chan = NULL;


void * readfile(void* ptr)
{
  int fd;
  int r;
  char file[FILENAME_SIZE];
  char buffer[BUFSIZE];

  memcpy(file,ptr,FILENAME_SIZE);

  fd = open(file,O_RDONLY);

  if(fd == -1)
  {
    perror("read thread - open");
    channel_close(chan);
    pthread_exit(NULL);
  }

  do{
    int err;
    memset(buffer,0,BUFSIZE);
    r = read(fd,buffer,BUFSIZE);

    // écrire dans le canal
    err = channel_send(chan,buffer);

    if(err == 1)
      perror("channel_send");

  }while(r > 0);

  channel_close(chan);
  close(fd);
  pthread_exit(NULL);
}

void * print(void* ptr)
{
  int err;
  char buffer[BUFSIZE];

  // lire le canal et vérifier

  do{
    memset(buffer,0,BUFSIZE);
    err = channel_recv(chan,buffer);

    if(err != -1)
      write(STDIN_FILENO,buffer,BUFSIZE);

  }while(err != -1);

  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  pthread_t thr, thp;

  if(argc != 2)
  {
    printf("usage: %s <file_name>\n",argv[0]);
    return EXIT_FAILURE;
  }

  chan = channel_create(1024*sizeof(char),1024,0);

  if(chan == NULL)
  {
    perror("channel_create");
    return EXIT_FAILURE;
  }

  pthread_create(&thr,NULL,readfile,argv[1]);
  pthread_create(&thp,NULL,print,NULL);

  pthread_join(thr,NULL);
  pthread_join(thp,NULL);

  channel_destroy(chan);

  return EXIT_SUCCESS;
}
