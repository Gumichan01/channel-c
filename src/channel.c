

#include "channel.h"
#include "lib/data_queue.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/// Channel
struct channel
{
    /// @todo Implement the channel
    int eltsize;                    // Size of an element
    int size;                       // Number of elements
    int closed;

    struct data_queue_t dataq;      // Linked list of data
    pthread_mutex_t lock;           // mutex for atomic operations
};

typedef struct channel channel_t;


struct channel *channel_create(int eltsize, int size, int flags)
{
    /// @todo create
    return NULL;
}


void channel_destroy(struct channel *channel)
{
    /// @todo destroy
}


int channel_send(struct channel *channel, const void *data)
{
    /// @todo send
    return -1;
}


int channel_close(struct channel *channel)
{
    /// @todo close
    return -1;
}


int channel_recv(struct channel *channel, void *data)
{
    /// @todo close
    return -1;
}


