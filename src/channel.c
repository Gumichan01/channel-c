

#include "channel.h"

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

/// Channel
struct channel_t
{
    int eltsize;                    // Size of an element
    int size;                       // Number of elements
    int closed;

    void **dataq;                   // Data queue
    pthread_mutex_t lock;           // mutex for atomic operations
    pthread_cond_t cond;            // condition variable
};


void ** allocate_array(int eltsize, int size)
{
    /// @todo allocation
    return NULL;
}


void free_array(int eltsize, int size)
{
    /// @todo free
}


struct channel_t *channel_create(int eltsize, int size, int flags)
{
    /// @todo create
    return NULL;
}


void channel_destroy(struct channel_t *channel)
{
    /// @todo destroy
}


int channel_send(struct channel_t *channel, const void *data)
{
    /// @todo send
    return -1;
}


int channel_close(struct channel_t *channel)
{
    /// @todo close
    return -1;
}


int channel_recv(struct channel_t *channel, void *data)
{
    /// @todo close
    return -1;
}


