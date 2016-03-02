#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFSIZE 1024
#define FILENAMESIZE 256

int pfd[2];

void * readfile(void* f){
  int fd, r;
  char file[FILENAMESIZE];
  char buf[BUFSIZE];

  memcpy(file, f, FILENAMESIZE);

  fd = open(file, O_RDONLY);
  if(fd < 0){
    perror("open");
    exit(EXIT_FAILURE);
  }

  do{
    memset(buf, 0, BUFSIZE);

    r = read(fd, buf, BUFSIZE);
    if(r < 0){
      perror("read");
      exit(EXIT_FAILURE);
    }    
  }while(r > 0);

  close(fd);
  pthread_exit(NULL);  
}

void * printfile(void* f){
  char buf[BUFSIZE];
  int r;
  
  do{
    memset(buf, 0, BUFSIZE);
    r = read(pfd[0], buf, BUFSIZE); 
    if(r < 0){
      perror("read");
      exit(EXIT_FAILURE);
    }
    write(STDIN_FILENO, buf, BUFSIZE);
  }while(r > 0);
  
  close(pfd[0]);
  pthread_exit(NULL);
  
}

int main(int argc, char ** argv){
  int err;
  pthread_t thr, thp;
  
  if(argc != 2){
    printf("usage: %s <file_name>\n",argv[0]);
    return EXIT_FAILURE;
  }

  err = pipe(pfd);
  if(err < 0){
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pthread_create(&thr,NULL,readfile,argv[1]);
  pthread_create(&thp,NULL,printfile,NULL);
  pthread_join(thr,NULL);
  pthread_join(thp,NULL);

  close(pfd[1]);
  
  return 0;
}
