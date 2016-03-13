

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

int pipefd[2];

void * readfile(void* ptr)
{
  int r, fd;
  char file[FILENAME_SIZE];
  char buffer[BUFSIZE];

  memcpy(file,ptr,FILENAME_SIZE);

  fd = open(file,O_RDONLY);

  if(fd == -1)
  {
    perror("read thread - open");
    pthread_exit(NULL);
  }

  do{

    memset(buffer,0,BUFSIZE);

    //printf("readfile : read\n");
    //fflush(stdout);
    r = read(fd,buffer,BUFSIZE);
    //printf("readfile : read done\n");
    //fflush(stdout);

    if(r > 0)
      write(pipefd[1],buffer,r);

  }while(r > 0);

  close(pipefd[1]);
  close(fd);
  pthread_exit(NULL);
}

void * writefile(void* ptr)
{
  int f;
  int r, err;
  char buffer[BUFSIZE];
  char file[FILENAME_SIZE];

  memcpy(file,ptr,FILENAME_SIZE);
  f = open(file,O_CREAT|O_TRUNC|O_WRONLY,0600);

  if(f == -1)
  {
    perror("thread writefile open");
    pthread_exit(NULL);
  }

  do{
    memset(buffer,0,BUFSIZE);
    // read from a pipe
    //printf("writefile : read\n");
    //fflush(stdout);
    r = read(pipefd[0],buffer,BUFSIZE);
    //printf("writefile : read done\n");
    //fflush(stdout);

    if(r > 0)
      err = write(f,buffer,r);

  }while(r > 0 && err != -1);

  close(f);
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  int err;
  pthread_t thr, thp;

  if(argc != 3)
  {
    printf("usage: %s <file_src> <file_dest>\n",argv[0]);
    return EXIT_FAILURE;
  }

  err = pipe(pipefd);

  if(err == -1)
  {
    perror("error pipe");
    return EXIT_FAILURE;
  }

  pthread_create(&thr,NULL,readfile,argv[1]);
  pthread_create(&thp,NULL,writefile,argv[2]);

  pthread_join(thr,NULL);
  pthread_join(thp,NULL);

  close(pipefd[0]);

  return EXIT_SUCCESS;
}
