
#include "../../channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <pthread.h>

#define BUFSIZE 1024
#define SIZE 8
#define FILENAME_SIZE 256

channel_t *chan = NULL;

void * readfile(void* ptr)
{
  int fd;
  int r, err;
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

    memset(buffer,0,BUFSIZE);
    r = read(fd,buffer,BUFSIZE);
    err = channel_send(chan,buffer);

    if(err == 1)
      perror("read thread - channel_send");

  }while(r > 0 && err != 1);

  channel_close(chan);
  close(fd);
  pthread_exit(NULL);
}

void * writefile(void* ptr)
{
  int f;
  int err;
  char buffer[BUFSIZE];
  char file[FILENAME_SIZE];

  memcpy(file,ptr,FILENAME_SIZE);
  f = open(file,O_CREAT|O_TRUNC|O_WRONLY,0600);

  if(f == -1)
  {
    perror("thread writefile open");
    channel_close(chan);
    pthread_exit(NULL);
  }

  do{
    memset(buffer,0,BUFSIZE);
    err = channel_recv(chan,buffer);

    if(err > 0)
      err = write(f,buffer,BUFSIZE);

  }while(err > 0);

  close(f);
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  pthread_t thr, thp;

  if(argc != 3)
  {
    printf("usage: %s <file_src> <file_dest>\n",argv[0]);
    return EXIT_FAILURE;
  }

  chan = channel_create(BUFSIZE*sizeof(char),SIZE,0);

  if(chan == NULL)
  {
    perror("channel_create");
    return EXIT_FAILURE;
  }

  pthread_create(&thr,NULL,readfile,argv[1]);
  pthread_create(&thp,NULL,writefile,argv[2]);

  pthread_join(thr,NULL);
  pthread_join(thp,NULL);

  channel_destroy(chan);

  return EXIT_SUCCESS;
}
