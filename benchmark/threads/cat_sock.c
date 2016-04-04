#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <pthread.h>

#define BUFSIZE 1024
#define FILENAMESIZE 256

#define SOCK_PATH "/tmp/soquette"
#define LISTEN_BACKLOG 2



void * readfile(void* f){
  int fd, r, s, c;
  int buffer[BUFSIZE];
  char file[FILENAMESIZE];
  socklen_t sz;
  struct sockaddr_un sun;

  memset(buffer, 0, BUFSIZE);
  s = socket(PF_UNIX, SOCK_STREAM, 0);
  if(s < 0){
    perror("socket");
    pthread_exit(NULL);
  }

  memset(&sun, 0, sizeof(struct sockaddr_un));
  sun.sun_family = AF_UNIX;
  strncpy(sun.sun_path, SOCK_PATH, sizeof(sun.sun_path) - 1);

  usleep(1000);
  sz = sizeof(struct sockaddr_un);
  c = connect(s, (struct sockaddr *)&sun, sz);
  if(c < 0){
    perror("connect");
    close(s);
    pthread_exit(NULL);
  }

  memcpy(file, f, FILENAMESIZE);

  fd = open(file, O_RDONLY);
  if(fd < 0){
    perror("open");
    close(s);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }


  while(( r = read(fd, buffer, BUFSIZE)) > 0){
    send(s, buffer, r, MSG_NOSIGNAL);
    memset(buffer, 0, BUFSIZE);
  }
  close(fd);
  close(s);
  pthread_exit(NULL);
}

void * printfile(void* f){
  int s, bd, lt, ac, r;
  int buffer[BUFSIZE];
  socklen_t sz;
  struct sockaddr_un sun;

  memset(buffer, 0, BUFSIZE);

  s = socket(PF_UNIX, SOCK_STREAM, 0);
  if(s < 0){
    perror("socket");
    pthread_exit(NULL);
  }

  memset(&sun, 0, sizeof(struct sockaddr_un));
  sun.sun_family = AF_UNIX;
  strncpy(sun.sun_path, SOCK_PATH, sizeof(sun.sun_path) - 1);

  sz = sizeof(struct sockaddr_un);

  bd = bind(s, (struct sockaddr *) &sun, sz);
  if(bd < 0){
    perror("bind");
    close(s);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  lt = listen(s,LISTEN_BACKLOG);

  if(lt < 0){
    perror("listen");
    close(s);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  ac = accept(s, (struct sockaddr *) &sun, &sz);
  if (ac < 0){
    perror("accept");
    close(s);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  while((r = recv(ac, buffer, BUFSIZE, 0)) > 0){
    write(STDOUT_FILENO, buffer, r);
    memset(buffer, 0, BUFSIZE);
  }
  close(s);
  unlink(SOCK_PATH);
  pthread_exit(NULL);
}

int main(int argc, char ** argv){
  pthread_t thr, thp;
  if(argc != 2){
    printf("usage: %s <file_name>\n",argv[0]);
    return EXIT_FAILURE;
  }

  pthread_create(&thr, NULL, readfile, argv[1]);
  pthread_create(&thp, NULL, printfile, NULL);
  pthread_join(thr, NULL);
  pthread_join(thp, NULL);

  return 0;
}
