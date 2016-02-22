struct channel;

/* flags */
#define CHANNEL_PROCESS_SHARED      1
#define CHANNEL_PROCESS_SYNCHRONOUS 2
#define CHANNEL_PROCESS_NONBLOCK    4
#define CHANNEL_PROCESS_LOCKFREE    8

struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

