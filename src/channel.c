

#include "channel.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


/// Linked list of threads
struct thread_t
{
    pthread_t id;
    struct thread_t * next;
};

struct thread_queue_t
{
    struct thread_t * head;
    struct thread_t * queue;
};

/// Linked list of data
struct data_t
{
    void * data;
    struct data_t * next;
};

struct data_queue_t
{
    struct data_t * head;
    struct data_t * queue;
};


/// Channel
struct channel
{
    /// @todo implémenter le channel
    int eltsize;                    // Size of an element
    int size;                       // Number of elements
    int closed;

    struct data_queue_t dataq;      // Linked list of data
    struct thread_queue_t recvq;    // Is that useful?
    struct thread_queue_t sendq;    // Is that useful?

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


