

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
    pthread_mutexattr_t attrlock;
    pthread_condattr_t attrcond;
};


#define CHAN_ISSHARED(flags) \
    (( (flags) & CHANNEL_PROCESS_SHARED ) == CHANNEL_PROCESS_SHARED)

#define CHAN_ISBATCHED(flags) \
    (( (flags) & CHANNEL_PROCESS_BATCH ) == CHANNEL_PROCESS_BATCH)

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


int channel_mutex_init(struct channel_t *chan, int flags)
{
    int err = pthread_mutexattr_init(&chan->attrlock);

    if(err != 0)
        return err;

    if(CHAN_ISSHARED(flags))
    {
        err = pthread_mutexattr_setpshared(&chan->attrlock,PTHREAD_PROCESS_SHARED);
        return (err == 0) ? pthread_mutex_init(&chan->lock,&chan->attrlock) : err;
    }

    return pthread_mutex_init(&chan->lock,NULL);
}

int channel_cond_init(struct channel_t *chan, int flags)
{
    int err = pthread_condattr_init(&chan->attrcond);

    if(err != 0)
        return err;

    if(CHAN_ISSHARED(flags))
    {
        err = pthread_condattr_setpshared(&chan->attrcond,PTHREAD_PROCESS_SHARED);
        return (err == 0) ? pthread_cond_init(&chan->cond,&chan->attrcond) : err;
    }

    return pthread_cond_init(&chan->cond,NULL);
}

void channel_mutex_destroy(struct channel_t *chan)
{
    pthread_mutexattr_destroy(&chan->attrlock);
    pthread_mutex_destroy(&chan->lock);
}

void channel_cond_destroy(struct channel_t *chan)
{
    pthread_condattr_destroy(&chan->attrcond);
    pthread_cond_destroy(&chan->cond);
}


void ** allocate_array(int eltsize, int size, int flags)
{
    int i, err;
    void ** array = NULL;

    if(CHAN_ISSHARED(flags))
    {
        array = mmap(NULL, (sizeof(void*) * size), PROT_READ|PROT_WRITE,
                        MAP_SHARED|MAP_ANONYMOUS, -1, 0);

        if(array == MAP_FAILED)
            return NULL;
    }
    else
    {
        array = malloc(sizeof(void*) * size);
        if(array == NULL)
            return NULL;
    }

    memset(array,0,size);

    for(i = 0; i < size; i++)
    {
        if(CHAN_ISSHARED(flags))
        {
            array[i] = mmap(NULL, sizeof(void) * eltsize, PROT_READ|PROT_WRITE,
                            MAP_SHARED|MAP_ANONYMOUS, -1, 0);

            if(array[i] == MAP_FAILED)
            {
                err = errno;
                i -= 1;
                while(i >= 0)
                {
                    munmap(array[i--], (sizeof(void) * eltsize));
                }

                munmap(array, (sizeof(void*) * size));
                errno = err;
                return NULL;
            }
        }
        else
        {
            array[i] = malloc(sizeof(void) * eltsize);

            if(array[i] == NULL)
            {
                err = errno;
                i -= 1;
                while(i >= 0)
                    free(array[i--]);

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
            munmap(array[i], (sizeof(void) * eltsize));
        }
        else
            free(array[i]);
    }

    if(CHAN_ISSHARED(flags))
    {
        munmap(array, (sizeof(void*) * size));
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
      chan = mmap(NULL, sizeof(struct channel_t), PROT_READ|PROT_WRITE,
                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);

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


void channel_free(struct channel_t *chan, int shared)
{
    if(shared == 1)
    {
        munmap(chan, sizeof(struct channel_t));
    }
    else
        free(chan);

    chan = NULL;
}

// Atomic operation called by channel_vsend
int channel_bsend(struct channel_t *channel, const void *data)
{
    memcpy(channel->data[channel->wr], data, channel->eltsize);

    if(channel->wr == channel->size-1)
        channel->wr = 0;
    else
        channel->wr += 1;

    channel->nbdata += 1;

    if(channel->nbdata == 1)
        pthread_cond_broadcast(&channel->cond);

    return 1;
}


int channel_brecv(struct channel_t *channel, void *data)
{
    return 1;
}


/// Public functions

struct channel_t *channel_create(int eltsize, int size, int flags)
{
    int err;
    struct channel_t *chan = NULL;

    if(eltsize <= 0 || size < 0)
    {
        errno = EINVAL;
        return NULL;
    }

    if(size == 0)
    {
        // Synchronous channel
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

    err = channel_mutex_init(chan,flags);

    if(err != 0)
    {
        free_array(chan->data,eltsize,size,flags);
        free(chan);
        return NULL;
    }

    err = channel_cond_init(chan,flags);

    if(err != 0)
    {
        channel_mutex_destroy(chan);
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

    channel_cond_destroy(channel);
    channel_mutex_destroy(channel);
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

    return 1;
}


// If the channel is not null but invalid, then the behaviour is unspecified
int channel_close(struct channel_t *channel)
{
    if(channel == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&channel->lock);
    if(channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        return 0;
    }

    channel->closed = 1;
    pthread_cond_broadcast(&channel->cond);
    pthread_mutex_unlock(&channel->lock);

    return 1;
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

/*
    Send at most size element in the channel using an array.
    It can write a number of elements less than the size of the buffer.
    This functions fails if the given arguments are invalid ot if the channel is
    synchronous or not configured for batch communication.
*/
int channel_vsend(struct channel_t *channel, const void *array, int size)
{
    // TODO fix a warning -> invalid values inserted into the channel
    int n, i;
    int nbwdata, written = 0;

    if(channel == NULL || array == NULL || size <= 0)
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

    if(!CHAN_ISBATCHED(channel->flags))
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EBADE;
        return -1;
    }

    // Test if the call will block
    if(channel->nbdata >= channel->size)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EWOULDBLOCK;
        return -1;
    }

    if(channel->size == 0)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EBADE;
        return -1;
    }

    nbwdata = channel->size - channel->nbdata;
    n = (size > nbwdata) ? nbwdata : size;

    for(i = 0; i < n; i++)
    {
        if(channel_bsend(channel,&array[i*channel->eltsize]) == -1)
        {
            pthread_mutex_unlock(&channel->lock);
            return -1;
        }
        written += 1;
    }

    pthread_mutex_unlock(&channel->lock);
    return written;
}


int channel_vrecv(struct channel_t *channel, void *array, int size)
{
    return 0;
}


// Uncomment it to test the implementation

int main(void)
{
    int err, q;
    int tab[] = {4,1,8};
    int tab2[] = {2048,16,64,32};
    struct channel_t *chan = NULL;
    chan = channel_create(sizeof(int),3,CHANNEL_PROCESS_BATCH);

    if(chan == NULL)
    {
        perror("create_channel");
        return -1;
    }

    err = channel_vsend(chan,tab,3);
    printf("1 - sent %d\n",err);
    err = channel_vsend(chan,tab2,4);
    perror("vsend");
    printf("2 - sent %d\n",err);

    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);
    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);
    channel_recv(chan,&q);
    printf("received from the channel: %d \n",q);

    channel_close(chan);
    channel_destroy(chan);
    return 0;
}
