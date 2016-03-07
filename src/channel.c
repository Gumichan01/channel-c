

#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

/// Channel
struct channel_t
{
    int eltsize;                    // Size of an element
    int size;                       // Number of elements
    int flags;
    int closed;

    int rd;                         // Read cursor
    int wr;                         // Write cursor
    int nbdata;
    void **data;                    // Data queue

    pthread_mutex_t lock;           // mutex for atomic operations
    pthread_cond_t cond;            // condition variable
};


#define CHAN_ISSHARED(flags) \
  (( (flags) & CHANNEL_PROCESS_SHARED ) == CHANNEL_PROCESS_SHARED)

/// Private functions

int channel_closed_empty(struct channel_t *chan)
{
    return (chan->closed == 1 && chan->nbdata == 0);
}

int channel_is_full(struct channel_t *chan)
{
    return (chan->rd == chan->wr) && (chan->nbdata == chan->size);
}

int channel_is_empty(struct channel_t *chan)
{
    return (chan->rd == chan->wr) && (chan->nbdata == 0);
}

void ** allocate_array(int eltsize, int size, int flags)
{
    int i, err;
    void ** array = NULL;

    if(CHAN_ISSHARED(flags))
    {
        // TODO Allocate the shared structure using mmap()
    }
    else
        array = malloc(sizeof(void*) * size);

    if(array == NULL)
        return NULL;

    memset(array,0,size);

    for(i = 0; i < size; i++)
    {
        if(CHAN_ISSHARED(flags))
        {
            // TODO Allocate a shared element using mmap()
        }
        else
        {
            array[i] = malloc(sizeof(void) * eltsize);

            if(array[i] == NULL)
            {
                err = errno;
                i -= 1;
                while(i > 0)
                    free(array[i]);

                free(array);
                errno = err;
                return NULL;
            }
        }
    }

    return array;
}

void free_array(void **array, int eltsize, int size, int flags)
{
    int i;

    if(array == NULL)
      return;

    for(i = 0; i < size; i++)
    {
        if(CHAN_ISSHARED(flags))
        {
            // TODO Free the shared element using munmap()
        }
        else
            free(array[i]);
    }

    if(CHAN_ISSHARED(flags))
    {
        // TODO Free the shared structure using munmap()
    }
    else
        free(array);
}


struct channel_t * channel_allocate(int eltsize, int size, int flags)
{
  int err;
  struct channel_t *chan = NULL;

  if(CHAN_ISSHARED(flags))
  {
      // TODO allocate a shared channel -> CHANNEL_PROCESS_SHARED
      chan = MAP_FAILED;

      if(chan == MAP_FAILED)
        return NULL;
  }
  else
  {
      chan = malloc(sizeof(struct channel_t));

      if(chan == NULL)
        return NULL;
  }

  chan->data = allocate_array(eltsize,size,flags);

  if(chan->data == NULL)
  {
      err = errno;
      free(chan);
      errno = err;
      return NULL;
  }

  return chan;
}

void channel_free(struct channel_t *chan,int shared)
{
    if(shared == 1)
    {
        // TODO Free the shared structure using munmap()
    }
    else
        free(chan);

    chan = NULL;
}


/// Public functions

struct channel_t *channel_create(int eltsize, int size, int flags)
{
    int err;
    struct channel_t *chan = NULL;

    if(size == 0 || CHAN_ISSHARED(flags))
    {
        // Synchronous channel || Shared channel
        errno = ENOSYS;
        return NULL;
    }

    chan = channel_allocate(eltsize,size,flags);

    if(chan == NULL)
        return NULL;

    chan->eltsize = eltsize;
    chan->size = size;
    chan->flags = flags;
    chan->closed = 0;

    chan->rd = 0;
    chan->wr = 0;
    chan->nbdata = 0;

    err = pthread_mutex_init(&chan->lock,NULL);

    if(err != 0)
    {
        free_array(chan->data,eltsize,size,flags);
        free(chan);
        return NULL;
    }

    err = pthread_cond_init(&chan->cond,NULL);

    if(err != 0)
    {
        pthread_mutex_destroy(&chan->lock);
        free_array(chan->data,eltsize,size,flags);
        free(chan);
        return NULL;
    }

    return chan;
}

/*
    If the channel is not null but invalid,
    then the behaviour is undefined.
*/
void channel_destroy(struct channel_t *channel)
{
    if(channel == NULL)
      return;

    pthread_cond_destroy(&channel->cond);
    pthread_mutex_destroy(&channel->lock);
    free_array(channel->data,channel->eltsize,channel->size,channel->flags);
    channel_free(channel, CHAN_ISSHARED(channel->flags));
}


int channel_send(struct channel_t *channel, const void *data)
{
    if(channel == NULL || data == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&channel->lock);
    if(channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    while(channel_is_full(channel) && channel->closed == 0)
    {
        pthread_cond_wait(&channel->cond, &channel->lock);
    }

    if(channel->nbdata >= channel->size || channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    memcpy(channel->data[channel->wr], data, channel->eltsize);
    if(channel->wr == channel->size-1)
      channel->wr = 0;
    else
      channel->wr += 1;

    channel->nbdata += 1;

    if(channel->nbdata == 1)
      pthread_cond_broadcast(&channel->cond);

    pthread_mutex_unlock(&channel->lock);

    return 0;
}


// If channel is not null but invalid, then the behaviour is unspecified
int channel_close(struct channel_t *channel)
{
    if(channel == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(channel->closed == 1)
        return 0;

    pthread_mutex_lock(&channel->lock);
    channel->closed = 1;
    pthread_mutex_unlock(&channel->lock);

    return 0;
}


int channel_recv(struct channel_t *channel, void *data)
{
    if(channel == NULL || data == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&channel->lock);

    while(channel_is_empty(channel) && channel->closed == 0)
    {
        pthread_cond_wait(&channel->cond, &channel->lock);
    }

    if(channel_closed_empty(channel))
    {
        pthread_mutex_unlock(&channel->lock);
        return 0;
    }

    if(channel->nbdata == 0)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    memcpy(data, channel->data[channel->rd], channel->eltsize);

    if(channel->rd == channel->size-1)
      channel->rd = 0;
    else
      channel->rd += 1;

    channel->nbdata -= 1;

    if(channel->nbdata == channel->size-1)
    {
        pthread_cond_broadcast(&channel->cond);
    }

    pthread_mutex_unlock(&channel->lock);
    return 1;
}

// Uncomment it to test the implementation

/*int main(void)
{
    int n = 4, m = 1, p = 8, q;
    struct channel_t *chan = NULL;
    chan = channel_create(sizeof(int),5,0);

    if(chan == NULL)
    {
        perror("create_channel");
        return -1;
    }

    channel_send(chan,&n);
    channel_send(chan,&m);
    channel_send(chan,&p);

    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);  // 4
    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);  // 1
    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);  // 8

    channel_close(chan);
    channel_destroy(chan);
    return 0;
}*/
