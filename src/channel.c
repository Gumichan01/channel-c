

#include "channel.h"

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>

// POSIX libraries
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

/// Channel
struct channel
{
    int eltsize;                    // Size of an element
    int size;                       // Number of elements (capacity)
    int flags;
    int closed;
    int nbwriters;
    int nbreaders;

    // Asynchronous channel
    int rd;                         // Read cursor
    int wr;                         // Write cursor
    int nbdata;                     // Current size
    void **data;                    // Buffer
    pthread_mutex_t lock;
    pthread_cond_t cond;

    // Synchronous channel
    int wsync;
    int rsync;
    void *tmp;
    pthread_cond_t sync;
    pthread_cond_t wcond;
    pthread_cond_t rcond;
};


#define CHANNEL_SLEEP 10000

#define CHAN_MAX_ERROR(a,b) \
    ((a > b) ? a : b )

// Check if the channel is shared
#define CHAN_ISSHARED(flags) \
    (((flags) & CHANNEL_PROCESS_SHARED) == CHANNEL_PROCESS_SHARED)

// Check if the channel is configured for batch communication
#define CHAN_ISBATCHED(flags) \
    (((flags) & CHANNEL_PROCESS_BATCH) == CHANNEL_PROCESS_BATCH)


// Check if the channel is configured for single copy
#define CHAN_ISSINGLE(flags) \
    (((flags) & CHANNEL_PROCESS_SINGLE_COPY) == CHANNEL_PROCESS_SINGLE_COPY)

#define CHAN_ISNONBLOCKING(flags) \
    (((flags) & CHANNEL_PROCESS_NONBLOCK) == CHANNEL_PROCESS_NONBLOCK)

// Synchronous communication
int channel_sync_send(struct channel *channel, const void *data);
int channel_sync_recv(struct channel *channel, void *data);

// Non-blocking channel
static int channel_noblock_send(struct channel *channel, const void *data);
static int channel_noblock_recv(struct channel *channel, void *data);


// Internal functions

int channel_closed_empty(struct channel *chan)
{
    return (chan->closed == 1 && chan->nbdata == 0);
}

int channel_is_full(struct channel *chan)
{
    return (chan->rd == chan->wr) && (chan->nbdata == chan->size);
}

int channel_is_empty(struct channel *chan)
{
    return (chan->rd == chan->wr) && (chan->nbdata == 0);
}


// Create the mutex
int channel_mutex_init(struct channel *chan, int flags)
{
    int err;
    pthread_mutexattr_t attrlock;

    if(!CHAN_ISSHARED(flags))
        return pthread_mutex_init(&chan->lock,NULL);

    // Mutex for shared channels
    if((err = pthread_mutexattr_init(&attrlock)) != 0)
        return err;

    err = pthread_mutexattr_setpshared(&attrlock,PTHREAD_PROCESS_SHARED);

    if(err == 0)
        err = pthread_mutex_init(&chan->lock,&attrlock);

    pthread_mutexattr_destroy(&attrlock);
    return err;
}


// Create the condition variable for synchronous channels
int channel_sync_cond_init(struct channel *chan,int flags)
{
    int werr = 0, rerr = 0, serr, err = 0;
    pthread_condattr_t attrcond;

    if(!CHAN_ISSHARED(flags))
    {
        werr = pthread_cond_init(&chan->wcond,NULL);
        rerr = pthread_cond_init(&chan->rcond,NULL);
        serr = pthread_cond_init(&chan->sync,NULL);

        return (werr != 0 || rerr != 0) ? CHAN_MAX_ERROR(werr, rerr) : 0;
    }

    // Condition variable creation for synchronous shared channels
    if((err = pthread_condattr_init(&attrcond)) != 0)
        return err;

    err = pthread_condattr_setpshared(&attrcond,PTHREAD_PROCESS_SHARED);
    werr = pthread_cond_init(&chan->wcond,&attrcond);
    rerr = pthread_cond_init(&chan->rcond,&attrcond);
    serr = pthread_cond_init(&chan->sync,&attrcond);

    pthread_condattr_destroy(&attrcond);

    if(err != 0)
        return err;
    else if (serr != 0)
        return serr;

    return CHAN_MAX_ERROR(werr,rerr);
}

