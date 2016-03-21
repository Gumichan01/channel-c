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

int pfd[2];

void readfile(char *str){
  int fd, r;
  char file[FILENAME_SIZE];
  char buf[BUFSIZE];

  strcpy(file, str);
  fd = open(file, O_RDONLY);
  if(fd < 0){
    perror("readfile open");
    exit(EXIT_FAILURE);
  }

  do{
    memset(buf, 0, BUFSIZE);
    r = read(fd, buf, BUFSIZE);
    
    if(r > 0)
      write(pfd[1], buf, r);
  }while(r > 0);

  close(pfd[1]);
  close(fd);
}


void printfile(char *str){
  int r;
  char buf[BUFSIZE];

  do{
    memset(buf, 0, BUFSIZE);
    r = read(pfd[0], buf, BUFSIZE);

    if(r > 0)
      write(STDIN_FILENO, buf, r);

  }while(r > 0);
}


int main(int argc, char **argv){

    pid_t pid;
    pipe(pfd);

    if(argc != 2){
      printf("usage: %s <file>\n",argv[0]);
      return EXIT_FAILURE;
    }

    if((pid = fork()) < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if(pid == 0){
      close(pfd[0]);
      readfile(argv[1]);
      exit(EXIT_SUCCESS);
    }
    else{
      close(pfd[1]);
      printfile(argv[1]);
      wait(NULL);
    }
    return 0;
}
