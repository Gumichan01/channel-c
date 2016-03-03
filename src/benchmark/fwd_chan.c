
#include "../channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>


void * receive(void * ptr)
{

  pthread_exit(NULL);
}

void * send(void * ptr)
{

  pthread_exit(NULL);
}

void * forward(void * ptr)
{

  pthread_exit(NULL);
}


int main(int argc, char **argv)
{

  return EXIT_SUCCESS;
}