// Create the condition variable for asynchronous channels
int channel_async_cond_init(struct channel *chan,int flags)
{
    int err;
    pthread_condattr_t attrcond;

    if(!CHAN_ISSHARED(flags))
        return pthread_cond_init(&chan->cond,NULL);

    // Condition variable creation for asynchronous shared channels
    if((err = pthread_condattr_init(&attrcond)) != 0)
        return err;

    err = pthread_condattr_setpshared(&attrcond,PTHREAD_PROCESS_SHARED);

    if(err != 0)
        goto async_cond;

    err = pthread_cond_init(&chan->cond,&attrcond);

    async_cond :
    {
        pthread_condattr_destroy(&attrcond);
        return err;
    }
}

int channel_cond_init(struct channel *chan, int flags)
{
    if(chan->size == 0)
        return channel_sync_cond_init(chan,flags);
    else
        return channel_async_cond_init(chan,flags);
}


void channel_mutex_destroy(struct channel *chan)
{
    pthread_mutex_destroy(&chan->lock);
}

void channel_cond_destroy(struct channel *chan)
{
    pthread_cond_destroy(&chan->cond);

    if(chan->size == 0)
    {
        pthread_cond_destroy(&chan->sync);
        pthread_cond_destroy(&chan->wcond);
        pthread_cond_destroy(&chan->rcond);
    }
}


