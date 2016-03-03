
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
  int r, fd;
  int sock, err;
  socklen_t sz;
  int buffer[BUFSIZE];
  char file[FILENAME_SIZE];
  struct sockaddr_un un;

  memset(buffer,0,BUFSIZE);
  sock = socket(PF_UNIX,SOCK_STREAM,0);

  if(sock == -1)
  {
    perror("error socket");
    pthread_exit(NULL);
  }

  // Configure the socket setting the unix socket address
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

  // Read the file and send its content
  memcpy(file,ptr,FILENAME_SIZE);
  fd = open(file,O_RDONLY);

  if(fd == -1)
  {
    perror("write - open");
    close(sock);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  while((r = read(fd,buffer,BUFSIZE)) > 0)
  {
    send(sock,buffer,r,MSG_NOSIGNAL);
    memset(buffer,0,BUFSIZE);
  }

  close(fd);
  close(sock);
  pthread_exit(NULL);
}

void * writefile(void * ptr)
{
  int sock, sockclt;
  int err, r, fd;
  socklen_t sz;
  int buffer[BUFSIZE];
  char file[FILENAME_SIZE];

  struct sockaddr_un un;
  struct sockaddr_un *clt;

  memset(buffer,0,BUFSIZE);
  sock = socket(AF_UNIX,SOCK_STREAM,0);

  if(sock == -1)
  {
    perror("error socket");
    pthread_exit(NULL);
  }

  // Configure the socket
  memset(&un,0,sizeof(struct sockaddr_un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, SOCK_PATH, sizeof(un.sun_path) - 1);

  printf("-> %s \n",un.sun_path);

  /// Bind
  err = bind(sock, (struct sockaddr *) &un, sizeof(struct sockaddr_un));

  if(err == -1)
  {
    perror("error bind");
    close(sock);
    unlink(SOCK_PATH);
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

  // file opening and writing
  memcpy(file,ptr,FILENAME_SIZE);
  fd = open(file,O_CREAT|O_TRUNC|O_WRONLY,0600);

  if(fd == -1)
  {
    perror("write - open");
    close(sock);
    unlink(SOCK_PATH);
    pthread_exit(NULL);
  }

  while((r = recv(sockclt,buffer,BUFSIZE,0)) > 0)
  {
    write(fd,buffer,r);
    memset(buffer,0,BUFSIZE);
  }

  close(fd);
  close(sock);
  unlink(SOCK_PATH);
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  pthread_t thr, thw;

  if(argc != 3)
  {
    printf("usage: %s <file_src> <file_dest>\n",argv[0]);
    return EXIT_FAILURE;
  }

  pthread_create(&thr,NULL,readfile,argv[1]);
  pthread_create(&thw,NULL,writefile,argv[2]);
  pthread_join(thr,NULL);
  pthread_join(thw,NULL);

  return EXIT_SUCCESS;
}
