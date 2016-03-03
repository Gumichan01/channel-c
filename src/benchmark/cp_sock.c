
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define BUFSIZE 1024
#define FILENAME_SIZE 256

#define SOCK_PATH "/tmp/gumi.socket"
#define LISTEN_BACKLOG 2


void * readfile(void * ptr)
{
  int sock, err;
  socklen_t sz;
  int buffer[BUFSIZE];

  struct sockaddr_un un;

  memset(buffer,0,BUFSIZE);
  sock = socket(PF_UNIX,SOCK_STREAM,0);

  if(sock == -1)
  {
    perror("error socket");
    pthread_exit(NULL);
  }

  // Set the unix socket address
  memset(&un,0,sizeof(struct sockaddr_un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, SOCK_PATH, sizeof(un.sun_path) - 1);

  usleep(1000);
  sz = sizeof(struct sockaddr_un);
  err = connect(sock,(struct sockaddr *) &un, sz);

  if(err == -1)
  {
      perror("error connect");
      close(sock);
      pthread_exit(NULL);
  }

  printf("Connect OK\n");

  close(sock);
  pthread_exit(NULL);
}

void * writefile(void * ptr)
{
  int sock, sockclt, err;
  socklen_t sz;
  int buffer[BUFSIZE];

  struct sockaddr_un un;
  struct sockaddr_un *clt;

  memset(buffer,0,BUFSIZE);
  sock = socket(AF_UNIX,SOCK_STREAM,0);

  if(sock == -1)
  {
    perror("error socket");
    pthread_exit(NULL);
  }

  memset(&un,0,sizeof(struct sockaddr_un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, SOCK_PATH, sizeof(un.sun_path) - 1);

  printf("-> %s \n",un.sun_path);

  /// Bind
  err = bind(sock, (struct sockaddr *) &un, sizeof(struct sockaddr_un));

  if(err == -1)
  {
    perror("error bind");
    unlink(SOCK_PATH);
    close(sock);
    pthread_exit(NULL);
  }

  /// Listen
  err = listen(sock,LISTEN_BACKLOG);

  if(err == -1)
  {
    perror("error listen");
    close(sock);
    pthread_exit(NULL);
  }

  /// Accept
  sz = sizeof(struct sockaddr_un);
  sockclt = accept(sock, (struct sockaddr *) &clt,&sz);
  if (sockclt == -1)
  {
    perror("error accept");
    close(sock);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  printf("Accept OK\n");

  close(sock);
  unlink(SOCK_PATH);

  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  //pthread_t thr;
  pthread_t thr, thw;

  pthread_create(&thr,NULL,readfile,NULL);
  pthread_create(&thw,NULL,writefile,NULL);
  pthread_join(thr,NULL);
  pthread_join(thw,NULL);

  return EXIT_SUCCESS;
}