// Allocation of the buffer in asynchronous channels
void ** allocate_array(int eltsize, int size, int flags)
{
    int i, err;
    void ** array = NULL;

    if(CHAN_ISSHARED(flags))
    {
        array = mmap(NULL, sizeof(void*) * size, PROT_READ|PROT_WRITE,
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

    if(CHAN_ISSINGLE(flags))
        goto channel_allocation;

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
                    munmap(array[i--], sizeof(void) * eltsize);
                }

                munmap(array, sizeof(void*) * size);
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

    channel_allocation :
        return array;
}


void free_array(void **array, int eltsize, int size, int flags)
{
    int i;

    if(array == NULL)
        return;

    for(i = 0; i < size; i++)
    {
        if(!CHAN_ISSINGLE(flags))
        {
            if(CHAN_ISSHARED(flags))
            {
                munmap(array[i], sizeof(void) * eltsize);
            }
            else
                free(array[i]);
        }
    }

    if(CHAN_ISSHARED(flags))
    {
        munmap(array, sizeof(void*) * size);
    }
    else
        free(array);
}


// Allocation of the channel
struct channel * channel_allocate(int eltsize, int size, int flags)
{
    int err;
    struct channel *chan = NULL;

    if(CHAN_ISSHARED(flags))
    {
        chan = mmap(NULL, sizeof(struct channel), PROT_READ|PROT_WRITE,
                    MAP_SHARED|MAP_ANONYMOUS, -1, 0);

        if(chan == MAP_FAILED)
            return NULL;
    }
    else
    {
        chan = malloc(sizeof(struct channel));

        if(chan == NULL)
            return NULL;
    }

    if(size > 0)
    {
        // The channel is asynchronous
        chan->data = allocate_array(eltsize,size,flags);

        if(chan->data == NULL)
        {
            goto fail_chan;
        }
    }

    return chan;

    // Deal with errors
    fail_chan :
    {
        err = errno;
        free(chan);
        errno = err;
        return NULL;
    }
}


void channel_free(struct channel *chan, int shared)
{
    if(shared == 1)
    {
        munmap(chan, sizeof(struct channel));
    }
    else
        free(chan);

    chan = NULL;
}

// Now the complete creation of the channel

/*
    Create a channel with a size of an element and a capacity specified by size.

    Return value :
        An allocated channel on success. NULL on failure, and errno is set.

    Notes :
        A synchronous channel cannot be used as a single-copy channel or
        for batch communication.
        A single-copy channel cannot be used for batched communication
        and vice versa.
        A non-blocking channel cannot be used as a single-copy channel
        and vice versa.

*/
struct channel *channel_create(int eltsize, int size, int flags)
{
    int err;
    struct channel *chan = NULL;

    if(eltsize <= 0 || size < 0)
    {
        errno = EINVAL;
        return NULL;
    }

    if(( size == 0 && (CHAN_ISBATCHED(flags) || CHAN_ISSINGLE(flags)
                        || CHAN_ISNONBLOCKING(flags)) )
        || (CHAN_ISBATCHED(flags) && CHAN_ISSINGLE(flags))
        || (CHAN_ISNONBLOCKING(flags) && CHAN_ISSINGLE(flags)))
    {
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
    chan->nbwriters = 0;
    chan->nbreaders = 0;

    chan->rd = 0;
    chan->wr = 0;
    chan->nbdata = 0;

    chan->rsync = 0;
    chan->wsync = 0;

    err = channel_mutex_init(chan,flags);

    if(err != 0)
        goto fail_atomic;

    err = channel_cond_init(chan,flags);

    if(err != 0)
    {
        channel_mutex_destroy(chan);
        goto fail_atomic;
    }

    return chan;

    // Deal with errors while creating mutex or condition variable
    fail_atomic:
    {
        free_array(chan->data,eltsize,size,flags);
        free(chan);
        errno = err;
        return NULL;
    }
}

/*
    Destroy a channel that is already created.

    If the channel is not null but invalid,
    then the behaviour is undefined.
*/
void channel_destroy(struct channel *channel)
{
    if(channel == NULL)
        return;

    channel_cond_destroy(channel);
    channel_mutex_destroy(channel);

    if(channel->size > 0)
        free_array(channel->data,channel->eltsize,channel->size,channel->flags);

    channel_free(channel, CHAN_ISSHARED(channel->flags));
}


/*
    Send data into the channel.

    This function may block if the channel is synchonous.
    It also may block if the channel is asynchronous and full.

    Rerurn Value : 1 on success, -1 on error, and errno is set.

*/
int channel_send(struct channel *channel, const void *data)
{
    if(channel == NULL || data == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(channel->size == 0)
        return channel_sync_send(channel,data);

    if(CHAN_ISNONBLOCKING(channel->flags))
        return channel_noblock_send(channel,data);

    pthread_mutex_lock(&channel->lock);
    if(channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    while(channel_is_full(channel) && channel->closed == 0)
    {
        channel->nbwriters += 1;
        pthread_cond_wait(&channel->cond, &channel->lock);
        channel->nbwriters -= 1;
    }

    if(channel->nbdata >= channel->size || channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    if(CHAN_ISSINGLE(channel->flags))
        channel->data[channel->wr] = (void *) data;
    else
        memcpy(channel->data[channel->wr], data, channel->eltsize);

    if(channel->wr == channel->size-1)
        channel->wr = 0;
    else
        channel->wr += 1;

    channel->nbdata += 1;

    if(channel->nbreaders > 0)
        pthread_cond_signal(&channel->cond);

    pthread_mutex_unlock(&channel->lock);

    return 1;
}

/*
    Receive data from the channel.

    This function may block if the channel is synchonous.
    It also may block if the channel is asynchronous and empty.

    Rerurn Value :  1 on success,
                    0 if the channel is closed and empty,
                    -1 on error, and errno is set.

    Note : In synchronous channels, the function immediately returns if
            the channel is closed. In asynchronous channels, if there are data
            that need to be read, this function can read them even if
            the channel is closed.

*/
int channel_recv(struct channel *channel, void *data)
{
    if(channel == NULL || data == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(channel->size == 0)
        return channel_sync_recv(channel,data);

    if(CHAN_ISNONBLOCKING(channel->flags))
        return channel_noblock_recv(channel,data);

    pthread_mutex_lock(&channel->lock);

    while(channel_is_empty(channel) && channel->closed == 0)
    {
        channel->nbreaders += 1;
        pthread_cond_wait(&channel->cond, &channel->lock);
        channel->nbreaders -= 1;
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

    if(channel->nbwriters > 0)
        pthread_cond_signal(&channel->cond);

    pthread_mutex_unlock(&channel->lock);
    return 1;
}


/*
    Close the current channel

    If the channel is already closed. this function does nothing.
    Otherwise, it closes it to prevent the writers from writing data.

    Return value : 1 on success, 0 is the channel is empty, -1 on error,
                    and errno is set.

*/
int channel_close(struct channel *channel)
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

    if(channel->size > 0)
        pthread_cond_broadcast(&channel->cond);
    else
    {
        pthread_cond_broadcast(&channel->wcond);
        pthread_cond_broadcast(&channel->rcond);
        pthread_cond_broadcast(&channel->sync);
    }

    pthread_mutex_unlock(&channel->lock);

    return 1;
}


/* ********************
    Synchronous channel
   ******************** */

/*
    Send synchronously data to a reader.

    This function may block if no reader is trying to read, or if there is
    already a writer that is synchronizing with another reader.

    Return value : 1 on success, -1 on error and errno is set.

    Note : Actually, the function send -1 if the channel is empty
*/
int channel_sync_send(struct channel *channel, const void *data)
{
    pthread_mutex_lock(&channel->lock);

    while(channel->wsync == 1 && channel->closed == 0)
    {
        channel->nbwriters++;
        pthread_cond_wait(&channel->wcond,&channel->lock);
        channel->nbwriters--;
    }

    if(channel->closed) { goto broken_channel; }
    channel->wsync = 1;

    if(channel->rsync == 0)                             // Wait for the reader
        pthread_cond_wait(&channel->sync,&channel->lock);

    if(channel->closed) { goto broken_channel; }        // Interrupt the operation
    channel->tmp = (void *) data;
    pthread_cond_signal(&channel->sync);                // Data was sent
    pthread_cond_wait(&channel->sync,&channel->lock);

    if(!channel->closed)
    {
        if(channel->nbwriters > 0)
            pthread_cond_signal(&channel->wcond);

        if(channel->nbreaders > 0)
            pthread_cond_signal(&channel->rcond);
    }

    channel->wsync = 0;
    pthread_mutex_unlock(&channel->lock);
    return 1;

    broken_channel :
    {
        channel->wsync = 0;
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }
}

/*
    Receive synchronously data from a writer.

    This function may block if no writer is trying to write, or if there is
    already a reader that is synchronizing with another writer.

    Return value : 1 on success, 0 if the channel is closed.

*/
int channel_sync_recv(struct channel *channel, void *data)
{
    pthread_mutex_lock(&channel->lock);

    while(channel->rsync == 1 && channel->closed == 0)
    {
        channel->nbreaders++;
        pthread_cond_wait(&channel->rcond,&channel->lock);
        channel->nbreaders--;
    }

    channel->rsync = 1;

    if(channel->wsync == 1)
        pthread_cond_signal(&channel->sync);
    else if(channel->closed)
        goto closed_channel;

    pthread_cond_wait(&channel->sync,&channel->lock);   // Wait for the writer
    memcpy(data,channel->tmp,channel->eltsize);

    channel->rsync = 0;
    pthread_cond_signal(&channel->sync);                // Acknowledgement
    pthread_mutex_unlock(&channel->lock);
    return 1;

    closed_channel :
    {
        channel->rsync = 0;
        pthread_mutex_unlock(&channel->lock);
        return 0;
    }
}


/* *********************
    Batch communication
   ********************* */

// Atomic operations called by channel_vsend()
int channel_bsend(struct channel *channel, const void *data)
{
    memcpy(channel->data[channel->wr], data, channel->eltsize);

    if(channel->wr == channel->size-1)
        channel->wr = 0;
    else
        channel->wr += 1;

    channel->nbdata += 1;

    return 1;
}

// Atomic operations called by channel_vrecv()
int channel_brecv(struct channel *channel, void *data)
{
    memcpy(data, channel->data[channel->rd], channel->eltsize);

    if(channel->rd == channel->size-1)
        channel->rd = 0;
    else
        channel->rd += 1;

    channel->nbdata -= 1;

    return 1;
}


/*
    Send at most size elements in the channel using an array.

    It can write a number of elements less than the size of the buffer.
    This functions must not be used if the channel is synchronous or
    not configured for batch communication.

    Return value : a number of written data on success, -1 on error and errno is set.
*/
int channel_vsend(struct channel *channel, const void *array, int size)
{
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

    if(!CHAN_ISBATCHED(channel->flags) || channel->size == 0)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EOPNOTSUPP;
        return -1;
    }

    nbwdata = channel->size - channel->nbdata;
    n = (size > nbwdata) ? nbwdata : size;

    for(i = 0; i < n; i++)
    {
        if(channel_bsend(channel, array + (i * channel->eltsize)) == -1)
        {
            break;
        }
        written += 1;
    }

    if(channel->nbreaders > 0)
        pthread_cond_signal(&channel->cond);

    pthread_mutex_unlock(&channel->lock);
    return written;
}


/*
    Receive at most size elements in the channel using an array.

    It can reade a number of elements less than the size of the buffer.
    This functions must not be used if the channel is synchronous or
    not configured for batch communication.

    Return value : a number of read data on success, -1 on error and errno is set.
*/
int channel_vrecv(struct channel *channel, void *array, int size)
{
    int n, i;
    int read = 0;

    if(channel == NULL || array == NULL || size <= 0)
    {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&channel->lock);

    if(channel->closed == 1 && channel->nbdata == 0){
        pthread_mutex_unlock(&channel->lock);
        return 0;
    }

    if(!CHAN_ISBATCHED(channel->flags) || channel->size == 0)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EOPNOTSUPP;
        return -1;
    }

    n = (size < channel->nbdata) ? size : channel->nbdata;

    for(i = 0; i < n; i++)
    {
        if(channel_brecv(channel, array + (i * channel->eltsize)) == -1)
        {
            break;
        }
        read += 1;
    }

    if(channel->nbwriters > 0)
        pthread_cond_signal(&channel->cond);

    pthread_mutex_unlock(&channel->lock);
    return read;
}


/* **********************
    Non-blocking channel
   ********************** */

static int channel_noblock_send(struct channel *channel, const void *data)
{
    pthread_mutex_lock(&channel->lock);
    if(channel->closed == 1)
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EPIPE;
        return -1;
    }

    if(channel_is_full(channel))
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EWOULDBLOCK;
        return -1;
    }

    memcpy(channel->data[channel->wr], data, channel->eltsize);

    if(channel->wr == channel->size-1)
        channel->wr = 0;
    else
        channel->wr += 1;

    channel->nbdata += 1;
    pthread_mutex_unlock(&channel->lock);

    return 1;
}

static int channel_noblock_recv(struct channel *channel, void *data)
{
    pthread_mutex_lock(&channel->lock);

    if(channel_closed_empty(channel))
    {
        pthread_mutex_unlock(&channel->lock);
        return 0;
    }

    if(channel_is_empty(channel))
    {
        pthread_mutex_unlock(&channel->lock);
        errno = EWOULDBLOCK;
        return -1;
    }

    memcpy(data, channel->data[channel->rd], channel->eltsize);

    if(channel->rd == channel->size-1)
        channel->rd = 0;
    else
        channel->rd += 1;

    channel->nbdata -= 1;
    pthread_mutex_unlock(&channel->lock);
    return 1;
}


static long getCurrentTime()
{
    long ms;
    time_t s;
    struct timespec t;

    clock_gettime(CLOCK_REALTIME, &t);

    s  = t.tv_sec;
    ms = round(t.tv_nsec / 1.0e6);

    return s * 1000 + ms;
}

// Check if a channel is available according to an event given in argument
static int channel_available(struct channel *chan, short ev)
{
    int err = 0;
    int event = 0;

    switch(ev)
    {
        case CHANNEL_EVENT_READ :
        {
            if(!channel_is_empty(chan))
                event = 1;
        }
        break;

        case CHANNEL_EVENT_WRITE :
        {
            if(!channel_is_full(chan))
                event = 1;
        }
        break;

        case CHANNEL_EVENT_NOEVT : break;

        default :   errno = EINVAL; err = 1;
                    break;
    }

    if(err)
        return -1;

    return event;
}

// Check an event and register it in the structure given in argument
static int channel_event(struct channel_set *chset)
{
    int eventup = 0;

    if(chset == NULL || chset->chan == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if((chset->events|CHANNEL_EVENT_READ) == CHANNEL_EVENT_READ)
    {
        eventup = channel_available(chset->chan,CHANNEL_EVENT_READ);

        if(eventup)
        {
            chset->revents = CHANNEL_EVENT_READ;
        }
    }

    if((chset->events|CHANNEL_EVENT_WRITE) == CHANNEL_EVENT_WRITE)
    {
        eventup = channel_available(chset->chan,CHANNEL_EVENT_WRITE);

        if(eventup)
        {
            chset->revents = CHANNEL_EVENT_WRITE;
        }
    }

    return eventup;
}


/*
    Check events from each channel of the channel set given in argument

    Return value : 1 if an event occurred, 0 if no event occurred, -1 on error
                    and errno is set.
*/
int channel_select(struct channel_set *chsets, nchan_t nchannels, int timeout)
{
    nchan_t i;
    int hasevent, stop;
    long current;

    if(chsets == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    stop = 0;
    hasevent = 0;
    current = getCurrentTime();

    while(!stop)
    {
        for(i = 0; i < nchannels; i++)
        {
            hasevent = channel_event(&chsets[i]);
        }

        switch(hasevent)
        {
            case -1 : return 0;

            case 0 :
            {
                if(timeout == CHANNEL_TIME_NOWAIT)
                {
                    // No event
                    return 0;
                }
                else if(timeout > 0 && getCurrentTime() - current > timeout)
                {
                    // No event and timeout expired
                    return 0;
                }
            }
            usleep(CHANNEL_SLEEP);
            break;

            default: return 1;
        }
    }

    // Unreachable
    return hasevent;
}


// Uncomment it to test the implementation

/*int main(void)
{
    int err, q = 16;
    int tab2[] = {2048,16,64,32};
    struct channel *chan = NULL;
    channel_set set;

    chan = channel_create(sizeof(int),1,CHANNEL_PROCESS_BATCH|CHANNEL_PROCESS_NONBLOCK);

    if(chan == NULL)
    {
        perror("create_channel");
        return -1;
    }

    //err = channel_vsend(chan,tab2,4);
    channel_send(chan,&q);

    set.chan = chan;
    set.events = CHANNEL_EVENT_READ;
    set.revents = CHANNEL_EVENT_NOEVT;

    err = channel_select(&set,1,1000);

    if(err == 1 && set.revents == CHANNEL_EVENT_READ)
    {
        channel_recv(chan,&q);
        printf("got : %d\n",q);
    }
    else if(err == 0)
        printf("No event\n");
    else
        perror("channel_select");

    channel_close(chan);
    channel_destroy(chan);
    return 0;
}*/
