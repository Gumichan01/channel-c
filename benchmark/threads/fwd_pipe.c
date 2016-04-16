

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

#define N 2
#define SIZE_MSG 64
#define NB_MSG 256
#define MAX_MSG 30000
#define MAX_FWD_MSG 15000
#define MAX_THREADS 16000

typedef struct msg_t
{
  pid_t pid;
  char content[SIZE_MSG];

}msg_t;

int n = 0;
int fdwriter[N];
int fdreader[N];


void * receive(void * ptr)
{
  int r;
  msg_t tmp;

  while(1)
  {
    r = read(fdreader[0],&tmp,sizeof(msg_t));

    if(r == -1)
    {
      break;
    }
    /*else if(r > 0)
      printf("%s",tmp.content);*/
  }

  pthread_exit(NULL);
}

void * sendm(void * ptr)
{
  int err, nb = 0;
  msg_t tmp;

  tmp.pid = getpid();
  sprintf(tmp.content," %ld @ %d !\n",pthread_self(),tmp.pid);

  while(nb < MAX_MSG)
  {
    err = write(fdwriter[1],&tmp,sizeof(msg_t));

    if(err == -1)
    {
      perror("sendm - write");
      break;
    }
    nb++;
  }

  pthread_exit(NULL);
}

void * forward(void * ptr)
{
  int err, r;
  msg_t tmp;

  while((r = read(fdwriter[0],&tmp,sizeof(msg_t))) > 0)
  {
      err = write(fdreader[1],&tmp,r);

      if(err == -1)
      {
        perror("FWD - error write");
        break;
      }

      n += 1;

      if(n >= MAX_FWD_MSG)
      {
          close(fdwriter[0]);
          close(fdreader[1]);
          break;
      }
  }
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  int i;
  int nb_writers, nb_readers;
  pthread_t th;
  pthread_t thr[MAX_THREADS];
  pthread_t thw[MAX_THREADS];

  if(argc != 3)
  {
    printf("usage : %s <nb_writers> <nb_readers>\n",argv[0]);
    return EXIT_FAILURE;
  }

  srand(time(NULL));
  nb_writers = atoi(argv[1]);
  nb_readers = atoi(argv[2]);

  if(pipe(fdreader) == -1)
  {
      perror("pipe fdreader");
      return EXIT_FAILURE;
  }

  if(pipe(fdwriter) == -1)
  {
      perror("pipe fdwriter");
      close(fdreader[0]);
      close(fdreader[1]);
      return EXIT_FAILURE;
  }

  // Create every threads
  for(i = 0; i < nb_writers; i++)
  {
    pthread_create(&thw[i],NULL,sendm,NULL);
  }

  for(i = 0; i < nb_readers; i++)
  {
    pthread_create(&thr[i],NULL,receive,NULL);
  }

  pthread_create(&th,NULL,forward,NULL);

  printf("Working...\n");
  // Wait for every threads
  for(i = 0; i < nb_writers; i++)
  {
    pthread_join(thw[i],NULL);
  }

  for(i = 0; i < nb_readers; i++)
  {
    pthread_join(thr[i],NULL);
  }

  pthread_join(th,NULL);
  close(fdreader[0]);
  close(fdwriter[1]);

  printf("End of program\n");
  return EXIT_SUCCESS;
}
