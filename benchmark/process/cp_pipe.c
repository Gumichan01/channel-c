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


void writefile(char *str){
  int d, err, r;
  char buf[BUFSIZE];
  char file[FILENAME_SIZE];

    strcpy(file, str);
    d = open(file, O_CREAT|O_TRUNC|O_WRONLY, 0600);
    if(d < 0){
      perror("writefile open");
      exit(EXIT_FAILURE);
    }

    do{
      memset(buf, 0, BUFSIZE);
      r = read(pfd[0], buf, BUFSIZE);

    if(r > 0)
      err = write(d, buf, r);
  }while(r > 0 && err != -1);

  close(d);

}


int main(int argc, char **argv){

    pid_t pid;
    pipe(pfd);

    if(argc != 3){
      printf("usage: %s <file_src> <file_dest>\n",argv[0]);
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
      writefile(argv[2]);
      wait(NULL);
    }
    return 0;
}
