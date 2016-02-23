

#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
    void **data;                    // Data queue

    pthread_mutex_t lock;           // mutex for atomic operations
    pthread_cond_t cond;            // condition variable
};


void ** allocate_array(int eltsize, int size)
{
    int i;
    void ** array = NULL;

    array = malloc(sizeof(void*) * size);

    if(array == NULL)
        return NULL;

    memset(array,0,size);

    for(i = 0; i < size; i++)
    {
        array[i] = malloc(sizeof(void) * eltsize);

        if(array[i] == NULL)
        {
            i -= 1;
            while(i > 0)
                free(array[i]);

            free(array);
            return NULL;
        }
    }

    return array;
}


void free_array(void **array, int size)
{
    int i;

    for(i = 0; i < size; i++)
    {
        free(array[i]);
    }

    free(array);
}


struct channel_t *channel_create(int eltsize, int size, int flags)
{
    /// @todo create
    int err;

    struct channel_t *chan = NULL;
    chan = malloc(sizeof(struct channel_t));

    if(chan == NULL)
        return NULL;

    chan->eltsize = eltsize;
    chan->size = size;
    chan->flags = flags;
    chan->closed = 0;

    chan->rd = 0;
    chan->wr = 0;

    chan->data = allocate_array(eltsize,size);

    if(chan->data == NULL)
    {
        free(chan);
        return NULL;
    }

    err = pthread_mutex_init(&chan->lock,NULL);

    if(err != 0)
    {
        free_array(chan->data,size);
        free(chan);
        return NULL;
    }

    err = pthread_cond_init(&chan->cond,NULL);

    if(err != 0)
    {
        pthread_mutex_destroy(&chan->lock);
        free_array(chan->data,size);
        free(chan);
        return NULL;
    }

    return chan;
}

/*
    If the channel is invalid or already destoyed,
    then the behaviour is undefined
*/
void channel_destroy(struct channel_t *channel)
{
    /// @todo destroy
    if(channel != NULL)
    {
        pthread_cond_destroy(&channel->cond);
        pthread_mutex_destroy(&channel->lock);
        free_array(channel->data,channel->size);
        free(channel);
        channel = NULL;
    }
}


int channel_send(struct channel_t *channel, const void *data)
{
    /// @todo send
    return -1;
}


// If channel is not null but invalid, the behaviour is unspecified
int channel_close(struct channel_t *channel)
{
    if(channel == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(channel->closed == 1)
        return 0;

    channel->closed = 1;
    return 0;
}


int channel_recv(struct channel_t *channel, void *data)
{
    /// @todo recv
    return -1;
}


// Uncomment it to test the implementation

int main(void)
{
    struct channel_t *chan = NULL;
    chan = channel_create(sizeof(int),8,0);

    sleep(1);

    channel_close(chan);
    channel_destroy(chan);
    return 0;
}

