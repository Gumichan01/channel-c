
#ifndef CHANNEL_H
#define CHANNEL_H

struct channel;
struct channel_set;
typedef unsigned long nchan_t;

/* flags */
#define CHANNEL_PROCESS_SHARED      1
#define CHANNEL_PROCESS_BATCH       2
#define CHANNEL_PROCESS_SINGLE_COPY 4
#define CHANNEL_PROCESS_NONBLOCK    8      // Non-blocking channel (bonus)

// Event type
#define CHANNEL_EVENT_NOEVT 0
#define CHANNEL_EVENT_READ  1
#define CHANNEL_EVENT_WRITE 2

// Time
#define CHANNEL_TIME_WAIT   -1
#define CHANNEL_TIME_NOWAIT  0


struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

int channel_vsend(struct channel *channel, const void *array, int size);
int channel_vrecv(struct channel *channel, void *array, int size);

int channel_select(struct channel_set *chsets, nchan_t nchannels, int timeout);

typedef struct channel channel_t;

typedef struct channel_set
{
    struct channel *chan;
    short events;
    short revents;
} channel_set;

#endif // CHANNEL_H
