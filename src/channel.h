
#ifndef CHANNEL_H
#define CHANNEL_H

struct channel;

/* flags */
#define CHANNEL_PROCESS_SHARED      1
#define CHANNEL_PROCESS_BATCH       2
#define CHANNEL_PROCESS_PTR         4
#define CHANNEL_PROCESS_NONBLOCK    8      // Non-blocking channel (bonus)


struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

int channel_vsend(struct channel *channel, const void *array, int size);
int channel_vrecv(struct channel *channel, void *array, int size);

typedef struct channel channel_t;

#endif // CHANNEL_H
